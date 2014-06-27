<?php

/**
 * File: doc_import.postprocess.inc
 * Postprocess functions run on media after creation
 */

 
/**
 * Runs final operations on import:
 *  - Reconstructs Xrefs
 *  - Adds navigation bits
 *  
 */
function doc_import_postprocess(){
    module_load_include('inc', 'doc_import', 'doc_import.navigation');
    module_load_include('inc', 'doc_import', 'doc_import.links');
    
    //pick up the entities referenced by this controller, then drop the first element (which is the controller).
    $endpoints = doc_manage_relation_query('is_a_record_in',$node);
    $parent_tmp = array_shift($endpoints);
    $cover_tmp = array_shift($endpoints);
  
    $parent = array_shift(entity_load($parent_tmp['entity_type'], array($parent_tmp['entity_id']), TRUE));
    $cover = array_shift(entity_load($cover_tmp['entity_type'], array($cover_tmp['entity_id']), TRUE));
    
    //set parent workflow state to "reviewing"
    $target_state = 31; //31 = "Reviewing" state
    $comment = t("State changed by import module (postprocess)");
    $new_state = workflow_execute_transition($parent, $target_state, $comment, TRUE);
    
    //drupal_set_message('Document state set to ' . $new_state);
    $state_object = workflow_get_workflow_states_by_sid(workflow_node_current_state($parent));
    $state_name = $state_object->state;
    drupal_set_message('Document state set to ' . $state_name, 'status');

    entity_get_controller('node')->resetCache(array($parent->nid));

    //set up batch
    $operations[] = array('doc_import_rebuild_page_links', array($endpoints, $parent, $cover));
    $operations[] = array('_doc_import_postprocess_batch_add_links_operation', array($endpoints, $parent, $cover));
    $batch = array(
      'operations' => $operations,
      'file' => drupal_get_path('module', 'doc_import') . '/doc_import.postprocess.inc',
      'finished' => '_doc_import_postprocess_batch_add_links_finished',
      'title' => t('Processing pages...'),
      'init_message' => t('Initializing...'),
      'progress_message' => t('Working on @current out of @total.'),
      'error_message' => t('Error while processing pages..'),
    );
    
    batch_set($batch);
    batch_process();
    
}

function _doc_import_postprocess_batch_add_links_operation($endpoints, $controller, $cover, &$context) {
    
    module_load_include('inc','doc_import');
    module_load_include('inc', 'doc_manage', 'doc_manage.relation');
    module_load_include('inc', 'doc_import', 'doc_import.links');
    module_load_include('inc', 'doc_import', 'doc_import.navigation');

  // information needed to track progression between successive calls.
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_node'] = 0;
    $context['results']['controller'] = $controller;
    $context['results']['cover'] = $cover;

    // Save topic count for the termination message.
    $context['sandbox']['max'] = count($endpoints);
  }

  // Retrieve the next group of sections
  $result = array_slice($endpoints,$context['sandbox']['current_node'],1);

    foreach ($result as $node) {
    
	$page = node_load($node['entity_id']);
	$body = $page->body[LANGUAGE_NONE][0]['value'];
	
	$page_uses_topics = doc_manage_relation_query('page_uses_topic', $page);
	$section = htmlqp($body);
	$level = $section->find('div.wrapper')->attr('heading');
	//if heading attribute wasn't found, this is an older document. Get it the hard way.
	if (!$level){
	    if ($section->top()->find('.this-level-1')->size() != 0){
		$level = 1;
		$section->attr('heading','1')->addClass('wrapper');
	    }
	    if ($section->top()->find('.this-level-2')->size() != 0){
		$level = 2;
		$section->attr('heading','2')->addClass('wrapper');
	    }
	    if ($section->top()->find('.this-level-3')->size() != 0){
		$level = 3;
		$section->attr('heading','3')->addClass('wrapper');
	    }
	}
	  
	//Add navigation elements
	if($level != ''){
	    $section = doc_import_add_page_links($section->top(), $level);
	}
	node_save($page);

	$page->body[LANGUAGE_NONE][0]['value'] = $section;
	try{
	    node_save($page);
	}
	catch (Exception $e) {
	    dpm($e,'exception!');
	    dpm((array)$section,'section');
	}
	
	$links = _doc_import_scrape_page_links($section, $page->nid);
	if ($links) $context['results']['sub_toc'][$page->nid] = $links;
	
	// Store some results for post-processing in the 'finished' callback.
	// The contents of 'results' will be available as $results in the
	// 'finished' function (in this example, batch_example_finished()).
	$context['results']['endpoints'][$page->nid] = $page->nid;
    
	// Update our progress information.
	$context['sandbox']['progress']++;
	$context['sandbox']['current_node']++;
	$context['message'] = 'Processing node ' . $context['sandbox']['current_node'] . ' of ' . $context['sandbox']['max'] . ': ' . $page->title;
    }

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Convert topic-scope xrefs to page-scope. Successively loads, modifies, and saves pages tracked by the controller.
 * @TODO: convert this to a batch operation.
 * @param $node (object) The controller (ticket_node) node object.
 */
