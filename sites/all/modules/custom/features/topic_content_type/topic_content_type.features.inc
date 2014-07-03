<?php
/**
 * @file
 * topic_content_type.features.inc
 */

/**
 * Implements hook_node_info().
 */
function topic_content_type_node_info() {
  $items = array(
    'topic' => array(
      'name' => t('Topic'),
      'base' => 'node_content',
      'description' => t('Individual topics that are used to assemble documents'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
