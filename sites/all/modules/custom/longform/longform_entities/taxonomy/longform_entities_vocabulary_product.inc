<?php

/**
 *  To automatically create and populate a vocabulary on install,
 *  use this file as a template and save your new vocabulary config
 *  in the /longform_entities/taxonomy directory. It will be discovered
 *  and loaded when the longform entities module is installed.
 *
 *  Requirements:
 *    - Filename must follow this format: "longform_entities_vocabulary_[UNIQUE_NAME].inc"
 *        (where UNIQUE_NAME is arbitrary - it will not be used to create any data, just to namespace)
 *    - function name must be the same as the filename (less the ".inc" extension)
 *    - The function must return an array with a 'vocab' key and a 'terms' key:
 *
 *      - $vocab = array (
 *          'name' => the human-readable name of the new vocabulary
 *          'machine_name' => the machine name of the new vocabulary
 *          'replace_if_exists' => (boolean) whether to overwrite an existing vocabulary with
 *                the same name.
 *        );
 *
 *      $terms = array(
 *        'New term name' => either '', if no children, or a nested array of child terms.
 *          This pattern can be repeated as many times as needed for deep taxonomies.
 *
 *          Example:
 *            $vocab = array(
 *              'name' => 'Document Type',
 *              'machine_name' => 'document_type',
 *              'replace_if_exists' => TRUE,
 *            );
 *
 *            $terms['Administrator Documents'] = array(
 *              'Administrator Guide' => '',
 *              'Release Notes' => '',
 *            );
 *
 *            return array('vocab' => $vocab, 'terms => $terms);
 *
 */

/**
 * @return array
 */
function longform_entities_vocabulary_product() {

  $vocab = array(
    'name' => 'Product',
    'machine_name' => 'product',
    'replace_if_exists' => TRUE,
  );

  $terms = array();

  // Field definition
  $field = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_' . $vocab['machine_name'],
    'foreign keys' => array(
      'taxonomy_term_data' => array(
        'columns' => array(
          'target_id' => 'tid',
        ),
        'table' => 'taxonomy_term_data',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'dereference_on_delete' => 1,
      'handler' => 'base',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'sort' => array(
          'type' => 'none',
        ),
        'target_bundles' => array(
          $vocab['machine_name'] => $vocab['machine_name'],
        ),
      ),
      'target_type' => 'taxonomy_term',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  // Field instances definitions (note: array of arrays, each instance is a separate array element)
  $instances = array(
    'longform_set' => array(
      'bundle' => 'longform_set',
      'default_value' => NULL,
      'deleted' => 0,
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'entityreference',
          'settings' => array(
            'link' => FALSE,
          ),
          'type' => 'entityreference_label',
          'weight' => 1,
        ),
      ),
      'entity_type' => 'longform_set',
      'fences_wrapper' => 'div',
      'field_name' => 'field_' . $vocab['machine_name'],
      'label' => 'Set ' . strtolower($vocab['name']),
      'required' => 1,
      'settings' => array(
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'entityreference',
        'settings' => array(
          'match_operator' => 'CONTAINS',
          'path' => '',
          'size' => 60,
        ),
        'type' => 'entityreference_autocomplete',
        'weight' => 7,
      ),
    ),

    'longform_ticket' => array(
      'bundle' => 'longform_ticket',
      'default_value' => NULL,
      'deleted' => 0,
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'entityreference',
          'settings' => array(
            'link' => FALSE,
          ),
          'type' => 'entityreference_label',
          'weight' => 1,
        ),
      ),
      'entity_type' => 'longform_ticket',
      'fences_wrapper' => 'div',
      'field_name' => 'field_' . $vocab['machine_name'],
      'label' => 'Set ' . strtolower($vocab['name']),
      'required' => 0,
      'settings' => array(
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'entityreference',
        'settings' => array(
          'match_operator' => 'CONTAINS',
          'path' => '',
          'size' => 60,
        ),
        'type' => 'entityreference_autocomplete',
        'weight' => 7,
      ),
    ),
  );

  return array(
    'vocab' => $vocab,
    'terms' => $terms,
    'field' => $field,
    'instances' => $instances
  );
}
