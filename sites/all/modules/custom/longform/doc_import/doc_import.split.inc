<?php

/**
 * File: doc_import.split.inc
 * These functions are responsible for parsing, splitting, and storing
 * documents into topics.
 */


/*
 * Converts the raw_html_[NID] file to an array of sections split at h1/h2/h3. Runs preprocess functions on the file (merging FM and Word differences)
 * and saves array as json_encoded data.
 * Returns jscon_encoded data or FALSE.
 */
function doc_import_split() {
  module_load_include('inc', 'doc_import');
  module_load_include('inc', 'doc_manage', 'doc_manage.relation');
  module_load_include('inc', 'doc_manage', 'doc_manage.workflow');
  module_load_include('inc', 'doc_import', 'htmLawed.lib');

  $ticket = longform_entities_get_entity();
  $wrap = entity_metadata_wrapper($ticket->type, $ticket);

  $final_directory = "public://longform/import-source-files/" . $wrap->getIdentifier() . "/";
  $source_tool = $wrap->field_source_tool->value();

  if (!isset($ticket->field_preprocessed_html)) {
    drupal_set_message('There is no raw HTML file to work with. Nothing was done.', 'error');
  }

  $content = file_get_contents($wrap->field_preprocessed_html->value()['uri']);
  if (strlen(trim($content)) == 0) {
    drupal_set_message('The raw HTML file is empty. There was likely an error in the unpacking process.
				Try unpacking the archive again; if that fails, contact Eric Lotze for help.', 'error');

    return FALSE;
  }

  // Adjust headings
  _doc_import_reorder_headings($content, $source_tool);

  // Document preprocessing

  if ($source_tool == 'msword') { //word
    $content = doc_import_word_preprocess(extract_body($content));
  }

  if ($source_tool == 'frame' || $source_tool == 'mif2go') { //frame or mif2go
    $content = doc_import_frame_prepare($content, $source_tool);
  }

  extract_body($content);
  // Strip existing <div> elements

  if (strpos($content, '</div') !== FALSE) {
    $content = preg_replace('/<\/*?div.*?>/i', '', $content);
  }
  // if there are no heading 1s in the content, bump everything up a level
  if (stripos($content, '</h1>') === FALSE) {
    //return false;
    drupal_set_message('No head1 tags are in this document. Shifting tags up one level (h2=>h1...).', 'warning');
    foreach (array(2, 3, 4, 5, 6) as $heading) {
      $head_tag['old'] = 'h' . $heading;
      $head_tag['new'] = 'h' . ($heading - 1);
      $content = str_ireplace('<' . $head_tag['old'], '<' . $head_tag['new'], $content);
      $content = str_ireplace('</' . $head_tag['old'] . '>', '</' . $head_tag['new'] . '>', $content);
    }

    // if, after that, there are still no H1 elements, abandon ship.
    if (stripos($content, '</h1>') === FALSE) {
      drupal_set_message('Unable to safely promote headings to reach a Head1. Check your source file to make sure the highest heading level is either Heading 1 or Heading 2.', 'error');

      return FALSE;
    }
  }

  // If there are head1s present, but a head2 comes first, insert a new section before the first H2.
  $first_h1 = stripos($content, '<h1');
  $first_h2 = stripos($content, '<h2');
  if ($first_h2 < $first_h1) {
    $insert = '<h1 class="inserted">Introduction</h1>';
    $content = substr($content, 0, $first_h2) . $insert . substr($content, $first_h2);
    drupal_set_message('This document\'s structure contains Head2 elements without a preceding Head1. These are most likely introductory sections.
									Since a leading Heading 1 is required for navigation, a blank Heading 1 called "Introduction" has been inserted immediately prior to the first Heading 2.
									This title may be changed prior to publication.', 'warning');
  }

  // To avoid causing querypath problems with sections that contain no content, inject an temporary placeholder after each heading level.
  $insert = '<p class="placeholder">===REMOVE THIS PLACEHOLDER===</p>';
  $content = str_ireplace(array(
      '</h1>',
      '</h2>',
      '</h3>'
    ), array(
      '</h1>' . $insert,
      '</h2>' . $insert,
      '</h3>' . $insert
    ), $content);

  // Split document into topics
  doc_import_split_headings($content, $content_sectioned, 1, 3, $source_tool);

  // Pull together an array to summarize progress. This is for debugging purposes only at present.
  $one = 0;
  $two = 0;
  $three = 0;

  foreach ($content_sectioned as $bit) {
    $summary[] = $bit['level'];
    switch ($bit['level']) {
      case 1:
        $one++;
        break;
      case 2:
        $two++;
        break;
      case 3:
        $three++;
        break;
    }
  }
  //dpm($content_sectioned,'content sectioned');
  if (empty($content_sectioned)) {
    drupal_set_message('An error occurred in _doc_import_split_headings.', 'error');

    return FALSE;
  }

  // Set up batch
  $operations[] = array(
    '_doc_import_save_topics_operation',
    array($content_sectioned, $ticket)
  );
  $batch = array(
    'operations' => $operations,
    'file' => drupal_get_path('module', 'doc_import') . '/doc_import.split.inc',
    'finished' => '_doc_import_save_topics_finished',
    'title' => t('Creating topics...'),
    'init_message' => t('Initializing...'),
    'progress_message' => t('Finished @current job of @total.'),
    'error_message' => t('Error while saving topics.'),
  );

  batch_set($batch);
  batch_process();
}

/**
 * Batch operation to save topic nodes
 */
function _doc_import_save_topics_operation($content_array, $ticket, &$context) {
  if (!$ticket) {
    watchdog('IMPORT', 'No controller passed to doc_import_save_topics_operation', 'error');
  }
  if (empty($context['sandbox'])) {
    global $language_content;
    $context['sandbox'] = array();
    $context['sandbox']['language'] = isset($ticket->language) ? $ticket->language : $language_content;
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_topic'] = 0;
    $context['results']['ticket'] = $ticket;

    // Save topic count for the termination message.
    $context['sandbox']['max'] = count($content_array);
  }
  // Retrieve the next group of sections
  $result = array_slice($content_array, $context['sandbox']['current_topic'], 1); //last parameter is $limit. Caps number of jobs that can be run at once.

  foreach ($result as $section) {
    $topic = new stdClass();
    $topic->type = 'topic';
    node_object_prepare($topic);
    $topic_wrap = entity_metadata_wrapper('node', $topic);
    //add topic-specific fields
    $topic_wrap->language($context['sandbox']['language'])->title_field->set(html_entity_decode($section['title'], ENT_QUOTES | 'ENT_HTML401', 'UTF-8'));
    $topic_wrap->field_topic_source_content->value->set($section['content']);
    $topic_wrap->field_topic_source_content->format->set('full_html');
    $topic_wrap->field_topic_source_weight->set($section['id']);
    $topic_wrap->field_topic_source_heading->set($section['level']);
    $topic_wrap->language = $ticket->language;

    //dpm($section['anchors'],'targets to save');
    if (!empty($section['anchors'])) {
      $targets = json_encode($section['anchors']);

      $topic_wrap->field_topic_anchors->set($targets);
    }

    //add fields from controller: get fields that exist both in the import_ticket and topic bundles.
    $target_fields = array_filter(
      field_info_field_map(),
      function ($array) {
        if (in_array('import_ticket', $array['bundles']['node']) && in_array('topic', $array['bundles']['node'])) {
          return TRUE;
        }
        else {
          return FALSE;
        }
      });

    foreach ($target_fields as $name => $field) {
      $topic_wrap->$name->set($ticket->$name);
    }

    //all fields have been added, so save the new node. Add the resulting nid to the relation array builder.
    $topic_wrap->save();

    // Create the ticket <--> topic relation
    longform_relation_create('longform_ticket', $ticket->tid, 'node', $topic_wrap->getIdentifier(), 'ticket_topic');

    $context['results']['endpoints'] = $topic_wrap->getIdentifier();

    // Update our progress information.
    $context['sandbox']['progress']++;
    $context['sandbox']['current_topic']++;
    $context['message'] = 'Creating topic ' . $context['sandbox']['current_topic'] . ' of ' . $context['sandbox']['max'] . ': ' . $section['title'];
  }

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
  //_batch_example_update_http_requests();
}


function _doc_import_save_topics_finished($success, $results, $operations) {
  if ($success) {
    $endpoints = $results['endpoints'];
    $ticket = $results['ticket'];
    $ticket_state = longform_workflow_set_state($ticket, 'tid', 'convert', NULL, 'State set by importer');
    $message = count($results) . ' tasks processed. ' . count($endpoints) . ' topics were created.';
    $message .= theme('item_list', $results);
    drupal_set_message($message);
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array(
        '%error_operation' => $error_operation[0],
        '@arguments' => print_r($error_operation[1], TRUE)
      ));
    drupal_set_message($message, 'error');
  }

}

/**
 * Returns a comma-delimited string of the possible heading split points, e.g.
 * "H1, H2, H3, H4"
 *
 */
function get_split_points($highest_split_level) {

  // Define break points
  $header_list = array();

  // Construct string of legal break headers. Will be all from H1 through h(current).
  for ($i = 1; $i <= $highest_split_level; $i++) {
    $header_list[] = 'h' . $i;
  }
  $break_at_headers = implode(', ', $header_list);

  return $header_list;

}


/**
 * Collects the information gathered from the $haystack QP object into an array
 * and pushes it onto the end of the $content_array array.
 *
 */
function push_section($title, $links, $contents, $heading_level, &$content_array = array()) {

  //skip sections with no title (empty title tags). These are authoring errors.
  $title_trim = htmlentities(trim(str_replace("  ", " ", str_replace(array(
            "\r",
            "\n",
            "\r\n",
            "\n\r"
          ), " ", $title))));
  if ($title_trim == '&Acirc;&nbsp;' || $title_trim == "&amp;nbsp;") {
    drupal_set_message('Skipping section with no text in heading tag.', 'warning');

    return;
  }

  //clear out added content shims

  $contents = extract_body(htmlqp($contents)->top()->html());
  unprotect_special_characters($contents);
  $section['level'] = $heading_level;
  $section['title'] = $title;
  $section['anchors'] = $links;

  $last = isset($content_array) ? end($content_array) : NULL;
  if (!empty($last)) {
    $section['id'] = $last['id'] + 1;
  }
  else {
    $section['id'] = 0;
  }
  $section['content'] = preg_replace(
    array('/<p class="placeholder"><\/p>/', '/<div id=".*?"/'),
    array('', '<div id="section-' . $section['id'] . '"'),
    $contents); //removes placeholder <p> tags

  $content_array[] = $section;
}


/**
 * Given a QueryPath object, attempts to separate an enclosed link from the
 * enclosed text. Returns a two-element array. For example, "<h3><a
 * name='xyz'>Hi mom</a></h3>" returns (array)('title'=>'Hi mom', 'target'=>
 * array('xyz', '123'))
 */
function _doc_import_separate_heading_links($content) {
  if (stripos($content, '</h1>') === FALSE && stripos($content, '</h2>') === FALSE && stripos($content, '</h3>') === FALSE) {
    return FALSE;
  }

  if (stripos($content, '</a>') === FALSE) {
    return array(
      'title' => _doc_import_get_heading_text($content),
      'target' => NULL
    );
  }
  else {
    $return['title'] = _doc_import_get_heading_text($content);
    $heading_links = _doc_import_get_heading_links($content);

    $heading_links ? $return['anchors']['head'] = $heading_links : $return['anchors']['head'] = array();

  }

  return $return;
}

/**
 * Takes a text string of HTML and removes HTML tags, returning the bare text
 */
function _doc_import_get_heading_text($content) {

  // Strip HTML tags from raw heading
  $regex = "/<\/*?(?:a|b|h|span|br\s?\/|sup).*?>/ui";
  $content = preg_replace($regex, '', $content);

  // Strip preface text (anything before a colon, like "chapter 1:")

  if (strpos($content, ':') !== FALSE) {
    $split = preg_split('/:/iu', $content);
    array_shift($split);
    $content = trim(implode(':', $split));
  }
  // Convert HTML codes and return string

  $content = unprotect_special_characters(html_entity_decode($content, ENT_QUOTES | 'ENT_HTML401', 'UTF-8'));

  // Re-convert less-than symbols to prevent them from being interpreted as tags
  $content = str_replace('<', '&lt;', $content);

  return $content;

}

/**
 * Takes a text string of HTML and returns an array of link names present.
 * Strips "_Tocxxxx" links.
 */
function _doc_import_get_heading_links($content) {
  $regex = "/name=\"(.*?)\"/ui";
  $content = unprotect_special_characters(html_entity_decode($content, ENT_QUOTES | 'ENT_HTML401', 'UTF-8'));
  preg_match_all($regex, $content, $out);
  $out = array_filter($out[1], function ($e) {
    if (stripos($e, '_toc') === FALSE) {
      return TRUE;
    }
  });

  return $out;
}

/**
 * Takes a string of content and returns the first topic, as defined by
 * $current_split and $splits.
 */
function _doc_import_parse_topic($content, $current_split, $splits) {

  $stop = 0;
  foreach ($splits as $split) {
    $tag = '<' . $split;
    $position = stripos($content, $tag);

    if ($position === FALSE) {
      continue;
    }

    if ($position > $stop) {
      $stop = $position;
      break;
    }
  }

  if ($stop == 0) {
    $return['this'] = trim($content);
    $return['after'] = NULL;
  }
  else {
    $return['this'] = trim(substr($content, 0, $stop));
    $return['after'] = trim(substr($content, $stop));
  }

  return $return;
}


/**
 * Takes a string of content and splits it into components. Returns an array of
 * content.
 */

function _doc_import_parse_section($content, $current_split, $splits) {
  //dpm((array)$content,'content to parse');
  $splits = implode('|', $splits);
  $current_split = 'h' . $current_split;

  $regex = "/<(?:$splits).*?>(?:.*?)<\/(?:$splits)>/isu";

  preg_match_all($regex, $content, $matches);

  $positions = array();
  $i = 0;

  foreach ($matches[0] as $key => $match) {
    $positions[$key]['heading'] = strpos($content, $match);
    $positions[$key]['start'] = $positions[$key]['heading'] + strlen($match);
    if ($i == count($matches[0]) - 1) {
      continue;
    }
    $positions[$key]['length'] = strpos($content, $matches[0][$i + 1]) - $positions[$key]['start'];
    $i++;
  }
  $positions[count($matches[0]) - 1]['length'] = strlen($content) - $positions[count($matches[0]) - 1]['start'];

  $return = array();

  foreach ($matches[0] as $key => $match) {
    $return[$key]['header'] = $match;
    $return[$key]['content'] = substr($content, $positions[$key]['start'], $positions[$key]['length']);
  }

  return $return;
}

function doc_import_split_headings($content_chunk, &$content_array = array(), $current_split_level = 1, $highest_split_level = 1, $tool = NULL, &$counter_1 = 0, &$counter_2 = 0, &$counter_3 = 0) {

  $section_break_tag = 'h' . $current_split_level;

  $sections = _doc_import_parse_section($content_chunk, $current_split_level, (array) ('h' . $current_split_level));

  //Loop through each chunk iteratively (all H1s, then H2s inside each H1, etc.)
  foreach ($sections as $section) {
    //Increment counters for div ID #s, then construct the ID term to attach to the section DIV
    switch ($current_split_level) {
      case 1:
        $counter_1++;
        if ($highest_split_level > 1) { //If we will be going deeper than this level, reset the next-higher counter
          $counter_2 = 1;
        }
        break;
      case 2:
        $counter_2++;
        if ($highest_split_level > 2) { //If we will be going deeper than this level, reset the next-higher counter
          $counter_3 = 1;
        }
        break;
      case 3:
        $counter_3++;
        break;
    }
    $section_id = $counter_1 . '-' . $counter_2 . '-' . $counter_3;
    $head = _doc_import_separate_heading_links($section['header']);
    $section_title = $head['title'];
    $section_links = $head['anchors'];

    //$section_title = $process_section->text();
    html_entity_decode(unprotect_special_characters($section_title));

    if ($current_split_level < $highest_split_level) {
      $split_points = get_split_points($highest_split_level);
    }
    else {
      if ($current_split_level == $highest_split_level) {
        $split_points = get_split_points($current_split_level);
      }
    }

    $css = "stylesheet-1-0";

    //Parse the current section, and add its content to the $content_array stack (content = :root to first break point)

    $topic = _doc_import_parse_topic($section['content'], $current_split_level, $split_points);
    $prefix = '<div id="section-' . $section_id . '" class="document-content heading-level-' . $current_split_level . ' ' . $css . '">' . PHP_EOL;
    $suffix = PHP_EOL . '</div>';

    push_section($section_title, $section_links, ($prefix . $topic['this'] . $suffix), $current_split_level, $content_array);

    //If we are not at the highest split level, increment the split level counter and recurse deeper into the section
    if ($current_split_level < $highest_split_level) {
      $current_split_level++;
      doc_import_split_headings($topic['after'], $content_array, $current_split_level, $highest_split_level, $tool, $counter_1, $counter_2, $counter_3);
      $current_split_level--;
    }
  }
}

/**
 * Performs initialization functions before splitting content
 */
function doc_import_initialize_import($ticket) {

  $data_directory = 'public://import-source-files/' . $ticket->nid . '/log';

  //clear out pre-existing link file
  file_unmanaged_delete($data_directory . '/links');

}

/**
 * Correct Word-generated HTML
 */
function doc_import_word_preprocess($content) {

  //flip spaces between consecutive <span> tags backwards so they're included at the end of the previous <span> instead of orphaned in the middle
  $content = str_replace('</span> <span', ' </span><span', $content);

  //remove hard-coded list styling. Number removal is done via regex later.
  $content = str_replace('<span>&middot;<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span></span>', '', $content);
  $content = str_replace('<span>&#9675;<span>&nbsp;</span></span>', '', $content);

  return $content;
}


/**
 * Prepare Frame HTML for splitting
 */
function doc_import_frame_prepare($doc, $tool) {

  /* Heading tags will be reassigned based on their class number + 1 ('.Head1' becomes an <H2>, etc.). This
   * leaves chapter titles as <H1> and moves everything else down one, regardless of the starting point. This accounts for Framemaker's dynamic
   * assignment of heading levels, which can vary from document to document.
   */

  switch ($tool) {
    case 'frame':
      $qp = htmlqp(longform_html($doc));
      $qp->find('p.TitleChapter, p.Head1, p.Heading1'); // Restore this for Frame-native docs 
      break;
    case 'mif2go':
      if (strpos($doc, 'p class="Title"') === FALSE) {
        $regex = "/(<p class=\"Head1.*?\">)(.*?)(<\/p>)/siu";
        $replace = "<h1 class\"TitleChapter\">$2</h1>";
        $doc = preg_replace($regex, $replace, $doc, 1, $count);
      }
  }

  //move headings based on their class. The existing heading tag will be discarded and replaced as Level+1 (ie, '<H3 class="head1"> becomes "<H2>")
  for ($i = 1; $i <= 3; $i++) {
    $regex = "/(<p class=\"Head$i.*?\">)(.*?)(<\/p>)/siu";
    $replace = "<H$i>$2</H$i>";
    $doc = preg_replace($regex, $replace, $doc, -1, $count);

  }

  //deal with sneaky off-label headings
  $regex = "/(<p class=\"TitleIntro\">)(.*?)(<\/p>)/si";
  $replace = "<h1 class=\"document\">$2</h1>";
  $doc = preg_replace($regex, $replace, $doc, -1, $count);

  return extract_body($doc);
}

/**
 * Helper function that redistributes document headings if <H6> tags are
 * present. H6 tags are part/volume headings, so are really H0 -- a level above
 * H1. Reassigns tags based on entries in the class conversion table.
 *
 */
function _doc_import_reorder_headings(&$content, $tool) {

  if ($tool != 'msword') {
    return;
  }

  // If any <H6> tags are in the document, shuffle all headings down a level and reset H6 as H1.
  $reorder = stripos($content, '<h6>');

  if ($reorder !== FALSE) {
    //H6 headings are present, which means the document is in parts.
    $conversion_table = _doc_import_class_conversion_table();
    $tags = $conversion_table['tags']['hpart'];
    foreach (array_reverse($tags) as $new => $old) {
      $replace = $old['tags'];
      $content = str_ireplace(array(
          "<$replace>",
          "</$replace>"
        ), array("<$new>", "</$new>"), $content);
    }
  }
}

/**
 * Helper function that strips chapter/part references from titles.
 * Returns a cleaned string.
 */
function _doc_import_strip_chapters($title) {
  if (strpos($title, ':') !== FALSE) {
    return trim(array_pop(explode(':', $title)));
  }

  return $title;
}
