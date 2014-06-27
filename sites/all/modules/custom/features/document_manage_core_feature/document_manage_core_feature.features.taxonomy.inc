<?php
/**
 * @file
 * document_manage_core_feature.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function document_manage_core_feature_taxonomy_default_vocabularies() {
  return array(
    'document_author' => array(
      'name' => 'Document Author',
      'machine_name' => 'document_author',
      'description' => 'The original author of the document',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'heading_split_level' => array(
      'name' => 'Heading split level',
      'machine_name' => 'heading_split_level',
      'description' => 'The available heading levels to use as break points when importing a document',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'marketing_category' => array(
      'name' => 'Marketing Category',
      'machine_name' => 'marketing_category',
      'description' => 'These categories map to product information on the marketing site.',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'part_number' => array(
      'name' => 'Part Number',
      'machine_name' => 'part_number',
      'description' => 'A version control assignment. This is NOT tracked on Drupal, and is not validated as unique.',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'platform' => array(
      'name' => 'Platform',
      'machine_name' => 'platform',
      'description' => 'Desktop, Windows, OSX, Mobile, iOS, Android, etc.',
      'hierarchy' => 1,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'product_manager' => array(
      'name' => 'Product Manager',
      'machine_name' => 'product_manager',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'project_name' => array(
      'name' => 'Project Name',
      'machine_name' => 'project_name',
      'description' => 'The name of the project this document is associated with.',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 3,
    ),
    'software_release' => array(
      'name' => 'Software Release',
      'machine_name' => 'software_release',
      'description' => 'Release number for software. This field is meaningless without a Device Type to contextualize it.',
      'hierarchy' => 1,
      'module' => 'taxonomy',
      'weight' => -7,
    ),
    'vocabulary_10' => array(
      'name' => 'Audience',
      'machine_name' => 'vocabulary_10',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => -10,
    ),
    'vocabulary_11' => array(
      'name' => 'Partner',
      'machine_name' => 'vocabulary_11',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => -4,
    ),
    'vocabulary_12' => array(
      'name' => 'Device Type',
      'machine_name' => 'vocabulary_12',
      'description' => 'Selects device type in a hierarchy, from general to specific.',
      'hierarchy' => 1,
      'module' => 'taxonomy',
      'weight' => -9,
    ),
    'vocabulary_8' => array(
      'name' => 'Document Type',
      'machine_name' => 'vocabulary_8',
      'description' => '',
      'hierarchy' => 1,
      'module' => 'taxonomy',
      'weight' => -6,
    ),
  );
}
