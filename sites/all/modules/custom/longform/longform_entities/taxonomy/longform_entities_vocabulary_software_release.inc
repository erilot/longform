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
function longform_entities_vocabulary_software_release() {

  $vocab = array(
    'name' => 'Software Release',
    'machine_name' => 'software_release',
    'replace_if_exists' => TRUE,
  );

  $terms = array();
  // Field definition
  $field = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_' . $vocab['machine_name'],
    'foreign keys' => array(
      'tid' => array(
        'columns' => array(
          'tid' => 'tid',
        ),
        'table' => 'taxonomy_term_data',
      ),
    ),
    'indexes' => array(
      'tid' => array(
        0 => 'tid',
      ),
    ),
    'locked' => 0,
    'module' => 'taxonomy',
    'settings' => array(
      'allowed_values' => array(
        0 => array(
          'vocabulary' => $vocab['machine_name'],
          'parent' => 0,
        ),
      ),
    ),
    'translatable' => 0,
    'type' => 'taxonomy_term_reference',
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
          'module' => 'taxonomy',
          'settings' => array(),
          'type' => 'taxonomy_term_reference_link',
          'weight' => 2,
        ),
      ),
      'entity_type' => 'longform_set',
      'fences_wrapper' => 'div',
      'field_name' => 'field_' . $vocab['machine_name'],
      'label' => 'Set ' . $vocab['name'],
      'required' => 1,
      'settings' => array(
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 0,
        'module' => 'taxonomy',
        'settings' => array(
          'autocomplete_path' => 'taxonomy/autocomplete',
          'size' => 60,
        ),
        'type' => 'taxonomy_autocomplete',
        'weight' => 8,
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
          'module' => 'taxonomy',
          'settings' => array(),
          'type' => 'taxonomy_term_reference_link',
          'weight' => 2,
        ),
      ),
      'entity_type' => 'longform_ticket',
      'fences_wrapper' => 'div',
      'field_name' => 'field_' . $vocab['machine_name'],
      'label' => 'Set ' . $vocab['name'],
      'required' => 1,
      'settings' => array(
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 0,
        'module' => 'taxonomy',
        'settings' => array(
          'autocomplete_path' => 'taxonomy/autocomplete',
          'size' => 60,
        ),
        'type' => 'taxonomy_autocomplete',
        'weight' => 8,
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

