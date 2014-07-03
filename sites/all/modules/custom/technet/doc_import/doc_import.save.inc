<?php

/**
 * file: doc_import.save.inc
 * Creates books and other media from source topics
 */

 /**
 * Create book and save all book nodes.
 */
function doc_import_save_nodes($ticket_node, &$meta, &$log, &$content_array){
  
  //dpm($content_array,'content array');

    //Set ticket node relation endpoints: First relation endpoint will be the ticket node  
    $relation_endpoints[] = array(
	'entity_type' => 'node',
        'entity_id' => $ticket_node->nid,
    );

    $meta['book_nid'] = $ticket_node->nid;
    $meta['author_tool'] = $ticket_node->field_authoring_tool[LANGUAGE_NONE][0];
    
    $title = $ticket_node->title;
    $edit['type'] = 'book';    //@TODO make this variable
    $ticket_node_type_default = variable_get('node_options_' . $edit['type'], array('status', 'promote'));

    $fields = field_info_instances('node',$edit['type']);
    
    global $user; //get global user object
    $author_tool = $meta['author_tool']['tool'];
    $edit['uid'] = $user->uid;
    $edit['name'] = $user->name;

    
    $edit['promote'] = FALSE;
    $edit['comment'] = variable_get('comment_' . $edit['type'], 2);
    $edit['format'] = filter_default_format();
    $edit['status'] = 1;

    $edit['language'] = $ticket_node->language;

    //copy fields
    foreach($fields as $name => $field){
      $edit[$name] = $ticket_node->$name;
    }
    $title = html_entity_decode($title,ENT_QUOTES,'utf-8');
    $edit['title'] = $title;
    
    //Top level book nodes have no body content
    $edit['body'][LANGUAGE_NONE][0]['value'] = NULL;
    $edit['body'][LANGUAGE_NONE][0]['summary'] = NULL;
    $edit['body'][LANGUAGE_NONE][0]['format'] = 'full_html';
    $edit['book'] = array('bid' => 'new');

   //dpm($edit,'$edit before recording node');
    $node = (object) node_submit((object) $edit);
    //dpm($node, 'node as prepared');
    
    //save top-level node
    node_save($node);

    //Set ticket relation endpoints: second relation endpoint will be the top level book node  
    $relation_endpoints[] = array(
	'entity_type' => 'node',
        'entity_id' => $node->nid
    );

    //record bid for future use (bid == top level node NID)
    $bid = $node->nid;    
	
    // make sure the node is a book root
    if ($node->book['bid'] == 0) {
	module_load_include('inc', 'book', 'book.pages');
	$book_form = array();
	$book_form['values']['book']['bid'] = $node->nid;
	$book_form['values']['book']['weight'] = 0;
	drupal_form_submit('book_outline_form', $book_form, $node);
    }
    
    // import headings to book nodes
    $num_doc_sections = count($content_array);
    $last_book_node = $node;
    $last_heading_type = 0;
    $weight = 0;
    
    for ($i = 0; $i < $num_doc_sections; $i++) {
      
	$book_node = clone $node;
	$book_node->nid = 0;
	$book_node->vid = 0;
	
	$section_title = $content_array[$i]['title'];
	$book_node->title = html_entity_decode($section_title, ENT_QUOTES | 'ENT_HTML401', 'UTF-8');
	$heading_type = $content_array[$i]['level'];
	$find_heading = 'h' . $heading_type;
	
	//strip same-level heading from content. This wil be attached separately via $book_node->title.
	$content = htmlqp($content_array[$i]['content'])->find($find_heading)->remove()->top()->html();
	
	//remove extra semicolon from double-encoded ldquo/lsquo
	//@TODO: Find out where this is happening and remove the band-aid
	$content = str_replace('quo;;','quo;', $content);

	
	$book_node->body[LANGUAGE_NONE][0]['value'] = $content;
	$book_node->body[LANGUAGE_NONE][0]['summary'] = text_summary($content);
	$book_node->body[LANGUAGE_NONE][0]['format'] = 'full_html';
	$book_node->path['pathauto'] = TRUE;
	$book_node->teaser = text_summary($content);
	$book_node->parent = $node->nid;
	$book_node->book['bid'] = $node->nid;
	$book_node->book['menu_name'] = $node->book['menu_name'];

	// find proper parent
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
      
	//log $nid for postprocessing
	$saved_nids[] = $nid;
      
	//add this section's links to an array (processed later)
	if($meta['links'][$i]){
	    foreach ($meta['links'][$i] as &$section_links){
		$section_links['nid'] = $nid;
	    }
	}

	//Add to ticket node relation endpoints array
	$relation_endpoints[] = array(
	    'entity_type' => 'node',
	    'entity_id' => $nid
	);
	
	$page_uses_topic = array();
	
	// Create 'topic_used_in_page' relation for this page
	$page_uses_topic[0] = array(
	    'entity_type' => 'node',
	    'entity_id' => $nid,
	);
	foreach($content_array[$i]['topics'] as $topic) {
	    $page_uses_topic [] = array(
		'entity_type' => 'node',
		'entity_id' => $topic,
	    );
	    
	    // Create a topic_in_entity relation for each topic, pointing to this page
	    $topic_in_entity = array();
	    $topic_in_entity = array(
		array(
		    'entity_type' => 'node',
		    'entity_id' => $topic,
		),
		array(
		    'entity_type' => 'node',
		    'entity_id' => $nid,
		),
	    );
	    
	    $topic_in_entity_relation = relation_create('topic_in_entity', $topic_in_entity);
	    $new_topic_in_entity_id = relation_save($topic_in_entity_relation);
	}
	
	// Create and save topic_used_in_page relation
	$page_uses_topic_relation = relation_create('page_uses_topic',$page_uses_topic);
	$new_relation_id = relation_save($page_uses_topic_relation);

	//_doc_import_save_book_node($book_node, $heading_type);
	$last_book_node = $book_node;
	$last_heading_type = $heading_type;

	//_doc_import_cleanup($source_doc);
	watchdog('content', '@type: import %title with Doc Import module.', 
	  array('@type' => $node->type, '%title' => $node->title), WATCHDOG_NOTICE, l(t('view'), "document/$node->nid"));

    }//section foreach 
  
    //create relations
    $new_relation = relation_create('ticket_controls_nodes',$relation_endpoints);
    $new_relation_id = relation_save($new_relation);
    
    //set top-level node workflow state to Approved (it has no content to edit) -- hardcoded ID#14 -- and assign relation:
    $tempnode = node_load($bid);
    workflow_execute_transition($tempnode, 14, 'State change by rule during import', TRUE);
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
  $break_level = array_pop(explode(' ',taxonomy_term_load($node->field_heading_split_level[LANGUAGE_NONE][0]['tid'])->name));
  $master_file = $node->field_imported_html_master[LANGUAGE_NONE][0]['uri'];
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