function _doc_import_postprocess_rebuild_page_xrefs($node) {
    
    // Reconstruct page links
    $endpoints = relation_load($node->field_document_relation[LANGUAGE_NONE][0]['value']);
    $endpoints = $endpoints->endpoints[LANGUAGE_NONE];
    $parent = array_shift($endpoints);

    foreach($endpoints as $endpoint) {
	$pages_in_book[] = $endpoint['entity_id'];
    }
    // Drop cover page
    array_shift($pages_in_book);
    
    doc_import_rebuild_page_links($pages_in_book);
    
}

function _doc_import_postprocess_batch_add_links_finished($success, $results, $operations) {
    if ($success) {

	$message = count($results['endpoints']) .' pages were processed.';
	drupal_set_message($message);
	
	$parent = $results['controller'];
	$sub_toc = $results['sub_toc'];

	//create TOC array and attach it to parent node
	doc_import_create_toc($parent, $sub_toc);
	
	//set cover page to approved
	$sid = 14; // = "Approved" (book node)
	$cover = $results['cover'];
	workflow_execute_transition($cover, $sid, t('Book cover page set to approved during postprocessing'), TRUE);
	entity_get_controller('node')->resetCache(array($cover->nid));
    
	
	//add lock file to resources, indicating this process has been completed.
	$timestamp = time();
	global $user;
	$file = "public://import-source-files/".$parent->nid."/postprocessed.lck";
	file_unmanaged_save_data('Converted by ' . $user->name . ' at ' . $timestamp, $file, FILE_EXISTS_REPLACE);
	
	//$new_state = workflow_execute_transition($parent, 31, 'State set by postprocess function', TRUE);
	//entity_get_controller('node')->resetCache(array($parent->nid));

	drupal_set_message('Done!');
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
 * Creates a nested TOC list array, formatted for themeing, and encoded as a JSON object.
 */
function doc_import_create_toc($parent, $sub_toc = NULL){
    
    if ($parent->type == 'book') {
	$toc_array = doc_import_recursive_book_array_builder($parent->nid);
	return false;
    }
    
    module_load_include('inc', 'doc_import', 'doc_import.build');

    // If $sub_toc is not supplied, generate it.
    if(!$sub_toc){
	//pick up the entities referenced by this controller, then split off the first two elements (controller & cover page).
	module_load_include('inc', 'doc_manage', 'doc_manage.relation');
	$endpoints = doc_manage_relation_query('is_a_record_in',$parent);
	$parent_tmp = array_shift($endpoints);
	$cover_tmp = array_shift($endpoints);
	
	// Loop through the rest of the pages and get their sub toc's.
	foreach($endpoints as $endpoint) {
	    $node = node_load($endpoint['entity_id']);
	    $links = _doc_import_scrape_page_links($node);
	    if($links) $sub_toc[$node->nid] = $links;
	}
    }
    // Process book TOC, folding in $sub_toc entries where appropriate
    $index = doc_import_get_page_index($parent);
    $l1 = 0;
    $l2 = 0;
    $l3 = 0;
    $previous = 1;
    foreach($index as $nid => $entity) {
	$thislevel = $entity['level'];
	unset ($entity['level']);
	switch ($thislevel) {
	    case 1:
		if ($previous >= $thislevel) {
		    $l1++;
		    $l2 == 0;
		    $l3 == 0;
		}
		$toc[$l1] = array(
		    'data' => $entity['link'],
		    'class' => array('l1'),
		    );
		if ($sub_toc[$nid]) {
		    $toc[$l1]['class'] = array('l1 with-children');
		    $toc[$l1]['children'] = $sub_toc[$nid];
		}
    
		break;
	    case 2:
		if ($previous >= $thislevel) {
		    $l2++;
		    $l3 == 0;
		}
		$toc[$l1]['children'][$l2] = array(
		    'data' => $entity['link'],
		    'class' => array('l2'),
		    );
		if ($sub_toc[$nid]) {
		    $toc[$l1]['children'][$l2]['class'] = array('l2','with-children');
		    $toc[$l1]['children'][$l2]['children'] = $sub_toc[$nid];
		}
    
		break;
	    case 3:
		$l3++;
		$toc[$l1]['children'][$l2]['children'][$l3] = array(
		    'data' => $entity['link'],
		    'class' => array('l3'),
		    );
		break;
	}
	$previous = $thislevel;
    }
    $contents = array(
      'items' => $toc,
      'title' => NULL,//t('Table of Contents'),
      'type' => 'ul',
      'attributes' => array('class' => array('toc-list', 'toc')),
      );
    $parent->field_toc_array[LANGUAGE_NONE][0]['value'] = drupal_json_encode($contents);
    node_save($parent);
    
}

/**
 * Helper function to scrape a generated page for inside links. These must already be present on the page.
 */
function _doc_import_scrape_page_links($section, $nid = NULL) {
    module_load_include('inc', 'doc_import');
    
    if ((!$nid && (!gettype($section) == 'object' && $section->type != 'book'))) {
	drupal_set_message('_doc_import_scrape_links needs either a NID or a node object passed as $section to work, and something is missing.','error');
	return FALSE;
    }
    
    if(!$nid) $nid = $section->nid;
    if(!$nid) {
	drupal_set_message('Unable to get NID for postprocessing. Dump of $section follows.');
	dpm($section,'section','error');
    }
    // This function can be called with a string or a node object. If the latter, get the body string.
    if(gettype($section) == 'object' && $section->type == 'book') {
	$section = $section->body[LANGUAGE_NONE][0]['value'];
    }

    try{
        $links = htmlqp(protect_special_characters($section), '#page-links li a');
    }
    catch (Exception $e){
	// An exception here means no content matched, so there are no page links to scrape.
	return;
    }
    
    $list = array();
    foreach($links as $link) {
	$og = qp($link);
	preg_match('/<a.*?href="(.*?)".*?>(.*?)<\/a>/iu', $link, $out);
	$text = unprotect_special_characters($og->text());
	$href = unprotect_special_characters($og->attr('href'));
	$url = 'node/' . $nid;
	$inside_link = array(
	    'data' => l($text,$url,array('fragment' => str_replace('#','',$href), 'html' => TRUE)),
	    'class' => array('inside-link'),
	);
	$list[] = $inside_link;
    }
    if ($list) return $list;
}

/**
 * Iterates through all topics in the controller/uses_topic relation and reconstructs links.
 * Returns TRUE if no errors were found, and a descriptive error array if any links could not be reconnected.
 * 
 */

 function doc_import_rebuild_page_links($pages, $controller, $cover, &$context){
    module_load_include('inc', 'doc_manage', 'doc_manage.topics');
    module_load_include('inc', 'doc_import');
    
    if (empty($context['sandbox'])) {
	$context['sandbox'] = array();
	$context['sandbox']['progress'] = 0;
	$context['sandbox']['current_topic'] = 0;

	// Save topic count for the termination message.
	$context['sandbox']['max'] = 1;
    }
    
    // First, get the array of head & body links from topic nodes
    $pages_objects = node_load_multiple($pages);
    $heads_combined = array();
    foreach ($pages_objects as $page) {
	$anchors = _doc_manage_get_topic_fields ($page, "field_topic_anchors");
	$topic_index[$page->nid] = _doc_manage_get_topic_nids($page, 'page_uses_topic');
	$subhead = array();
	foreach ($anchors as $key => $anchor) {
	    $anchors_split['head'][$page->nid][$key] = $anchors[$key]['head'];
	    $anchors_split['body'][$page->nid][$key] = $anchor[$key]['body'];
	}
	
    }
    // Strip empty values
    //$anchors_split['head'] = array_filter($anchors_split['head']);
    //$anchors_split['body'] = array_filter($anchors_split['body']);

    // Cycle through page content, replacing topic hrefs with page hrefs
    global $base_path;
    foreach ($pages_objects as $page) {
	$content = $page->body[LANGUAGE_NONE][0]['value'];
	$content = extract_body(htmlqp($content, 'a.matched.other-section.topic-link-processed')
    	->each(function($index, $item) use ($topic_index, $anchors_split, $base_path) {
		$link = qp($item);
		$ref = $link->attr('ref');
		$topic_nid = $link->attr('topic');
		$href = $link->attr('href');
		
		$topic = $link->attr('topic');
		//@TODO: Create fallback if topic attribute is not available
		
		$page_id = recursive_array_search($topic, $topic_index);
		$index = array_search($topic, array_keys($anchors_split['head'][$page_id]));
		//dpm($anchors_split['head'][$page_id],'heads subsection');
		if ($index === FALSE) {
		    return $link->addClass('link-error page-link-error')->removeClass('match');
		}
		
		if ($index === 0) {
		    $new_href = $base_path . "node/$page_id";
		    		    
		}
		else {
		    $new_href = $base_path . "node/$page_id#$ref";
		}
		$link->attr('href', $new_href)
			->attr('topic', $topic)
			->removeClass('link-topic')->removeClass('topic-link-processed')
			->addClass('page-link page-link-processed');
		
	})->top()->html());
	
	$page->body[LANGUAGE_NONE][0]['value'] = substr($content, strpos($content, '<div class="wrapper"'));
	//dpm((array)$page->body[LANGUAGE_NONE][0]['value'],'updated page');
	 node_save($page);
    }

    drupal_set_message('Cross-reference links rebuilt.','status');
    
    $context['results']['endpoints'] = $relation_endpoints;
  
    // Update our progress information.
    $context['sandbox']['progress']++;
    $context['sandbox']['current_topic']++;
    $context['message'] = 'Processing links...';
  
    // Inform the batch engine that we are not finished,
    // and provide an estimation of the completion level we reached.
    if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
      $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
    }

    $sandbox['results']['link-errors'] = $errors;
 }
