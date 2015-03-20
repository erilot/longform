<?php

/**
 * file: doc_import.save.inc
 * Creates books and other media from source topics
 */

 /**
 * Create book and save all book nodes.
 */
function doc_import_save_nodes($ticket, &$content_array){
  $title = $ticket->title;
  $edit['type'] = 'book';    //@TODO make this variable
  $fields = field_info_instances('node',$edit['type']);
 
  global $user; //get global user object
  $edit['uid'] = $user->uid;
  $edit['name'] = $user->name;

  
  $edit['promote'] = FALSE;
  $edit['comment'] = variable_get('comment_' . $edit['type'], 2);
  $edit['format'] = filter_default_format();
  $edit['status'] = 1;

  $edit['language'] = $ticket->language;

  //copy fields
  foreach($fields as $name => $field){
    $edit[$name] = $ticket->$name;
  }
  $title = html_entity_decode($title, ENT_QUOTES, 'utf-8');
  $edit['title'] = $title;
  
  //Top level book nodes have no body content
  $edit['field_book_body'][LANGUAGE_NONE][0] = array('value' => NULL, 'format' => 'full_html');

  $edit['book'] = array('bid' => 'new', 'plid' => 0);

  $cover = (object) node_submit((object) $edit);
  node_save($cover);

  $cover_relation = longform_relation_create('longform_ticket', $ticket->tid, 'node', $cover->nid, 'ticket_page');
  $cover_state = longform_workflow_set_state($cover, 'nid', 'approved', NULL, 'Automatically approved by importer');

  // Transfer imported content to book pages
		$build = array();
		$build[0] = array(
    'nid' => $cover->nid,
    'title' => $cover->title,
    'bid' => $cover->nid,
    'mlid' => $cover->book['mlid'],
    'plid' => $cover->book['plid'],
    'weight' => 0,
    'level' => 0);

  $page_relation = array();

  for ($i = 0; $i < count($content_array); $i++) {
				$book_node = clone $cover;
				$book_node->nid = 0;
				$book_node->vid = 0;
				
				$level = $content_array[$i]['level'];
				$find_heading = 'h' . $level;
				
				// Strip same-level heading from content and set it as the node title instead
				$content = htmlqp($content_array[$i]['content'])->find($find_heading)->remove()->top()->html();
				$book_node->title = html_entity_decode($content_array[$i]['title'], ENT_QUOTES | 'ENT_HTML401', 'UTF-8');

				$book_node->field_book_body[LANGUAGE_NONE][0]['value'] = $content;
				$book_node->field_book_body[LANGUAGE_NONE][0]['format'] = 'full_html';
				$book_node->path['pathauto'] = TRUE;
				$book_node->teaser = text_summary($content);
				$book_node->parent = $cover->nid;
				$book_node->book['bid'] = $cover->nid;
				$book_node->book['menu_name'] = $cover->book['menu_name'];

				if ($level < $build[$i]['level']) { // at lower level
						$weight = 0;
						$book_node->book['weight'] = 0;
						$book_node->book['plid'] = $build[$i]['mlid'];
				}
				elseif ($level == $build[$i]['level']) { // at the same level
						$weight++;
						$book_node->book['weight'] = $weight;
						$book_node->book['plid'] = $build[$i]['plid'];
				}

				node_save($book_node);
    $cover_state = longform_workflow_set_state($book_node, 'nid', 'review', NULL, 'Automatically set by importer');

    $build[] = array('nid' => $book_node->nid, 'title' => $book_node->title, 'bid' => $cover->nid, 'mlid' => $book_node->book['mlid'], 'plid' => $book_node->book['plid'], 'weight' => $weight, 'level' => $level);

				$nid = $book_node->nid;
				$saved_nids[] = $nid;
		
    $page_relation = longform_relation_create('longform_ticket', $ticket->tid, 'node', $nid, 'ticket_page');
    dpm($page_relation,'created relation');
				foreach($content_array[$i]['topics'] as $topic) {
						// Add subsequent topics as the other endpoints in page->topic
      $page_relation = longform_relation_create('node', $nid, 'node', $topic, 'page_topic');
				}
				
  }//--foreach process (page)

  dpm($page_relation,'page relations');
  $ticket_state = longform_workflow_set_state($ticket, 'tid', 'postprocess', NULL, 'State set by importer');
}

/**
 * Takes the split level of the current document and chops the content into an array
 * Runs a function on cross-reference links that prepares them to be re-connected after node save
 */
function doc_import_split_document($ticket, &$content_sectioned){
  module_load_include('inc', 'doc_import');
  module_load_include('inc', 'doc_import', 'doc_import.links');
  module_load_include('inc', 'doc_import', 'doc_import.build');
  $content_sectioned = array();
  $ticket = entity_metadata_wrapper('longform_ticket', $ticket);

  $content_sectioned = doc_import_construct_document_array($ticket, $ticket->field_granularity->value(), array('source'=>'doc_split_save'));
		$index = 0;
  //Process links
  foreach($content_sectioned as &$section){
    $index++;
    doc_import_prepare_links($section, $index, $ticket->getIdentifier());
  }
}