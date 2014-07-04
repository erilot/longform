<?php
/**
 * @file
 * document_manage_core_feature.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function document_manage_core_feature_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'node-book-body'
  $field_instances['node-book-body'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '<p>
	A <em>book page</em> is a page of content, organized into a collection of related entries collectively known as a <em>book</em>. A <em>book page</em> automatically displays links to adjacent pages, providing a simple navigation system for organizing and reviewing structured content.</p>
',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 1,
      ),
      'document_print_version' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 1,
      ),
      'print' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 1,
      ),
      'print_document' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'hidden',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 1,
      ),
      'teaser' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(
          'trim_length' => 600,
        ),
        'trim_length' => 600,
        'type' => 'text_summary_or_trimmed',
        'weight' => 1,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'body',
    'label' => 'Body',
    'required' => 0,
    'settings' => array(
      'display_summary' => 0,
      'mediafront' => array(
        'custom' => '',
        'field_type' => 0,
        'media_type' => 'media',
        'preview' => 0,
        'thumbnail' => 0,
      ),
      'text_processing' => 1,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 20,
        'summary_rows' => 5,
      ),
      'type' => 'text_textarea_with_summary',
      'weight' => 1,
    ),
  );

  // Exported field_instance: 'node-book-field_document_author'
  $field_instances['node-book-field_document_author'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'The original author of this document',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 16,
      ),
      'document_print_version' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 16,
      ),
      'print' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 16,
      ),
      'print_document' => array(
        'label' => 'above',
        'module' => 'hs_taxonomy',
        'settings' => array(),
        'type' => 'hs_taxonomy_term_reference_hierarchical_text',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 16,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_document_author',
    'label' => 'Document Author',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'hs_taxonomy',
      'settings' => array(
        'autocomplete_path' => 'taxonomy/autocomplete',
        'content_taxonomy_autocomplete_new_terms' => 'allow',
        'size' => 60,
      ),
      'type' => 'taxonomy_hs',
      'weight' => 7,
    ),
  );

  // Exported field_instance: 'node-book-field_document_relation'
  $field_instances['node-book-field_document_relation'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'This is assigned by the import module. When a book is saved, a new relation is generated and the resulting rid is saved here.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'number',
        'settings' => array(
          'decimal_separator' => '.',
          'prefix_suffix' => TRUE,
          'scale' => 0,
          'thousand_separator' => ' ',
        ),
        'type' => 'number_integer',
        'weight' => 21,
      ),
      'print' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 21,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_document_relation',
    'label' => 'Document relation',
    'required' => 0,
    'settings' => array(
      'max' => '',
      'min' => '',
      'prefix' => '',
      'suffix' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'number',
      'settings' => array(),
      'type' => 'number',
      'weight' => 13,
    ),
  );

  // Exported field_instance: 'node-book-field_document_type'
  $field_instances['node-book-field_document_type'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 10,
      ),
      'document_print_version' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 10,
      ),
      'print' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 10,
      ),
      'print_document' => array(
        'label' => 'above',
        'module' => 'hs_taxonomy',
        'settings' => array(),
        'type' => 'hs_taxonomy_term_reference_hierarchical_text',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'hidden',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_plain',
        'weight' => 10,
      ),
      'teaser' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 10,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_document_type',
    'label' => 'Document Type',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(
        'content_taxonomy_opt_groups' => FALSE,
      ),
      'type' => 'options_select',
      'weight' => 2,
    ),
  );

  // Exported field_instance: 'node-book-field_document_unique_id'
  $field_instances['node-book-field_document_unique_id'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 23,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_document_unique_id',
    'label' => 'Document Unique ID',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 10,
    ),
  );

  // Exported field_instance: 'node-book-field_internal'
  $field_instances['node-book-field_internal'] = array(
    'bundle' => 'book',
    'default_value' => array(
      0 => array(
        'value' => 0,
      ),
    ),
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 20,
      ),
      'print' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 20,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_internal',
    'label' => 'Internal Document',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_buttons',
      'weight' => 6,
    ),
  );

  // Exported field_instance: 'node-book-field_platform'
  $field_instances['node-book-field_platform'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'If this book applies to a specific platform, indicate that here.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 18,
      ),
      'document_print_version' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 18,
      ),
      'print' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 18,
      ),
      'print_document' => array(
        'label' => 'above',
        'module' => 'hs_taxonomy',
        'settings' => array(),
        'type' => 'hs_taxonomy_term_reference_hierarchical_text',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'above',
        'module' => 'hs_taxonomy',
        'settings' => array(),
        'type' => 'hs_taxonomy_term_reference_last_text_only',
        'weight' => 18,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_platform',
    'label' => 'Platform',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'hs_taxonomy',
      'settings' => array(),
      'type' => 'taxonomy_hs',
      'weight' => 4,
    ),
  );

  // Exported field_instance: 'node-book-field_products_manager'
  $field_instances['node-book-field_products_manager'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 17,
      ),
      'document_print_version' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 17,
      ),
      'print' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 17,
      ),
      'print_document' => array(
        'label' => 'above',
        'module' => 'hs_taxonomy',
        'settings' => array(),
        'type' => 'hs_taxonomy_term_reference_hierarchical_text',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 17,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_products_manager',
    'label' => 'Product Manager',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'hs_taxonomy',
      'settings' => array(
        'autocomplete_path' => 'taxonomy/autocomplete',
        'content_taxonomy_autocomplete_new_terms' => 'allow',
        'size' => 60,
      ),
      'type' => 'taxonomy_hs',
      'weight' => 8,
    ),
  );

  // Exported field_instance: 'node-book-field_products'
  $field_instances['node-book-field_products'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 22,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_products',
    'label' => 'Product Type',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'hs_taxonomy',
      'settings' => array(),
      'type' => 'taxonomy_hs',
      'weight' => 3,
    ),
  );

  // Exported field_instance: 'node-book-field_project_name'
  $field_instances['node-book-field_project_name'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 13,
      ),
      'document_print_version' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 13,
      ),
      'print' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 13,
      ),
      'print_document' => array(
        'label' => 'above',
        'module' => 'hs_taxonomy',
        'settings' => array(),
        'type' => 'hs_taxonomy_term_reference_hierarchical_text',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 13,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_project_name',
    'label' => 'Project Name',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'hs_taxonomy',
      'settings' => array(
        'autocomplete_path' => 'taxonomy/autocomplete',
        'content_taxonomy_autocomplete_new_terms' => 'allow',
        'size' => 60,
      ),
      'type' => 'taxonomy_hs',
      'weight' => 9,
    ),
  );

  // Exported field_instance: 'node-book-field_publish_date'
  $field_instances['node-book-field_publish_date'] = array(
    'bundle' => 'book',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'medium',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
        ),
        'type' => 'date_default',
        'weight' => -2,
      ),
      'document_print_version' => array(
        'label' => 'hidden',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => -2,
      ),
      'full' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'medium',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => -2,
      ),
      'mobile_tools' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'medium',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => -2,
      ),
      'print' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => -2,
      ),
      'print_document' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'long',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => 0,
      ),
      'rss' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'medium',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => -2,
      ),
      'search_index' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'medium',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => -2,
      ),
      'search_result' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => -2,
      ),
      'teaser' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'medium',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => -2,
      ),
      'token' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'medium',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => -2,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_publish_date',
    'label' => 'Publish Date',
    'required' => 0,
    'settings' => array(
      'default_value' => 'blank',
      'default_value2' => 'same',
      'default_value_code' => '',
      'default_value_code2' => '',
      'user_register_form' => FALSE,
    ),
    'weight' => -2,
    'widget' => array(
      'active' => 1,
      'module' => 'date',
      'settings' => array(
        'display_all_day' => 0,
        'increment' => 1,
        'input_format' => 'Y-m-d H:i:s',
        'input_format_custom' => '',
        'label_position' => 'above',
        'repeat_collapsed' => 0,
        'text_parts' => array(),
        'year_range' => '-3:+3',
      ),
      'type' => 'date_popup',
      'weight' => 5,
    ),
    'widget_type' => 'date_text',
  );

  // Exported field_instance: 'node-book-field_software_release'
  $field_instances['node-book-field_software_release'] = array(
    'bundle' => 'book',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 24,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_software_release',
    'label' => 'Software Release',
    'required' => 0,
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
      'weight' => 17,
    ),
  );

  // Exported field_instance: 'node-book-upload'
  $field_instances['node-book-upload'] = array(
    'bundle' => 'book',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_table',
        'weight' => 0,
      ),
      'document_print_version' => array(
        'label' => 'hidden',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'full' => array(
        'label' => 'hidden',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_table',
        'weight' => 0,
      ),
      'print' => array(
        'label' => 'hidden',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'print_document' => array(
        'label' => 'above',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 0,
      ),
      'rss' => array(
        'label' => 'hidden',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_table',
        'weight' => 0,
      ),
      'search_result' => array(
        'label' => 'hidden',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'hidden',
        'module' => NULL,
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'upload',
    'label' => 'File attachments',
    'required' => 0,
    'settings' => array(
      'description_field' => 1,
      'file_directory' => 'pdf-versions',
      'file_extensions' => 'jpg jpeg gif png eps txt doc xls pdf ppt pps cdr cpt ai psd dotx docx cdt rtf wav mpeg htm dotm',
      'max_filesize' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'insert' => 0,
        'insert_absolute' => 0,
        'insert_class' => '',
        'insert_default' => 'auto',
        'insert_styles' => array(
          'auto' => 'auto',
          'icon_link' => 0,
          'image' => 0,
          'image_asdf' => 0,
          'image_large' => 0,
          'image_medium' => 0,
          'image_thumbnail' => 0,
          'link' => 0,
        ),
        'insert_width' => '',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'file_generic',
      'weight' => 12,
    ),
  );

  // Exported field_instance: 'node-import_ticket-body'
  $field_instances['node-import_ticket-body'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Enter any notes related to this document here; for example, intended heading heading split level.',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 22,
      ),
      'full' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 17,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(
          'trim_length' => 600,
        ),
        'type' => 'text_summary_or_trimmed',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'body',
    'label' => 'Notes',
    'required' => 0,
    'settings' => array(
      'display_summary' => 0,
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 5,
        'summary_rows' => 5,
      ),
      'type' => 'text_textarea_with_summary',
      'weight' => 4,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_authoring_tool'
  $field_instances['node-import_ticket-field_authoring_tool'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 21,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_authoring_tool',
    'label' => 'Authoring Tool',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 20,
      ),
      'type' => 'text_textfield',
      'weight' => 9,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_auto_pdf'
  $field_instances['node-import_ticket-field_auto_pdf'] = array(
    'bundle' => 'import_ticket',
    'deleted' => 0,
    'description' => 'This field holds the PDF automatically generated by PDF Reactor. It should not normally be edited.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 47,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_auto_pdf',
    'label' => 'Auto PDF',
    'required' => 0,
    'settings' => array(
      'description_field' => 0,
      'file_directory' => 'import-source/pdf-reactor/[current-date:custom:Y-M]',
      'file_extensions' => 'pdf',
      'max_filesize' => '',
      'mediafront' => array(
        'custom' => '',
        'field_type' => 0,
        'media_type' => 'media',
        'preview' => 0,
        'thumbnail' => 0,
      ),
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'insert' => 0,
        'insert_absolute' => 0,
        'insert_class' => '',
        'insert_default' => 'auto',
        'insert_styles' => array(
          'auto' => 'auto',
          'icon_link' => 0,
          'image' => 0,
          'image_document_thumbnail' => 0,
          'image_large' => 0,
          'image_medium' => 0,
          'image_product_shot' => 0,
          'image_thumbnail' => 0,
          'image_watermark_100' => 0,
          'image_whitebranded_logo' => 0,
          'link' => 0,
        ),
        'insert_width' => '',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'file_generic',
      'weight' => 7,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_document_archive'
  $field_instances['node-import_ticket-field_document_archive'] = array(
    'bundle' => 'import_ticket',
    'deleted' => 0,
    'description' => '<p>Upload the zip archive of the document to be imported. The file must comply with the following:</p><ul><li>Must be a .zip file</li><li>Must be generated either by Word (filtered HTML), or Mif2Go  for FrameMaker documents.</li></ul><p>Make sure that <b>editdata.mso</b> is removed from Word archives before uploading.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 16,
      ),
      'full' => array(
        'label' => 'above',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 13,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_document_archive',
    'label' => 'Document Archive',
    'required' => 0,
    'settings' => array(
      'description_field' => 0,
      'file_directory' => 'imports/import-source-files/raw/[current-date:custom:Y-m-d]',
      'file_extensions' => 'zip',
      'max_filesize' => '',
      'mediafront' => array(
        'custom' => '',
        'field_type' => 0,
        'media_type' => 'media',
        'preview' => 0,
        'thumbnail' => 0,
      ),
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'insert' => 0,
        'insert_absolute' => 0,
        'insert_class' => '',
        'insert_default' => 'auto',
        'insert_styles' => array(
          'auto' => 'auto',
          'colorbox__cheap_dropshadow' => 0,
          'colorbox__corners_combo' => 0,
          'colorbox__default' => 0,
          'colorbox__document_thumbnail' => 0,
          'colorbox__keyword_positioning' => 0,
          'colorbox__large' => 0,
          'colorbox__medium' => 0,
          'colorbox__positioned_underlay' => 0,
          'colorbox__product_shot' => 0,
          'colorbox__relative_canvas' => 0,
          'colorbox__rotate_alpha' => 0,
          'colorbox__rotate_alpha_gif' => 0,
          'colorbox__rotate_scale' => 0,
          'colorbox__rotate_scale_alpha' => 0,
          'colorbox__rounded' => 0,
          'colorbox__rounded_bl' => 0,
          'colorbox__rounded_flattened' => 0,
          'colorbox__scale' => 0,
          'colorbox__scale_canvas' => 0,
          'colorbox__scale_rotate' => 0,
          'colorbox__thumbnail' => 0,
          'colorbox__watermark_50' => 0,
          'colorbox__watermark_100' => 0,
          'colorbox__whitebranded_logo' => 0,
          'icon_link' => 0,
          'image' => 0,
          'image_cheap_dropshadow' => 0,
          'image_corners_combo' => 0,
          'image_default' => 0,
          'image_document_thumbnail' => 0,
          'image_keyword_positioning' => 0,
          'image_large' => 0,
          'image_medium' => 0,
          'image_positioned_underlay' => 0,
          'image_product_shot' => 0,
          'image_relative_canvas' => 0,
          'image_rotate_alpha' => 0,
          'image_rotate_alpha_gif' => 0,
          'image_rotate_scale' => 0,
          'image_rotate_scale_alpha' => 0,
          'image_rounded' => 0,
          'image_rounded_bl' => 0,
          'image_rounded_flattened' => 0,
          'image_scale' => 0,
          'image_scale_canvas' => 0,
          'image_scale_rotate' => 0,
          'image_thumbnail' => 0,
          'image_watermark_50' => 0,
          'image_watermark_100' => 0,
          'image_whitebranded_logo' => 0,
          'link' => 0,
        ),
        'insert_width' => '',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'file_generic',
      'weight' => 14,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_document_author'
  $field_instances['node-import_ticket-field_document_author'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 12,
      ),
      'full' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 9,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_document_author',
    'label' => 'Document Author',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'taxonomy',
      'settings' => array(
        'autocomplete_path' => 'taxonomy/autocomplete',
        'content_taxonomy_autocomplete_new_terms' => 'allow',
        'size' => 60,
      ),
      'type' => 'taxonomy_autocomplete',
      'weight' => 47,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_document_type'
  $field_instances['node-import_ticket-field_document_type'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '<p>Starting from broad categories and working to narrow, tag this document type. For example, <b>User Documents</b> > <b>User\'s Guide</b>.</p>',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 6,
      ),
      'full' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 3,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_document_type',
    'label' => 'Document Type',
    'required' => 1,
    'settings' => array(
      'mediafront' => array(
        'custom' => '',
        'field_type' => 0,
        'media_type' => 'media',
        'preview' => 0,
        'thumbnail' => 0,
      ),
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'hs_taxonomy',
      'settings' => array(),
      'type' => 'taxonomy_hs',
      'weight' => 45,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_document_unique_id'
  $field_instances['node-import_ticket-field_document_unique_id'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Document Unique ID should be the unique identifier for the entire document (for example, a part number). Uniqueness is not enforced since several HTML subpages may share the same unique ID.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 51,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_document_unique_id',
    'label' => 'Document Unique ID',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 50,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_granularity'
  $field_instances['node-import_ticket-field_granularity'] = array(
    'bundle' => 'import_ticket',
    'default_value' => array(
      0 => array(
        'value' => 1,
      ),
    ),
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 54,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_granularity',
    'label' => 'Granularity',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_buttons',
      'weight' => -3,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_imported_html_master'
  $field_instances['node-import_ticket-field_imported_html_master'] = array(
    'bundle' => 'import_ticket',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 19,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_imported_html_master',
    'label' => 'Imported HTML master',
    'required' => 0,
    'settings' => array(
      'description_field' => 0,
      'file_directory' => 'imports/import-source-files/html-masters/[current-date:custom:Y-m-d]',
      'file_extensions' => 'htm html',
      'max_filesize' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'insert' => 0,
        'insert_absolute' => 0,
        'insert_class' => '',
        'insert_default' => 'auto',
        'insert_styles' => array(
          'auto' => 'auto',
          'colorbox__cheap_dropshadow' => 0,
          'colorbox__corners_combo' => 0,
          'colorbox__default' => 0,
          'colorbox__document_thumbnail' => 0,
          'colorbox__keyword_positioning' => 0,
          'colorbox__large' => 0,
          'colorbox__medium' => 0,
          'colorbox__positioned_underlay' => 0,
          'colorbox__relative_canvas' => 0,
          'colorbox__rotate_alpha' => 0,
          'colorbox__rotate_alpha_gif' => 0,
          'colorbox__rotate_scale' => 0,
          'colorbox__rotate_scale_alpha' => 0,
          'colorbox__rounded' => 0,
          'colorbox__rounded_bl' => 0,
          'colorbox__rounded_flattened' => 0,
          'colorbox__scale' => 0,
          'colorbox__scale_canvas' => 0,
          'colorbox__scale_rotate' => 0,
          'colorbox__thumbnail' => 0,
          'colorbox__watermark_50' => 0,
          'colorbox__watermark_100' => 0,
          'icon_link' => 0,
          'image' => 0,
          'image_cheap_dropshadow' => 0,
          'image_corners_combo' => 0,
          'image_default' => 0,
          'image_document_thumbnail' => 0,
          'image_keyword_positioning' => 0,
          'image_large' => 0,
          'image_medium' => 0,
          'image_positioned_underlay' => 0,
          'image_relative_canvas' => 0,
          'image_rotate_alpha' => 0,
          'image_rotate_alpha_gif' => 0,
          'image_rotate_scale' => 0,
          'image_rotate_scale_alpha' => 0,
          'image_rounded' => 0,
          'image_rounded_bl' => 0,
          'image_rounded_flattened' => 0,
          'image_scale' => 0,
          'image_scale_canvas' => 0,
          'image_scale_rotate' => 0,
          'image_thumbnail' => 0,
          'image_watermark_50' => 0,
          'image_watermark_100' => 0,
          'link' => 0,
        ),
        'insert_width' => '',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'file_generic',
      'weight' => 8,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_internal'
  $field_instances['node-import_ticket-field_internal'] = array(
    'bundle' => 'import_ticket',
    'default_value' => array(
      0 => array(
        'value' => 0,
      ),
    ),
    'deleted' => 0,
    'description' => 'Select <b>internal</b> if this document is not public-facing. Documents marked as internal will only be visible to logged-in users.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 37,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_internal',
    'label' => 'Internal Document',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(
        'display_label' => 0,
      ),
      'type' => 'options_onoff',
      'weight' => -4,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_manual_pdf'
  $field_instances['node-import_ticket-field_manual_pdf'] = array(
    'bundle' => 'import_ticket',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 52,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_manual_pdf',
    'label' => 'Manual PDF',
    'required' => 0,
    'settings' => array(
      'description_field' => 0,
      'file_directory' => 'resources/pdf/[current-date:custom:Y]/[current-date:custom:M]',
      'file_extensions' => 'pdf',
      'max_filesize' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'allowed_schemes' => array(
          'public' => 'public',
        ),
        'allowed_types' => array(
          'audio' => 0,
          'default' => 0,
          'image' => 'image',
          'video' => 0,
        ),
        'progress_indicator' => 'throbber',
      ),
      'type' => 'file_generic',
      'weight' => 15,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_platform'
  $field_instances['node-import_ticket-field_platform'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '<b>Only use this field if there are multiple platform versions for this product</b>. <br/><br/>If this document applies to specific platforms only (for example, Windows, Android, or iOS), specify that here. Only specify a sane depth here; unless your document only applies to Windows 7, you can simply specify Windows.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 8,
      ),
      'full' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 5,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_platform',
    'label' => 'Platform',
    'required' => 0,
    'settings' => array(
      'mediafront' => array(
        'custom' => '',
        'field_type' => 0,
        'media_type' => 'media',
        'preview' => 0,
        'thumbnail' => 0,
      ),
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'hs_taxonomy',
      'settings' => array(),
      'type' => 'taxonomy_hs',
      'weight' => 47,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_preprocessed_html'
  $field_instances['node-import_ticket-field_preprocessed_html'] = array(
    'bundle' => 'import_ticket',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 38,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_preprocessed_html',
    'label' => 'Preprocessed HTML',
    'required' => 0,
    'settings' => array(
      'description_field' => 0,
      'file_directory' => 'imports/import-source-files/html-preprocessed/[current-date:custom:Y-m-d]',
      'file_extensions' => 'bin',
      'max_filesize' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'insert' => 0,
        'insert_absolute' => 0,
        'insert_class' => '',
        'insert_default' => 'auto',
        'insert_styles' => array(
          'auto' => 'auto',
          'colorbox__cheap_dropshadow' => 0,
          'colorbox__corners_combo' => 0,
          'colorbox__default' => 0,
          'colorbox__document_thumbnail' => 0,
          'colorbox__keyword_positioning' => 0,
          'colorbox__large' => 0,
          'colorbox__medium' => 0,
          'colorbox__positioned_underlay' => 0,
          'colorbox__relative_canvas' => 0,
          'colorbox__rotate_alpha' => 0,
          'colorbox__rotate_alpha_gif' => 0,
          'colorbox__rotate_scale' => 0,
          'colorbox__rotate_scale_alpha' => 0,
          'colorbox__rounded' => 0,
          'colorbox__rounded_bl' => 0,
          'colorbox__rounded_flattened' => 0,
          'colorbox__scale' => 0,
          'colorbox__scale_canvas' => 0,
          'colorbox__scale_rotate' => 0,
          'colorbox__thumbnail' => 0,
          'colorbox__watermark_50' => 0,
          'colorbox__watermark_100' => 0,
          'icon_link' => 0,
          'image' => 0,
          'image_cheap_dropshadow' => 0,
          'image_corners_combo' => 0,
          'image_default' => 0,
          'image_document_thumbnail' => 0,
          'image_keyword_positioning' => 0,
          'image_large' => 0,
          'image_medium' => 0,
          'image_positioned_underlay' => 0,
          'image_relative_canvas' => 0,
          'image_rotate_alpha' => 0,
          'image_rotate_alpha_gif' => 0,
          'image_rotate_scale' => 0,
          'image_rotate_scale_alpha' => 0,
          'image_rounded' => 0,
          'image_rounded_bl' => 0,
          'image_rounded_flattened' => 0,
          'image_scale' => 0,
          'image_scale_canvas' => 0,
          'image_scale_rotate' => 0,
          'image_thumbnail' => 0,
          'image_watermark_50' => 0,
          'image_watermark_100' => 0,
          'link' => 0,
        ),
        'insert_width' => '',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'file_generic',
      'weight' => 7,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_processed_json'
  $field_instances['node-import_ticket-field_processed_json'] = array(
    'bundle' => 'import_ticket',
    'deleted' => 0,
    'description' => 'This is the parking space for a completely converted content array. This is created after all content processing has been finished. It stores an array of the spec: {Item: (1: Heading Level, 2: Title, 3: Content)} ',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 39,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_processed_json',
    'label' => 'Processed JSON',
    'required' => 0,
    'settings' => array(
      'description_field' => 0,
      'file_directory' => '',
      'file_extensions' => 'bin',
      'max_filesize' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'insert' => 0,
        'insert_absolute' => 0,
        'insert_class' => '',
        'insert_default' => 'auto',
        'insert_styles' => array(
          'auto' => 'auto',
          'colorbox__cheap_dropshadow' => 0,
          'colorbox__corners_combo' => 0,
          'colorbox__default' => 0,
          'colorbox__document_thumbnail' => 0,
          'colorbox__keyword_positioning' => 0,
          'colorbox__large' => 0,
          'colorbox__medium' => 0,
          'colorbox__positioned_underlay' => 0,
          'colorbox__relative_canvas' => 0,
          'colorbox__rotate_alpha' => 0,
          'colorbox__rotate_alpha_gif' => 0,
          'colorbox__rotate_scale' => 0,
          'colorbox__rotate_scale_alpha' => 0,
          'colorbox__rounded' => 0,
          'colorbox__rounded_bl' => 0,
          'colorbox__rounded_flattened' => 0,
          'colorbox__scale' => 0,
          'colorbox__scale_canvas' => 0,
          'colorbox__scale_rotate' => 0,
          'colorbox__thumbnail' => 0,
          'colorbox__watermark_50' => 0,
          'colorbox__watermark_100' => 0,
          'icon_link' => 0,
          'image' => 0,
          'image_cheap_dropshadow' => 0,
          'image_corners_combo' => 0,
          'image_default' => 0,
          'image_document_thumbnail' => 0,
          'image_keyword_positioning' => 0,
          'image_large' => 0,
          'image_medium' => 0,
          'image_positioned_underlay' => 0,
          'image_relative_canvas' => 0,
          'image_rotate_alpha' => 0,
          'image_rotate_alpha_gif' => 0,
          'image_rotate_scale' => 0,
          'image_rotate_scale_alpha' => 0,
          'image_rounded' => 0,
          'image_rounded_bl' => 0,
          'image_rounded_flattened' => 0,
          'image_scale' => 0,
          'image_scale_canvas' => 0,
          'image_scale_rotate' => 0,
          'image_thumbnail' => 0,
          'image_watermark_50' => 0,
          'image_watermark_100' => 0,
          'link' => 0,
        ),
        'insert_width' => '',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'file_generic',
      'weight' => 16,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_products_manager'
  $field_instances['node-import_ticket-field_products_manager'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 13,
      ),
      'full' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 10,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_products_manager',
    'label' => 'Product Manager',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'taxonomy',
      'settings' => array(
        'autocomplete_path' => 'taxonomy/autocomplete',
        'content_taxonomy_autocomplete_new_terms' => 'allow',
        'size' => 60,
      ),
      'type' => 'taxonomy_autocomplete',
      'weight' => 48,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_products'
  $field_instances['node-import_ticket-field_products'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 53,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_products',
    'label' => 'Product Type',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'hs_taxonomy',
      'settings' => array(),
      'type' => 'taxonomy_hs',
      'weight' => 46,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_project_name'
  $field_instances['node-import_ticket-field_project_name'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 14,
      ),
      'full' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 11,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_project_name',
    'label' => 'Project Name',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'taxonomy',
      'settings' => array(
        'autocomplete_path' => 'taxonomy/autocomplete',
        'content_taxonomy_autocomplete_new_terms' => 'allow',
        'size' => 60,
      ),
      'type' => 'taxonomy_autocomplete',
      'weight' => 49,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_publish_date'
  $field_instances['node-import_ticket-field_publish_date'] = array(
    'bundle' => 'import_ticket',
    'deleted' => 0,
    'description' => 'The publication date for this document (this will appear on the title page).<br/><b>If there is a GA date for this document, use that. This is normally displayed on the site as a month and year, without the day.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'long',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => 10,
      ),
      'full' => array(
        'label' => 'above',
        'module' => 'date',
        'settings' => array(
          'format_type' => 'long',
          'fromto' => 'both',
          'multiple_from' => '',
          'multiple_number' => '',
          'multiple_to' => '',
          'show_repeat_rule' => 'show',
        ),
        'type' => 'date_default',
        'weight' => 7,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_publish_date',
    'label' => 'Publish Date',
    'required' => 1,
    'settings' => array(
      'default_value' => 'now',
      'default_value2' => 'same',
      'default_value_code' => '',
      'default_value_code2' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'date',
      'settings' => array(
        'increment' => 15,
        'input_format' => 'Y-m-d H:i:s',
        'input_format_custom' => '',
        'label_position' => 'above',
        'text_parts' => array(),
        'year_range' => '-3:+3',
      ),
      'type' => 'date_popup',
      'weight' => 46,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_raw_html_master'
  $field_instances['node-import_ticket-field_raw_html_master'] = array(
    'bundle' => 'import_ticket',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 18,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_raw_html_master',
    'label' => 'Raw HTML master',
    'required' => 0,
    'settings' => array(
      'description_field' => 0,
      'file_directory' => 'imports/import-source-files/html-raw/[current-date:custom:Y-m-d]',
      'file_extensions' => 'html',
      'max_filesize' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'insert' => 0,
        'insert_absolute' => 0,
        'insert_class' => '',
        'insert_default' => 'auto',
        'insert_styles' => array(
          'auto' => 'auto',
          'colorbox__cheap_dropshadow' => 0,
          'colorbox__corners_combo' => 0,
          'colorbox__default' => 0,
          'colorbox__document_thumbnail' => 0,
          'colorbox__keyword_positioning' => 0,
          'colorbox__large' => 0,
          'colorbox__medium' => 0,
          'colorbox__positioned_underlay' => 0,
          'colorbox__relative_canvas' => 0,
          'colorbox__rotate_alpha' => 0,
          'colorbox__rotate_alpha_gif' => 0,
          'colorbox__rotate_scale' => 0,
          'colorbox__rotate_scale_alpha' => 0,
          'colorbox__rounded' => 0,
          'colorbox__rounded_bl' => 0,
          'colorbox__rounded_flattened' => 0,
          'colorbox__scale' => 0,
          'colorbox__scale_canvas' => 0,
          'colorbox__scale_rotate' => 0,
          'colorbox__thumbnail' => 0,
          'colorbox__watermark_50' => 0,
          'colorbox__watermark_100' => 0,
          'icon_link' => 0,
          'image' => 0,
          'image_cheap_dropshadow' => 0,
          'image_corners_combo' => 0,
          'image_default' => 0,
          'image_document_thumbnail' => 0,
          'image_keyword_positioning' => 0,
          'image_large' => 0,
          'image_medium' => 0,
          'image_positioned_underlay' => 0,
          'image_relative_canvas' => 0,
          'image_rotate_alpha' => 0,
          'image_rotate_alpha_gif' => 0,
          'image_rotate_scale' => 0,
          'image_rotate_scale_alpha' => 0,
          'image_rounded' => 0,
          'image_rounded_bl' => 0,
          'image_rounded_flattened' => 0,
          'image_scale' => 0,
          'image_scale_canvas' => 0,
          'image_scale_rotate' => 0,
          'image_thumbnail' => 0,
          'image_watermark_50' => 0,
          'image_watermark_100' => 0,
          'link' => 0,
        ),
        'insert_width' => '',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'file_generic',
      'weight' => 6,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_release_state'
  $field_instances['node-import_ticket-field_release_state'] = array(
    'bundle' => 'import_ticket',
    'default_value' => array(
      0 => array(
        'value' => 0,
      ),
    ),
    'deleted' => 0,
    'description' => 'This is normally <i>Stable</i>, indicating a GA release. You may also specify other pre-release states like <i>beta</i>. Documents that are tagged as pre-release versions will not appear in "related documents" lists, and are only accessible via direct link.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 48,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_release_state',
    'label' => 'Release Type',
    'required' => 0,
    'settings' => array(
      'mediafront' => array(
        'custom' => '',
        'field_type' => 0,
        'media_type' => 'media',
        'preview' => 0,
        'thumbnail' => 0,
      ),
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(
        'content_taxonomy_opt_groups' => FALSE,
      ),
      'type' => 'options_select',
      'weight' => 49,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_software_release'
  $field_instances['node-import_ticket-field_software_release'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 55,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'fences_wrapper' => 'div',
    'field_name' => 'field_software_release',
    'label' => 'Software Release',
    'required' => 0,
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
      'weight' => 48,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_toc_array'
  $field_instances['node-import_ticket-field_toc_array'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'This field is automatically filled by doc_import_postprocess.inc.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 42,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_toc_array',
    'label' => 'TOC array',
    'required' => 0,
    'settings' => array(
      'mediafront' => array(
        'custom' => '',
        'field_type' => 0,
        'media_type' => 'media',
        'preview' => 0,
        'thumbnail' => 0,
      ),
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 5,
      ),
      'type' => 'text_textarea',
      'weight' => 10,
    ),
  );

  // Exported field_instance: 'node-import_ticket-field_topic_used_in_'
  $field_instances['node-import_ticket-field_topic_used_in_'] = array(
    'bundle' => 'import_ticket',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 40,
      ),
      'print_document' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_topic_used_in_',
    'label' => 'Tracking these topics',
    'required' => 0,
    'settings' => array(
      'relation_type' => array(
        'uses_topic' => 'uses_topic',
      ),
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'relation_dummy_field',
      'settings' => array(),
      'type' => 'relation_default',
      'weight' => 13,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('<b>Only use this field if there are multiple platform versions for this product</b>. <br/><br/>If this document applies to specific platforms only (for example, Windows, Android, or iOS), specify that here. Only specify a sane depth here; unless your document only applies to Windows 7, you can simply specify Windows.');
  t('<p>
	A <em>book page</em> is a page of content, organized into a collection of related entries collectively known as a <em>book</em>. A <em>book page</em> automatically displays links to adjacent pages, providing a simple navigation system for organizing and reviewing structured content.</p>
');
  t('<p>Starting from broad categories and working to narrow, tag this document type. For example, <b>User Documents</b> > <b>User\'s Guide</b>.</p>');
  t('<p>Upload the zip archive of the document to be imported. The file must comply with the following:</p><ul><li>Must be a .zip file</li><li>Must be generated either by Word (filtered HTML), or Mif2Go  for FrameMaker documents.</li></ul><p>Make sure that <b>editdata.mso</b> is removed from Word archives before uploading.');
  t('Authoring Tool');
  t('Auto PDF');
  t('Body');
  t('Document Archive');
  t('Document Author');
  t('Document Type');
  t('Document Unique ID');
  t('Document Unique ID should be the unique identifier for the entire document (for example, a part number). Uniqueness is not enforced since several HTML subpages may share the same unique ID.');
  t('Document relation');
  t('Enter any notes related to this document here; for example, intended heading heading split level.');
  t('File attachments');
  t('Granularity');
  t('If this book applies to a specific platform, indicate that here.');
  t('Imported HTML master');
  t('Internal Document');
  t('Manual PDF');
  t('Notes');
  t('Platform');
  t('Preprocessed HTML');
  t('Processed JSON');
  t('Product Manager');
  t('Product Type');
  t('Project Name');
  t('Publish Date');
  t('Raw HTML master');
  t('Release Type');
  t('Select <b>internal</b> if this document is not public-facing. Documents marked as internal will only be visible to logged-in users.');
  t('Software Release');
  t('TOC array');
  t('The original author of this document');
  t('The publication date for this document (this will appear on the title page).<br/><b>If there is a GA date for this document, use that. This is normally displayed on the site as a month and year, without the day.');
  t('This field holds the PDF automatically generated by PDF Reactor. It should not normally be edited.');
  t('This field is automatically filled by doc_import_postprocess.inc.');
  t('This is assigned by the import module. When a book is saved, a new relation is generated and the resulting rid is saved here.');
  t('This is normally <i>Stable</i>, indicating a GA release. You may also specify other pre-release states like <i>beta</i>. Documents that are tagged as pre-release versions will not appear in "related documents" lists, and are only accessible via direct link.');
  t('This is the parking space for a completely converted content array. This is created after all content processing has been finished. It stores an array of the spec: {Item: (1: Heading Level, 2: Title, 3: Content)} ');
  t('Tracking these topics');

  return $field_instances;
}
