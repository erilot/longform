<?php

/**
 * Get the other endpoint in a relation.
 *
 * @param $entity
 *  The loaded entity.
 *
 * @param $relation_type
 *  The relation machine name.
 *
 * return (array)
 *  The other endpoints, loaded.
 */
function longform_relation_get_other_endpoint($type, $entity, $relation_type, $load = TRUE) {

  $wrap = entity_metadata_wrapper($type, $entity);
  $query = relation_query($type, $wrap->getIdentifier());
  $query->entityCondition('bundle', $relation_type);

  $results = $query->execute();
  $endpoints = array();
  foreach (relation_load_multiple(array_keys($results)) as $relation) {
    // The order of endpoints can't be relied on, so do a filter operation to get the opposite end
    $ep = reset(array_filter(
      $relation->endpoints[LANGUAGE_NONE],
      function ($el) use ($type) {
        return $el['entity_type'] != $type;
      }
    ));
    if ($load == TRUE) {
      $endpoints[$ep['entity_id']] = reset(entity_load($ep['entity_type'], array($ep['entity_id'])));
    }
    else {
      $endpoints[$ep['entity_id']] = $ep['entity_type'];
    }
  }
  return $endpoints;
}


/**
 * Get the other endpoints in a set <--> ticket relation.
 *
 * @param $entity
 *  The loaded object.
 *
 * @return array
 *  An array of loaded objects
 *
 */
function longform_relation_get_other_endpoint_set_ticket($entity, $load = TRUE) {
  return longform_relation_get_other_endpoint($entity->type, $entity, 'set_ticket', $load);
}

/**
 * Get the other endpoints in a ticket <--> topic relation.
 *
 * @param $entity
 *  The loaded object.
 *
 * @return array
 *  An array of loaded objects
 *
 */
function longform_relation_get_other_endpoint_ticket_topic($entity, $load = TRUE) {
  return longform_relation_get_other_endpoint($entity->type == 'topic' ? 'node' : $entity->type, $entity, 'ticket_topic', $load);
}

/**
 * Get the other endpoints in a ticket <--> page relation.
 *
 * @param $entity
 *  The loaded object.
 *
 * @return array
 *  An array of loaded objects
 *
 */
function longform_relation_get_other_endpoint_ticket_page($entity, $load = TRUE) {
  return longform_relation_get_other_endpoint($entity->type == 'book' ? 'node' : $entity->type, $entity, 'ticket_page', $load);
}

/**
 * Create a relation between two entities.
 *
 * @param $first_entity_type
 *  The first entity type (e.g. 'node', 'longform_set')
 *
 * @param $first_entity_id
 *  The first entity ID
 *
 * @param $second_entity_type
 *  The second entity type
 *
 * @param $second_entity_id
 *  The second entity ID
 *
 * @param $relation_type
 *  The relation machine name.
 *
 * @return
 *  The created relation, or FALSE if one was not created.
 */
function longform_relation_create($first_entity_type, $first_entity_id, $second_entity_type, $second_entity_id, $relation_type) {

  $endpoints = array(
    array(
      'entity_type' => $first_entity_type,
      'entity_id' => $first_entity_id,
    ),
    array(
      'entity_type' => $second_entity_type,
      'entity_id' => $second_entity_id,
    ),
  );
  return relation_insert($relation_type, $endpoints);

}