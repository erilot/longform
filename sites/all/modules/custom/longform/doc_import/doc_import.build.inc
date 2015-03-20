<?php

/**
 * File: doc_import.build.inc
 * Functions that assemble topics into document arrays, build TOC arrays, etc.
 */

/**
 * Given a ticket node and a heading split level, returns an array of document
 * sections.
 *
 * @param: $node = A controller node object
 * @param: $split_level = an integer from 1-3. Sets the heading split level
 *   used to build the document array.
 * @param: $options = array()
 *   Available options are: 'toc' => TRUE, returns a smaller version without
 *   content (titles and level information only)
 *
 */
function doc_import_construct_document_array($ticket, $split_level = 1, $options = NULL) {
  module_load_include('inc', 'doc_import', 'doc_import');
  $options['qp'] = array(
    'convert_from_encoding' => 'utf-8',
    'convert_to_encoding' => 'utf-8',
  );

  $ticket = longform_entities_get_entity();
  $topics = longform_relation_get_other_endpoint_ticket_topic($ticket, TRUE);

  foreach ($topics as $topic) {
    $check = _doc_import_get_topic_chunk($topic);
    if (!empty($check['content'])) {
      $content[] = $check;
    }
    else {
      drupal_set_message('Content has not been converted yet; no preview will be generated.', 'warning');
      return FALSE;
    }
  }
  if (!$content) {
    drupal_set_message('All topics are empty -- something went wrong, probably during document split.', 'error');
    return FALSE;
  }

  $temp = array();
  $i = -1;
  foreach ($content as $section) {
    $level = $section['level'];
    $title = iconv(mb_detect_encoding($section['title']), 'UTF-8', $section['title']);

    $title = str_replace(array("\r\n", "\r", "\n"), ' ', $title);
    $content = trim(iconv(mb_detect_encoding($section['content']), 'UTF-8', $section['content']));
    $targets = $section['anchors']['head'];
    if ($level <= $split_level) {
      //Equal to (or less than) split level. Start a new section.
      $i++;
      $temp_content = "";

      if ($options['toc']) {
        $temp[$i]['title'] = $section['title'];
        $temp[$i]['level'] = $section['level'];
        $temp[$i]['topic'] = $section['topic'];
      }
      else {
        $temp[$i] = array(
          'title' => $title,
          'level' => $level,
          'anchors' => $anchors
        );
        $temp[$i]['content'] = trim(unprotect_special_characters(extract_body(htmlqp(($content), 'div', $options['qp'])
              //->prepend($title_html)
                ->attr('data-topic', $section['topic_nid'])
              ->top()
              ->find('p.placeholder')
              ->remove()
              ->top()
              ->html()
          ))) . PHP_EOL;
        $temp[$i]['topics'] = array($section['topic_nid']);
      }
    }
    else {
      //Higher than split level. Fold into current section.
      if ($options['toc']) {
        continue;
      }

      $target = "";
      if ($section['anchors']['head']) {
        foreach ($section['anchors']['head'] as $link) {
          $target .= mb_convert_encoding("<a name=\"$link\"></a>", 'UTF-8', 'auto');
        }
      }

      $prefix = "<h$level class=\"document subtitle\" data-level=\"$level\">";
      $suffix = "</h$level>";
      $middle = htmlspecialchars($title);
      $title_html = trim($prefix . $middle . $suffix);
      // Character encoding issues at this point can corrupt the HTML output. Force everything to UTF-8.
      $encoding = mb_detect_encoding($title_html);
      $converted = iconv($encoding, 'UTF-8', $title_html);

      // Determine anchor links to prepend to title. Use Querypath to insert them.
      if (!empty($target)) {
        $target_html = $target;
      }
      else {
        $target_html = "";
      }

      if (strpos($section['content'], '<div') !== 0) {
        $section['content'] = substr($section['content'], strpos($section['content'], '<div'));
      }
      $temp_content = trim(unprotect_special_characters(extract_body(htmlqp($section['content'], 'div', $options['qp'])
          ->attr('data-topic', $section['topic_nid'])
          ->prepend($target_html . $title_html)
          ->top()
          ->find("p.placeholder")
          ->remove()
          ->top()
          ->html()
      )));
      $temp[$i]['content'] .= trim($temp_content);
      $temp[$i]['topics'][] = $section['topic_nid'];

    }
    // Make sure all the QP cruft has been removed (doctype declarations, body/html bits)
    $temp[$i]['content'] = preg_replace('/(?:<!doctype.*?>)|(?:<\/?body.*?>)|(?:<\/?html.*?>)/iu', '', $temp[$i]['content'], -1, $count);
  }

  if (!$options['toc']) {
    foreach ($temp as &$x) {
      $x['content'] = '<div class="wrapper" data-heading="' . $x['level'] . '" class="this-level-' . $x['level'] . '">' . $x['content'] . "</div>";
    }
  }

  dpm($temp,'temp');
  return $temp;
}

