<?php

/**
 * File: doc_import.split.inc
 * These functions are responsible for parsing, splitting, and storing documents into topics.
 */

 
 
/*
 * Converts the raw_html_[NID] file to an array of sections split at h1/h2/h3. Runs preprocess functions on the file (merging FM and Word differences)
 * and saves array as json_encoded data.
 * Returns jscon_encoded data or FALSE.
 */
function doc_import_split(){
    module_load_include('inc', 'doc_import');
    module_load_include('inc', 'doc_import', 'doc_import.log');
    module_load_include('inc', 'doc_manage', 'doc_manage.relation');
    module_load_include('inc', 'doc_manage', 'doc_manage.workflow');
    module_load_include('inc', 'doc_import', 'htmLawed.lib');
  
    $node = menu_get_object();
    $start = time();
    add_log_entry('info',"Starting content splitting for node $node_id",$log);
  
    $meta['final_dir'] = "public://import-source-files/".$node->nid."/";
    $meta['author_tool'] = $node->field_authoring_tool[LANGUAGE_NONE][0]['value'];
    if(!isset($node->field_raw_html_master)){
      add_log_entry('error',"Node " . $node->nid . " does not have a raw_html file to work with!");
      drupal_set_message('There is no raw HTML file to work with. Nothing was done.','error');
    }
    $author_tool = $meta['author_tool'];
    $content = file_get_contents($node->field_raw_html_master[LANGUAGE_NONE][0]['uri']);
    if(strlen(trim($content)) == 0) {
	drupal_set_message('The raw HTML file is empty. There was likely an error in the unpacking process.
			  Try unpacking the archive again; if that fails, contact Eric Lotze for help.','error');
	return FALSE;
    }
    
    add_log_entry('info',"Read file contents from " . $node->field_raw_html_master[LANGUAGE_NONE][0]['uri'], $log);
    
    // Adjust headings
    _doc_import_reorder_headings($content, $author_tool);

    // Document preprocessing
    add_log_entry('info',"Beginning [$author_tool] preprocessing", $log);
    
    if($author_tool == 'msword'){ //word
	$content = doc_import_word_preprocess(extract_body($content));
    }
    
    if ($author_tool == 'frame' || $author_tool == 'mif2go'){ //frame or mif2go
	$content = doc_import_frame_prepare($content, $author_tool);
    }
    
    add_log_entry('info',"Completed [$author_tool] preprocessing",$log);
    extract_body($content);
    // Strip existing <div> elements
  
    if(strpos($content, '</div') !== FALSE){
	$content = preg_replace('/<\/*?div.*?>/i','',$content);
    }
    // if there are no heading 1s in the content, bump everything up a level
    if(stripos($content,'</h1>') === FALSE){
	//return false;
	drupal_set_message('No head1 tags are in this document. Shifting tags up one level (h2=>h1...).','warning');
	foreach(array(2,3,4,5,6) as $heading){
	    $head_tag['old'] = 'h' . $heading;
	    $head_tag['new'] = 'h' . ($heading - 1);
	    $content = str_ireplace('<' . $head_tag['old'], '<' . $head_tag['new'], $content);
	    $content = str_ireplace('</' . $head_tag['old'] . '>', '</' . $head_tag['new'] . '>', $content);
	}

	// if, after that, there are still no H1 elements, abandon ship.
	if(stripos($content,'</h1>') === FALSE){
	    drupal_set_message('Unable to safely promote headings to reach a Head1. Check your source file to make sure the highest heading level is either Heading 1 or Heading 2.','error');
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
			   This title may be changed prior to publication.','warning');
    }
    
    // To avoid causing querypath problems with sections that contain no content, inject an temporary placeholder after each heading level.
    $insert = '<p class="placeholder">===REMOVE THIS PLACEHOLDER===</p>';
    $content = str_ireplace(array('</h1>','</h2>','</h3>'), array ('</h1>' . $insert,'</h2>' . $insert,'</h3>' . $insert), $content);
  
    add_log_entry('info',"Converting content to heading-based array",$log);
    
    // Split document into topics
    doc_import_split_headings($content, $content_sectioned, 1, 3, $author_tool);
    // Pull together an array to summarize progress. This is for debugging purposes only at present.
    foreach($content_sectioned as $bit) {
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
    if(!empty($content_sectioned)){
	add_log_entry('info',"Finished creating content array. [" . count($content_sectioned) . "] sections were found.",$log);
    }
    else{
	add_log_entry('error',"Content was not converted. Error in _doc_import_split_headings.",$log);
	doc_import_update_log($log, $node_id);
	drupal_set_message('An error occurred in _doc_import_split_headings.','error');
	return FALSE;
    }

    // Drop pre-existing relations
    $query = relation_query('node', $node->nid, 'is_a_topic_in_');
    $result = $query->execute();
    
    foreach ($result as $kill) {
	relation_delete($kill->rid);
	drupal_set_message('Deleted outdated relation #' . $kill->rid . ' .', 'status');
    }
    
    // Set up batch  
    $operations[] = array('_doc_import_save_topics_operation', array($content_sectioned, $node));
    $batch = array(
	'operations' => $operations,
	'file' => drupal_get_path('module', 'doc_import') . '/doc_import.split.inc',
	'finished' => '_doc_import_save_topics_finished',
	'title' => t('Creating topics...'),
	'init_message' => t('Initializing...'),
	'progress_message' => t('Created @current out of @total.'),
	'error_message' => t('Error while saving topics.'),
    );
    
    batch_set($batch);
    batch_process();
}

/**
 * Batch operation to save topic nodes
 */
function _doc_import_save_topics_operation($content_array, $controller, &$context){
    if(!$controller) {
	watchdog('IMPORT', 'No controller passed to doc_import_save_topics_operation', 'error');
    }
  // information needed to track progression between successive calls.
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_topic'] = 0;

    // Save topic count for the termination message.
    $context['sandbox']['max'] = count($content_array);
  }
//dpm($context,' context array');
  // Retrieve the next group of sections
  $result = array_slice($content_array,$context['sandbox']['current_topic'],1); //last parameter is $limit. Caps number of jobs that can be run at once.

  foreach ($result as $section) {
    $topic = new stdClass();
    $topic->type = 'topic';
    node_object_prepare($topic);
    
    //add topic-specific fields
    if(!$section['title']) drupal_set_message('a topic with an empty title (a heading tag with no content) was found while saving topics. That topic has been discarded.', 'warning');
    $topic->title = html_entity_decode($section['title'], ENT_QUOTES | 'ENT_HTML401', 'UTF-8');
    $topic->field_topic_raw_content[LANGUAGE_NONE][0]['value'] = $section['content'];
    $topic->field_topic_raw_content[LANGUAGE_NONE][0]['format'] = 'full_html';
    $topic->field_topic_initial_weight[LANGUAGE_NONE][0]['value'] = $section['id'];
    $topic->field_topic_initial_heading[LANGUAGE_NONE][0]['value'] = $section['level'];
    
    //dpm($section['anchors'],'targets to save');
    if(!empty($section['anchors'])){
      $targets = json_encode($section['anchors']);

      $topic->field_topic_anchors[LANGUAGE_NONE][0]['value'] = $targets;      
    }
    
    //add fields from controller: get fields that exist both in the import_ticket and topic bundles.
    $target_fields = array_filter(
	field_info_field_map(),
	function($array){
	    if(in_array('import_ticket', $array['bundles']['node']) && in_array('topic', $array['bundles']['node'])) {
		return TRUE;
	    }
	    else {
		return FALSE;
	    }    
	});
    
    foreach($target_fields as $name=>$field){
      $topic->$name = $controller->$name;
    }
    //dpm($topic,'topic to save');
    //all fields have been added, so save the new node. Add the resulting nid to the relation array builder.
    node_save($topic);
    
    //if this is the last pass, prepend the $controller to the relation array and create the relation. Add it to the $controller and save.
    $context['sandbox']['relation_endpoints'][] = array('entity_type' => 'node', 'entity_id' => $topic->nid);
    if($context['sandbox']['current_topic'] == $context['sandbox']['max'] -1){
	$context['results']['endpoints'] = $context['sandbox']['relation_endpoints'];
    }
    
    // Store some results for post-processing in the 'finished' callback.
    // The contents of 'results' will be available as $results in the
    // 'finished' function (in this example, batch_example_finished()).
    $context['results']['ticket'] = $controller->nid;
    
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
    $endpoints = array_merge(
	array(
	    array(
		'entity_type' => 'node',
		'entity_id' => $ticket,
		),
	    ),
	$endpoints);
      //now create the new relation, save it, and add it to $controller
      $new_relation = relation_create('uses_topic',$endpoints);
      $new_relation_id = relation_save($new_relation);
      //load the $controller object, attach the relation, and save.
      $temp = node_load($ticket);
      
      $temp->field_topic_used_in_[LANGUAGE_NONE][0] = $new_relation;
      node_save($temp);

    // Here we do something meaningful with the results.
    $message = count($results) .' processed.';
    $message .= theme('item_list', $results);  // D6 syntax
    drupal_set_message($message);
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array('%error_operation' => $error_operation[0], '@arguments' => print_r($error_operation[1], TRUE)));
    drupal_set_message($message, 'error');
  }
  
}

/**
 * Returns a comma-delimited string of the possible heading split points, e.g. "H1, H2, H3, H4"
 *
 */
function get_split_points($highest_split_level){
		
    // Define break points
    $header_list = array();
    
    // Construct string of legal break headers. Will be all from H1 through h(current).
    for($i=1; $i <= $highest_split_level; $i++){
		    $header_list[] = 'h' . $i;
    }
    $break_at_headers = implode(', ', $header_list);
    return $header_list;

    //delete next line when above return value tests ok
    //return $break_at_headers;
}


/**
 * Collects the information gathered from the $haystack QP object into an array and
 * pushes it onto the end of the $content_array array.
 * 
 */ 
function push_section($title, $links, $contents, $heading_level, &$content_array = array()){

    //skip sections with no title (empty title tags). These are authoring errors.
    $title_trim = htmlentities(trim(str_replace("  ", " ", str_replace(array("\r", "\n", "\r\n", "\n\r"), " ", $title))));
    if($title_trim == '&Acirc;&nbsp;' || $title_trim == "&amp;nbsp;") {
	drupal_set_message('Skipping section with no text in heading tag.','warning');
	return;
    }
    
    //clear out added content shims
      
    $contents = extract_body(htmlqp($contents)->top()->html());
    unprotect_special_characters($contents);
    $section['level'] = $heading_level;
    $section['title'] = $title;
    $section['anchors'] = $links;
    
    $last = end($content_array);
    if(!empty($last)){
	$section['id'] = $last['id'] + 1;
	}
    else {
	$section['id'] = 0;
    }
    $section['content'] = preg_replace(
	array('/<p class="placeholder"><\/p>/',   '/<div id=".*?"/'),
	array('', '<div id="section-'.$section['id'] . '"'),
	$contents); //removes placeholder <p> tags
    
      $content_array[] = $section;
}


/**
 * Given a QueryPath object, attempts to separate an enclosed link from the enclosed text. Returns a two-element array.
 * For example, "<h3><a name='xyz'>Hi mom</a></h3>" returns (array)('title'=>'Hi mom', 'target'=> array('xyz', '123'))
 */
function _doc_import_separate_heading_links($content){
    if (stripos($content,'</h1>') === FALSE && stripos($content,'</h2>') === FALSE && stripos($content,'</h3>') === FALSE) return FALSE;
    
    if (stripos($content,'</a>') === FALSE) {
	return array('title' => _doc_import_get_heading_text($content), 'target' => NULL);
    }
    else{
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
 * Takes a text string of HTML and returns an array of link names present. Strips "_Tocxxxx" links.
 */
function _doc_import_get_heading_links($content) {
    $regex = "/name=\"(.*?)\"/ui";
    $content = unprotect_special_characters(html_entity_decode($content, ENT_QUOTES | 'ENT_HTML401', 'UTF-8'));
    preg_match_all($regex, $content, $out);
    $out = array_filter($out[1],function($e){
	if(stripos($e,'_toc') === FALSE) return TRUE;});
    return $out;
}

/**
 * Takes a string of content and returns the first topic, as defined by $current_split and $splits.
 */
function _doc_import_parse_topic($content, $current_split, $splits) {
    
    $stop = 0;   
    foreach($splits as $split){
	$tag = '<' . $split;
	$position = stripos($content, $tag);
	
	if($position === FALSE) continue;
	
	if($position > $stop) {
	    $stop = $position;
	    break;
	}
    }
    
    if($stop == 0) {
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
 * Takes a string of content and splits it into components. Returns an array of content.
 */

function _doc_import_parse_section($content, $current_split, $splits) {
    //dpm((array)$content,'content to parse');
    $splits = implode('|',$splits);
    $current_split = 'h'.$current_split;

    $regex = "/<(?:$splits).*?>(?:.*?)<\/(?:$splits)>/isu";

    preg_match_all($regex, $content, $matches);
    
    $positions = array();
    $i = 0;
    
    foreach($matches[0] as $key => $match) {
	$positions[$key]['heading'] = strpos($content, $match);
	$positions[$key]['start'] = $positions[$key]['heading'] + strlen($match);
	if($i == count($matches[0])-1) continue;	
	$positions[$key]['length'] = strpos($content,$matches[0][$i+1]) - $positions[$key]['start'];
	$i++;
    }
    $positions[count($matches[0])-1]['length'] = strlen($content) - $positions[count($matches[0])-1]['start'];

    $return = array();
    
    foreach($matches[0] as $key => $match) {
	$return[$key]['header'] = $match;
	$return[$key]['content'] = substr($content,$positions[$key]['start'],$positions[$key]['length']);
    }
    //return array_combine(array('all','heading','content'),$matches);
    return $return;
}

function doc_import_split_headings($content_chunk, &$content_array = array(), $current_split_level = 1, $highest_split_level = 1, $tool = NULL, &$counter_1=0,&$counter_2=0,&$counter_3=0 ) {
        
    $section_break_tag = 'h' . $current_split_level;

    $sections = _doc_import_parse_section($content_chunk, $current_split_level, (array)('h' . $current_split_level));

  //Loop through each chunk iteratively (all H1s, then H2s inside each H1, etc.)
    foreach($sections as $section) {
    //Increment counters for div ID #s, then construct the ID term to attach to the section DIV
    switch ($current_split_level){
      case 1:
        $counter_1++;
        if($highest_split_level > 1){ //If we will be going deeper than this level, reset the next-higher counter
          $counter_2 = 1;
        }
        break;
      case 2:
        $counter_2++;
        if($highest_split_level > 2){ //If we will be going deeper than this level, reset the next-higher counter
          $counter_3 = 1;          
        }
        break;
      case 3:
        $counter_3++;
        break;
    }
    $section_id = $counter_1.'-'.$counter_2.'-'.$counter_3;
    $head = _doc_import_separate_heading_links($section['header']);
    $section_title = $head['title'];
    $section_links = $head['anchors'];

    //$section_title = $process_section->text();
    html_entity_decode(unprotect_special_characters($section_title));
    
    if($current_split_level < $highest_split_level){
        $split_points = get_split_points($highest_split_level);
    }
    else if ($current_split_level == $highest_split_level){
        $split_points = get_split_points($current_split_level);
    }

    // As of version 1.0 of the polycom template: CSS is now unified. The old switch code is here for reference.    
    //// Add CSS Template flavor
    //switch ($tool){
    //    case 'mif2go':
    //    case 'frame':
    //        $css = "css-set-1";
    //        break;
    //    case 'word':
    //        $css = "css-set-2";
    //        break;
    //    default:
    //        $css = "css-set-0";
    //}   

    $css = "stylesheet-1-0";
    
    //Parse the current section, and add its content to the $content_array stack (content = :root to first break point)
   
   //dpm((array)$section,'section');
    $topic = _doc_import_parse_topic($section['content'], $current_split_level, $split_points);
    
    $prefix = '<div id="section-'.$section_id.'" class="document-content heading-level-'.$current_split_level.' ' . $css . '">' . PHP_EOL;
    $suffix = PHP_EOL . '</div>';
    
    //$process_section->branch()->nextUntil($split_points)->wrapAll('<div id="section-'.$section_id.'" class="document-content heading-level-'.$current_split_level.' ' . $css . '"/>');
    //$section_contents = $process_section->top()->find('#section-'.$section_id)->html();
    $section_level = $current_split_level;
    push_section($section_title, $section_links, ($prefix . $topic['this'] . $suffix), $current_split_level, $content_array);
    //If we are not at the highest split level, increment the split level counter and recurse deeper into the section
    if ($current_split_level < $highest_split_level){
      
        $current_split_level ++;
	
        doc_import_split_headings ($topic['after'], $content_array, $current_split_level, $highest_split_level, $tool, $counter_1,$counter_2,$counter_3);
        //doc_import_split_headings ($process_section->nextUntil($section_break_tag)->andSelf(), $content_array, $current_split_level, $highest_split_level, $tool, $counter_1,$counter_2,$counter_3);
        
        $current_split_level --;
    }
  }				
}

/**
 * Performs initialization functions before splitting content
 */
function doc_import_initialize_import($node){
    
    $data_directory = 'public://import-source-files/'.$node->nid.'/log';
    
    //clear out pre-existing link file
  	file_unmanaged_delete($data_directory.'/links');

}

/**
 *	Correct Word-generated HTML
 */
function doc_import_word_preprocess($content) {

    //flip spaces between consecutive <span> tags backwards so they're included at the end of the previous <span> instead of orphaned in the middle
    $content = str_replace('</span> <span',' </span><span',$content);
    
    //remove hard-coded list styling. Number removal is done via regex later.
    $content = str_replace('<span>&middot;<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span></span>','',$content);
    $content = str_replace('<span>&#9675;<span>&nbsp;</span></span>','',$content);
    
    return $content;
}


/**
 * Prepare Frame HTML for splitting
 */
function doc_import_frame_prepare($doc, $tool) {
  $start = time();
  
/* Heading tags will be reassigned based on their class number + 1 ('.Head1' becomes an <H2>, etc.). This
 * leaves chapter titles as <H1> and moves everything else down one, regardless of the starting point. This accounts for Framemaker's dynamic
 * assignment of heading levels, which can vary from document to document.
 */	
	
  switch ($tool){
    case 'frame':
      $qp = htmlqp(technet_html($doc));
      $qp->find('p.TitleChapter, p.Head1, p.Heading1'); // Restore this for Frame-native docs 
      break;
    case 'mif2go':
      if(strpos($doc,'p class="Title"') === FALSE){
        $regex = "/(<p class=\"Head1.*?\">)(.*?)(<\/p>)/siu";
	$replace = "<h1 class\"TitleChapter\">$2</h1>";
	$doc = preg_replace($regex, $replace, $doc, 1, $count);
      }
  }
  
  //move headings based on their class. The existing heading tag will be discarded and replaced as Level+1 (ie, '<H3 class="head1"> becomes "<H2>")
  for($i = 1; $i<=3; $i++){
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
 * Helper function that redistributes document headings if <H6> tags are present.
 * H6 tags are part/volume headings, so are really H0 -- a level above H1.
 * Reassigns tags based on entries in the class conversion table.
 *
 */
function _doc_import_reorder_headings(&$content, $tool){
    
    if ($tool != 'msword') return;
    
    // If any <H6> tags are in the document, shuffle all headings down a level and reset H6 as H1.
    $reorder = stripos($content,'<h6>');

    if($reorder !== FALSE){
	//H6 headings are present, which means the document is in parts.
        $conversion_table = _doc_import_class_conversion_table();
	$tags = $conversion_table['tags']['hpart'];
	foreach(array_reverse($tags) as $new => $old){
	    $replace = $old['tags'];
	    $content = str_ireplace(array("<$replace>", "</$replace>"), array("<$new>", "</$new>"), $content);
	} 
    }
}

/**
 * Helper function that strips chapter/part references from titles.
 * Returns a cleaned string.
 */
function _doc_import_strip_chapters($title){
    if(strpos($title,':') !== FALSE){
	return trim(array_pop(explode(':',$title)));
    }

    return $title;
}
