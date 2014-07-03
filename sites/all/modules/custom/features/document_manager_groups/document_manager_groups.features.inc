<?php
/**
 * @file
 * document_manager_groups.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function document_manager_groups_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "ds" && $api == "ds") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function document_manager_groups_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function document_manager_groups_node_info() {
  $items = array(
    'documentation_group' => array(
      'name' => t('Documentation Group'),
      'base' => 'node_content',
      'description' => t('Groups can be geographical, functional, conceptual, or anything else that makes logical sense.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
