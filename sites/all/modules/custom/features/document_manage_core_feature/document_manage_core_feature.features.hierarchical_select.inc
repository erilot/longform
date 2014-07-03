<?php
/**
 * @file
 * document_manage_core_feature.features.hierarchical_select.inc
 */

/**
 * Implements hook_hierarchical_select_default_configs().
 */
function document_manage_core_feature_hierarchical_select_default_configs() {
$configs = array();
$config = array(
  'config_id'       => 'taxonomy-12',
  'save_lineage'    => 0,
  'enforce_deepest' => 0,
  'entity_count'    => 0,
  'require_entity'  => 0,
  'resizable'       => 0,
  'level_labels' => array(
    'status' => 1,
    'labels' => array(
      0 => 'Product Family...',
      1 => 'Product Group...',
      2 => 'Product Variant...',
      3 => '',
      4 => '',
    ),
  ),
  'dropbox' => array(
    'status'    => 1,
    'title'     => 'Assigned Devices',
    'limit'     => 0,
    'reset_hs'  => 0,
  ),
  'editability' => array(
    'status' => 0,
    'item_types' => array(
      0 => '',
      1 => '',
      2 => '',
      3 => '',
      4 => '',
    ),
    'allowed_levels' => array(
      0 => 1,
      1 => 1,
      2 => 1,
      3 => 1,
      4 => 1,
    ),
    'allow_new_levels' => 0,
    'max_levels'       => 2,
  ),
);

$configs['taxonomy-12'] = $config;
$config = array(
  'config_id'       => 'taxonomy-8',
  'save_lineage'    => 0,
  'enforce_deepest' => 0,
  'entity_count'    => 0,
  'require_entity'  => 0,
  'resizable'       => 0,
  'level_labels' => array(
    'status' => 1,
    'labels' => array(
      0 => '',
      1 => '',
      2 => '',
    ),
  ),
  'dropbox' => array(
    'status'    => 0,
    'title'     => 'All selections',
    'limit'     => 0,
    'reset_hs'  => 1,
  ),
  'editability' => array(
    'status' => 0,
    'item_types' => array(
      0 => 'Document Category',
      1 => 'Document Type',
      2 => 'Document Sub-Type',
    ),
    'allowed_levels' => array(
      0 => 1,
      1 => 1,
      2 => 1,
    ),
    'allow_new_levels' => 0,
    'max_levels'       => 0,
  ),
);

$configs['taxonomy-8'] = $config;
return $configs;
}
