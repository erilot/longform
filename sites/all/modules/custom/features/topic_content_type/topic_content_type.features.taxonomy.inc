<?php
/**
 * @file
 * topic_content_type.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function topic_content_type_taxonomy_default_vocabularies() {
  return array(
    'document_author' => array(
      'name' => 'Document Author',
      'machine_name' => 'document_author',
      'description' => 'The original author of the document',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
  );
}