/**
 * Helper function returns a compact array of topic information for use in
 * building documents and previews.
 * $topic: a topic node object.
 *
 */
function _doc_import_get_topic_chunk($topic) {

  //build link array
  $topic = entity_metadata_wrapper('node', $topic);
  $links = json_decode($topic->field_topic_anchors->value(), TRUE);
  if (!$links) {
    $links = json_decode($topic->field_topic_section_link_targets->value(), TRUE);
  }
  $chunk = array(
    'topic_nid' => $topic->getIdentifier(),
    'level' => $topic->field_topic_source_heading->value(),
    'title' => $topic->title->value(),
    'anchors' => $links,
    'topic' => $topic->getIdentifier(),
    'content' => $topic->field_topic_processed_html->value->value(),
  );
  if (!$chunk['content']) {
    $chunk['content'] = $topic->field_topic_source_content->value->value();
  }
  return $chunk;
}


/**
 * Retrieve preprocessed OR postprocessed JSON file.
 * Takes a node object and a "pre" or "post" string switch.
 * Returns the decoded variant requested.
 */
function _doc_import_get_processed_file($node, $type) {
  if (!is_object($node)) {
    $node_id = $node;
    unset($node);
    $node = node_load($node_id);
  }
  else {
    $node_id = $node->nid;
  }
  //$node = node_load($node->nid);

  switch ($type) {
    case "pre":
      watchdog('import cron', "Retrieving preprocessed file for node [" . $node->nid . "]");
      if (!$node->field_preprocessed_html[LANGUAGE_NONE][0]['uri']) {
        dpm('nothing here');
      }
      $master_file = $node->field_preprocessed_html[LANGUAGE_NONE][0]['uri'];
      if (!$master_file) {
        watchdog('import cron', 'no preprocessed html was available');
      }
      break;
    case "post":
      $master_file = $node->field_processed_json[LANGUAGE_NONE][0]['uri'];
      break;
  }
  watchdog('import cron', "Retrieving [$master_file]");

  if (file_exists($master_file)) {
    $content = json_decode(file_get_contents($master_file), TRUE);
    watchdog('import cron', "Finished retrieving content. [" . count($content) . "] elements loaded.");

    //dpm($content,'json array read');
    return $content;
  }
  else {
    watchdog('import cron', "Expected to find json file at [$master_file], but nothing was there.");

    return FALSE;
  }
}

/**
 * Given a ticket node, returns an array keyed by node ID, with links, titles,
 * and levels.
 *
 */
function doc_import_get_page_index($ticket) {

  module_load_include('inc', 'doc_manage', 'doc_manage.relation');

  //pick up the entities referenced by this controller, then split off the first two elements (controller & cover page).
  $endpoints = longform_relation_get_other_endpoint_ticket_page($ticket, TRUE);

  $tocarray = array();
  foreach ($endpoints as $page) {
    $wrap = entity_metadata_wrapper('node', $page);

    $tocarray[$page->nid] = array(
      'title' => $page->title,
      'link' => l($page->title, 'node/' . $wrap->getIdentifier(), array('html' => TRUE)),
      'level' => $page->book['depth'] - 1,
    );
  }
  return $tocarray;
  //}

}

/**
 * Given a document array, constructs a single HTML string from all the
 * components
 */
function doc_import_construct_html_file($content_array) {

  module_load_include('inc', 'doc_import', 'doc_import');

  foreach ($content_array as $section) {
    $level = $section['level'];
    $title = trim($section['title']);

    if ($section['target']['head']) {
      $target = '<a name="' . $section['target']['head'] . '"></a>';
    }
    $final .= extract_body(htmlqp(longform_html($section['content']), 'div')
          ->prepend("$target<h$level>$title</h$level>")
          ->top()
          ->html()) . PHP_EOL;
  }

  return $final;
}

/**
 * Wrap an HTML string in HTML headers. Use this before passing a string to
 * QueryPath to preserve UTF-8 character sets.
 */
function longform_html($string) {
  if (gettype($string) != 'string') {
    return;
  }
  $prefix = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
    <html lang="en"><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" /></head><body>';
  $suffix = '</body></html>';

  return $prefix . $string . $suffix;
}
