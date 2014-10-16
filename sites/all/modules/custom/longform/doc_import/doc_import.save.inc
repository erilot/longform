<?php

/**
 * file: doc_import.save.inc
 * Creates books and other media from source topics
 */

 /**
 * Create book and save all book nodes.
 */
function doc_import_save_nodes($ticket, &$content_array){

  // Set up the ticket endpoint for the ticket->pages relation
  $ticket_contains_pages_endpoints[] = array(
      'entity_type' => 'node',
      'entity_id' => $ticket->nid,
  );

  $title = $ticket->title;
  $edit['type'] = 'book';    //@TODO make this variable
  $ticket_type_default = variable_get('node_options_' . $edit['type'], array('status', 'promote'));

  $fields = field_info_instances('node',$edit['type']);
 
  global $user; //get global user object
  $author_tool = $ticket->field_authoring_tool[LANGUAGE_NONE][0];
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
  
  //save top-level node
  node_save($cover);
  // Create page->ticket relation
  $page_to_ticket_relation = relation_create('entity_is_on_ticket', array(array('entity_type' => 'node', 'entity_id' => $cover->nid),array('entity_type' => 'node', 'entity_id' => $ticket->nid)));
  $page_to_ticket_relation_id = relation_save($page_to_ticket_relation);

  // Set cover page ticket->page endpoint 
  $ticket_contains_pages_endpoints[] = array(
      'entity_type' => 'node',
      'entity_id' => $cover->nid
  );
  
  // Record the cover page nid for future use (it's the book ID)
  $bid = $cover->nid;
  
		// *** This is never called and should be removed -- leftover from D6 code?
		//  // Establish the book outline
		//  if ($cover->book['bid'] == 0) {
		//				dpm('check');
		//      module_load_include('inc', 'book', 'book.pages');
		//      $book_form = array();
		//      $book_form['values']['book'] = array('bid' => $cover->nid, 'weight' => 0);
		//      drupal_form_submit('book_outline_form', $book_form, $cover);
		//  }

  // Transfer imported content to book pages
		$build = array();
		$build[0] = array('nid' => $cover->nid, 'title' => $cover->title, 'bid' => $cover->nid, 'mlid' => $cover->book['mlid'], 'plid' => $cover->book['plid'], 'weight' => 0, 'level' => 0);
  //$last_book_node = $cover;
  //$last_heading_type = 0;
  //$weight = 0;
  for ($i = 0; $i < count($content_array); $i++) {
    
				$book_node = clone $cover;
				$book_node->nid = 0;
				$book_node->vid = 0;
				
				$level = $content_array[$i]['level'];
				$find_heading = 'h' . $level;
				
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
				$book_node->parent = $cover->nid;
				$book_node->book['bid'] = $cover->nid;
				$book_node->book['menu_name'] = $cover->book['menu_name'];

				
				// find  parent
//				while (!empty($last_heading_type) && $level < $last_heading_type) {
//						_doc_import_get_book_parent($last_book_node);
//						$last_heading_type = _doc_import_get_book_heading_type($last_book_node);
//						$weight = _doc_import_get_book_weight($last_book_node);
//    }
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
				$build[] = array('nid' => $book_node->nid, 'title' => $book_node->title, 'bid' => $cover->nid, 'mlid' => $book_node->book['mlid'], 'plid' => $book_node->book['plid'], 'weight' => $weight, 'level' => $level);

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

						// Add subsequent topics as the other endpoints in page->topic
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

				//_doc_import_save_book_node($book_node, $level);
				$last_book_node = $book_node;
				$last_heading_type = $level;

				//_doc_import_cleanup($source_doc);
				watchdog('content', '@type: import %title with Doc Import module.', array('@type' => $cover->type, '%title' => $cover->title), WATCHDOG_NOTICE, l(t('view'), "document/$cover->nid"));

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
}

/**
 * Takes the split level of the current document and chops the content into an array
 * Runs a function on cross-reference links that prepares them to be re-connected after node save
 */
function doc_import_split_document($node, &$content_sectioned){
  module_load_include('inc', 'doc_import');
  module_load_include('inc', 'doc_import', 'doc_import.links');
  module_load_include('inc', 'doc_import', 'doc_import.build');
  $content_sectioned = array();
  $break_level = $node->field_granularity[LANGUAGE_NONE][0]['value'];
  //$master_file = $node->field_raw_html_master[LANGUAGE_NONE][0]['uri'];
  //$author_tool = $node->field_authoring_tool[LANGUAGE_NONE][0]['value'];
  
  $content_sectioned = doc_import_construct_document_array($node, $break_level, array('source'=>'doc_split_save'));
		$index = 0;
  //Process links
  foreach($content_sectioned as &$section){
    $index++;
    doc_import_prepare_links($section, $index, $node->nid);
  }
  
  //dpm(doc_import_read_link_file($node->nid),'link file');

//}
}