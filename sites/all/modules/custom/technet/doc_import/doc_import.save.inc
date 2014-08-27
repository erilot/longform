<?php

/**
 * file: doc_import.save.inc
 * Creates books and other media from source topics
 */

 /**
 * Create book and save all book nodes.
 */
function doc_import_save_nodes($ticket, &$meta, &$log, &$content_array){
  

  // Set up the ticket endpoint for the ticket->pages relation
  $ticket_contains_pages_endpoints[] = array(
      'entity_type' => 'node',
      'entity_id' => $ticket->nid,
  );

  $meta['book_nid'] = $ticket->nid;
  $meta['author_tool'] = $ticket->field_authoring_tool[LANGUAGE_NONE][0];
  
  $title = $ticket->title;
  $edit['type'] = 'book';    //@TODO make this variable
  $ticket_type_default = variable_get('node_options_' . $edit['type'], array('status', 'promote'));

  $fields = field_info_instances('node',$edit['type']);
 
  global $user; //get global user object
  $author_tool = $meta['author_tool']['tool'];
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

 //dpm($edit,'$edit before recording node');
  $node = (object) node_submit((object) $edit);
  
  //save top-level node
  node_save($node);

  // Create page->ticket relation
  $page_to_ticket_relation = relation_create('entity_is_on_ticket', array(array('entity_type' => 'node', 'entity_id' => $node->nid),array('entity_type' => 'node', 'entity_id' => $ticket->nid)));
  $page_to_ticket_relation_id = relation_save($page_to_ticket_relation);

  // Set cover page ticket->page endpoint 
  $ticket_contains_pages_endpoints[] = array(
      'entity_type' => 'node',
      'entity_id' => $node->nid
  );
  
  // Record the cover page nid for future use (it's the book ID)
  $bid = $node->nid;
  
  // Establish the book outline
  if ($node->book['bid'] == 0) {
      module_load_include('inc', 'book', 'book.pages');
      $book_form = array();
      $book_form['values']['book'] = array('bid' => $node->nid, 'weight' => 0);
      drupal_form_submit('book_outline_form', $book_form, $node);
  }

  // Transfer imported content to book pages
  $last_book_node = $node;
  $last_heading_type = 0;
  $weight = 0;
  
  for ($i = 0; $i < count($content_array); $i++) {
    
      $book_node = clone $node;
      $book_node->nid = 0;
      $book_node->vid = 0;
      
      $heading_type = $content_array[$i]['level'];
      $find_heading = 'h' . $heading_type;
      
      // Strip same-level heading from content and set it as the node title instead
      $content = htmlqp($content_array[$i]['content'])->find($find_heading)->remove()->top()->html();
      $book_node->title = html_entity_decode($content_array[$i]['title'], ENT_QUOTES | 'ENT_HTML401', 'UTF-8');

      //remove extra semicolon from double-encoded ldquo/lsquo
      //@TODO: Find out where this is happening and remove the band-aid
      $content = str_replace('quo;;','quo;', $content);

      $book_node->field_book_body[LANGUAGE_NONE][0]['value'] = $content;
      $book_node->field_book_body[LANGUAGE_NONE][0]['format'] = 'full_html';
      $book_node->path['pathauto'] = TRUE;
      $book_node->teaser = text_summary($content);
      $book_node->parent = $node->nid;
      $book_node->book['bid'] = $node->nid;
      $book_node->book['menu_name'] = $node->book['menu_name'];

      // find  parent
      while (!empty($last_heading_type) && $heading_type < $last_heading_type) {
	  _doc_import_get_book_parent($last_book_node);
	  $last_heading_type = _doc_import_get_book_heading_type($last_book_node);
	  $weight = _doc_import_get_book_weight($last_book_node);
      }

      if ($heading_type > $last_heading_type) { // at lower level
	  $weight = 0;
	  $book_node->book['weight'] = $weight;
	  $book_node->book['plid'] = $last_book_node->book['mlid'];
      }
      elseif ($heading_type == $last_heading_type) { // at the same level
	  $weight++;
	  $book_node->book['weight'] = $weight;
	  $book_node->book['plid'] = $last_book_node->book['plid'];
      }

      node_save($book_node);
      $nid = $book_node->nid;
      $saved_nids[] = $nid;
    
      // Add to ticket node relation endpoints array
      $ticket_contains_pages_endpoints[] = array(
	  'entity_type' => 'node',
	  'entity_id' => $nid
      );

      // Create page->topics relation for this page      
      $page_uses_topics = array();
      
      // Add the page as the first page->topics endpoint
      $page_uses_topics[] = array(
	  'entity_type' => 'node',
	  'entity_id' => $nid,
      );
      foreach($content_array[$i]['topics'] as $topic) {

      // Add subsequent topics as the other endpoints in page->topics

	$page_uses_topics [] = array(
	    'entity_type' => 'node',
	    'entity_id' => $topic,
	);
	
	// Create a relation pointing from the topic back to the page (topic->page)
	$topic_used_on_page = array();
	$topic_used_on_page = array(
	  array(
	      'entity_type' => 'node',
	      'entity_id' => $topic,
	  ),
	  array(
	      'entity_type' => 'node',
	      'entity_id' => $nid,
	  ),
	);
	
	$topic_used_on_page_relation = relation_create('topic_used_on_page', $topic_used_on_page);
	$new_topic_used_on_page_id = relation_save($topic_used_on_page_relation);
      }
      
      // Create and save topic_used_in_page relation
      $page_uses_topics_relation = relation_create('page_uses_topics',$page_uses_topics);
      $page_uses_topics_relation_id = relation_save($page_uses_topics_relation);

      // Create page->ticket relation
      $page_to_ticket_relation = relation_create('entity_is_on_ticket', array(array('entity_type' => 'node', 'entity_id' => $nid),array('entity_type' => 'node', 'entity_id' => $ticket->nid)));
      $page_to_ticket_relation_id = relation_save($page_to_ticket_relation);
  
      //_doc_import_save_book_node($book_node, $heading_type);
      $last_book_node = $book_node;
      $last_heading_type = $heading_type;

      //_doc_import_cleanup($source_doc);
      watchdog('content', '@type: import %title with Doc Import module.', 
	array('@type' => $node->type, '%title' => $node->title), WATCHDOG_NOTICE, l(t('view'), "document/$node->nid"));

  }//--foreach process (page)

  // Create ticket->pages relation
  $ticket_contains_pages_relation = relation_create('ticket_contains_book_pages',$ticket_contains_pages_endpoints);
  $ticket_contains_pages_relation_id = relation_save($ticket_contains_pages_relation);
  
  //set top-level node workflow state to Approved (it has no content to edit) and assign relation:
  $tempnode = node_load($bid);
  workflow_execute_transition($tempnode, WORKFLOW_PAGE_APPROVED_STATE, 'State change by rule during import', TRUE);
  entity_get_controller('node')->resetCache(array($tempnode->nid));
  
  $tempnode->field_document_relation[LANGUAGE_NONE][0]['value'] = $new_relation_id;
  node_save($tempnode);
  
  //set other nodes to workflow state "In Review" and assign relation
  foreach($saved_nids as $saved_nid){
      $tempnode = node_load($saved_nid);
      $tempnode->field_document_relation[LANGUAGE_NONE][0]['value'] = $new_relation_id;
      node_save($tempnode);
  }
    

}//end 

/**
 * Takes the split level of the current document and chops the content into an array
 * Runs a function on cross-reference links that prepares them to be re-connected after node save
 */
function doc_import_split_document($node, &$meta, &$log, &$content_sectioned){
    module_load_include('inc', 'doc_import');
    module_load_include('inc', 'doc_import', 'doc_import.links');
    module_load_include('inc', 'doc_import', 'doc_import.build');
  $content_sectioned = array();
  $break_level = $node->field_granularity[LANGUAGE_NONE][0]['value'];
  $master_file = $node->field_raw_html_master[LANGUAGE_NONE][0]['uri'];
  $author_tool = $node->field_authoring_tool[LANGUAGE_NONE][0]['value'];
  
  $content_sectioned = doc_import_construct_document_array($node, $break_level, array('source'=>'doc_split_save'));

  //Process links
  foreach($content_sectioned as &$section){
    $index++;
    doc_import_prepare_links($section, $index, $node->nid);
  }
  
  //dpm(doc_import_read_link_file($node->nid),'link file');

//}
}