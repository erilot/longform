<?php

/**
 * Returns an array of entities contained by the specified relation on the current node
 *
 * Returns only the first matching relation found, so if there is more than one the results will be wrong.
 * This function should only be used when a single result is expected. Code based on relation_query() from the relation module.
 *
 * @param $relation_type
 * 	The machine name of the relation to query.
 *
 * @param $ticket
 * 	The loaded ticket object to use as the query base.
 *
 * @return
 * 	The endpoints array provided by the relation object.
 */
function doc_manage_relation_query($relation_type, $controller = NULL){
  //code adapted from relation_get_related_entity() in relation.module
  if (!$controller) $controller = menu_get_object();
  
    $query = relation_query('node', $controller->nid, 0)->range(0, 1);
    if ($relation_type) {
      $query->entityCondition('bundle', $relation_type);
    }
    $results = $query->execute();
    $result = reset($results);
    if ($result) {
      $relation = relation_load($result->rid);
      $entities = field_get_items('relation', $relation, 'endpoints');
    }
    else {
      $entities = FALSE;
    }
  return $entities;
}

/*
 * Get all book node IDs that belong to an import ticket.
 *
 * @param $ticket
 * 	The loaded ticket object to use in the query
 *
 * @param $full
 * 	Optional. If FALSE or not supplied, strips all extra information
 * 	from the query and returns a simple array of book nids.
 * 	If TRUE, the relation query is returned unaltered.
 *
 * @return
 * 	An array of book NIDs belonging to the import ticket. If $full is TRUE,
 * 	this will also contain other information (entity types and RID#s).
 * 	
 */
function doc_manage_get_book_pages($ticket, $full = FALSE){

  $relation = doc_manage_relation_query('ticket_controls_nodes', $ticket);
  if($full) {
    return $relation;
  }
  else {
    foreach($relation as $rel) {
      $pages[] = $rel['entity_id'];
    }
    
    // Dump the first element; it's the import ticket and we only want book nodes.
    array_shift($pages);
    
    return $pages;
  }
}

/**
 * Returns the book page endpoints given a component book page.
 *
 * This function requires a field to be set with the relation ID to work.
 *
 * @var $node
 * 	The book node.
 *
 * @var $full
 * 	(optional) if TRUE, return the entire relation object. Otherwise just return the endpoints.
 *
 * @return (array)
 * 	The book node IDs.
 */
function doc_manage_get_book_pages_from_book_page($node, $full = FALSE) {
  $wrapper = entity_metadata_wrapper('node', $node);
  
  if (isset($wrapper->field_document_relation)) {
    $relation = relation_load($wrapper->field_document_relation->value());
    if ($full) {
      return $relation;
    }
    else {
      foreach ($relation->endpoints[LANGUAGE_NONE] as $endpoint) {
	$pages[] = $endpoint['entity_id'];
      }
      return $pages;
    }
  }
  else {
    return FALSE;
  }
}


/*
 * Get a list of topic nodes controlled by an import ticket.
 *
 * @param $node
 * 	The loaded node object to use in the query
 *
 * @param $type
 * 	Optional. The machine name of the relation to use.
 * 	If not supplied, 'uses_topic' is assumed.
 *
 * @return
 * 	An associative array of topic titles, keyed by node ID.
 * 	
 */
function _doc_manage_get_topics($node, $type = NULL){
  
  // If no type is supplied, assume the "uses topic" relation
  if (!$type) {
    $type = "uses_topic";
  }
  
  //get endpoints and split off controller object ([0]).
  $endpoints = doc_manage_relation_query($type, $node);
  $parent = array_shift($endpoints);

  foreach($endpoints as $ep){
    $nids[] = $ep['entity_id'];
  }
    
  $query = db_query("SELECT nid, title FROM {node} WHERE nid IN (:nids)", array(':nids' => $nids));
  return $query->fetchAllAssoc('nid');

}

/*
 * Get a list of topic nodes used by a page node.
 *
 * @param $node
 * 	The loaded node object to use in the query
 *
 * @param $type
 * 	Optional. The machine name of the relation to use.
 * 	If not supplied, 'page_uses_topic' is assumed.
 *
 * @return
 * 	An associative array of topic titles, keyed by node ID.
 * 	
 */
function doc_manage_get_page_topics($node, $type = NULL){
  
  // If no type is supplied, assume the "uses topic" relation
  if (!$type) {
    $type = 'page_uses_topic';
  }
  
  $endpoints = doc_manage_relation_query($type, $node);

  foreach($endpoints as $ep){
    $nids[] = $ep['entity_id'];
  }
    
  $query = db_query("SELECT nid, title, type FROM {node} WHERE nid IN (:nids) ", array(':nids' => $nids));
  $result = $query->fetchAllAssoc('nid');
  foreach($result as $record) {
    if($record->type != 'topic') continue;
    $list[$record->nid] = $record->title;
  }
  return $list;

}

/**
 * Utility function that returns a list of all all uses_topic relations
 */

function _doc_manage_get_topic_relations($relation_type = NULL){
  
  if(!$relation_type) {
    $relation_type = 'uses_topic';
  }
  
  $query = db_query("SELECT rid FROM {relation} WHERE relation_type IN (:types)", array(':types' => $relation_type));
  return ($query->fetchCol());
}

/**
 * Utility function that returns all orphaned relations (relations without a controller).
 */
function _doc_manage_get_orphaned_topics($relation_type = NULL){

  $topic_relations = _doc_manage_get_topic_relations($relation_type);    
  $relations = relation_load_multiple($topic_relations);
  
  foreach($relations as $relation){
    if (!$relation->endpoints){
      $abandoned[$relation->rid] = $relation;
      continue;
    }
    $parent = node_load($relation->endpoints[LANGUAGE_NONE][0]['entity_id']);
    if($parent->type != 'import_ticket'){
      $orphans[$relation->rid] = $relation;
    }
  }
  return array('abandoned'=>$abandoned,'orphans'=>$orphans);
  
}

/**
 * Get the import ticket NID given a component book page.
 *
 * @var $page
 * 	The page object.
 *
 * @return (int)
 * 	The import ticket NID.
 */
function doc_manage_get_ticket_from_page($page) {

  $wrap = entity_metadata_wrapper('node', $page);
  // Get the book relation ID. Confusingly, this is the relation named 'ticket_controls_nodes', and the field is 'field_document_relation'.
  try {
    $rid = $wrap->field_document_relation->value();
    $relation = relation_load($rid);
    $endpoints = $relation->endpoints[LANGUAGE_NONE];
    $ticket = array_shift($endpoints);
  }
  catch (Exception $e) {
    watchdog('doc_manage', 'Entity metadata wrapper error: field "field_document_relation" probably has no value.', WATCHDOG_WARNING);
    return FALSE;
  }
  return $ticket['entity_id'];
}
