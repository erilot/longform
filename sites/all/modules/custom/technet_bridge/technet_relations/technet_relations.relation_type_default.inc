<?php

/**
 * @file
 * Bulk export of relation_type_default objects generated by Bulk export module.
 */

/**
 * Implements hook_relation_default_relation_types().
 */
function longform_relations_relation_default_relation_types() {
  $relation_types = array();

  $relation_type = new stdClass();
  $relation_type->disabled = FALSE; /* Edit this to true to make a default relation_type disabled initially */
  $relation_type->api_version = 1;
  $relation_type->relation_type = 'ticket_contains_book_pages';
  $relation_type->label = 'is a record in';
  $relation_type->reverse_label = 'is a record in';
  $relation_type->directional = 0;
  $relation_type->transitive = 0;
  $relation_type->r_unique = 0;
  $relation_type->min_arity = 2;
  $relation_type->max_arity = 0;
  $relation_type->source_bundles = array(
    0 => 'node:book',
    1 => 'node:import_ticket',
  );
  $relation_type->target_bundles = array();
  $relation_types['ticket_contains_book_pages'] = $relation_type;

  $relation_type = new stdClass();
  $relation_type->disabled = FALSE; /* Edit this to true to make a default relation_type disabled initially */
  $relation_type->api_version = 1;
  $relation_type->relation_type = 'topic_used_on_page';
  $relation_type->label = 'Page uses topic';
  $relation_type->reverse_label = 'topic_used_on_page';
  $relation_type->directional = 0;
  $relation_type->transitive = 0;
  $relation_type->r_unique = 0;
  $relation_type->min_arity = 2;
  $relation_type->max_arity = 0;
  $relation_type->source_bundles = array(
    0 => 'node:article',
    1 => 'node:book',
    2 => 'node:page',
    3 => 'node:topic',
  );
  $relation_type->target_bundles = array();
  $relation_types['topic_used_on_page'] = $relation_type;

  $relation_type = new stdClass();
  $relation_type->disabled = FALSE; /* Edit this to true to make a default relation_type disabled initially */
  $relation_type->api_version = 1;
  $relation_type->relation_type = 'uses_topic';
  $relation_type->label = 'uses topic';
  $relation_type->reverse_label = 'topic is used by';
  $relation_type->directional = 0;
  $relation_type->transitive = 0;
  $relation_type->r_unique = 0;
  $relation_type->min_arity = 2;
  $relation_type->max_arity = 0;
  $relation_type->source_bundles = array(
    0 => 'node:book',
    1 => 'node:import_ticket',
    2 => 'node:topic',
  );
  $relation_type->target_bundles = array();
  $relation_types['uses_topic'] = $relation_type;

  return $relation_types;
}
