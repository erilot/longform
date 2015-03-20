<?php

/**
 * Display and control main ticket nav
 */
function doc_manage_render_main_nav($ticket = NULL, $opts = NULL) {

  /**
   * Panes included by default:
   *
   * Import:  Contains the entire import-review-publish cycle, using vertical tabs. The title of this tab changes
   *   to display the current task, so it will be one of "Import", "Review", or "Publish.
   *
   * Document:  This is the administrative document display. Contains a link to the cover page, a copy of the TOC,
   *   and status indicators. This pane is hidden until the import process is complete.
   *
   * Metadata:  Displays listing of document metadata. In the future, this will include options for modifying existing documents.
   *
   * Links:  Displays the link information for the current document.
   */

  global $user;

  if (!user_is_logged_in()) {
    return;
  }
  $ticket = longform_entities_get_entity();
    $ticket_state = longform_get_current_workflow_state($ticket);
    $import_state = $ticket_state->name;
    $published = $ticket_state->name == 'published';

  // Define panes that will be included
  $panes = array(
    'import' => array(
      'id' => 'tab-import',
      'label' => ($published ? 'Publication Tasks' : $import_state),
      'function' => 'doc_manage_render_workflow'
    ),
//    'document' => array(
//      'id' => 'tab-document',
//      'label' => 'Document',
//      'function' => 'doc_manage_render_document_pane'
//    ),
//    'metadata' => array(
//      'id' => 'tab-metadata',
//      'label' => 'Metadata',
//      'function' => 'doc_manage_metadata_table'
//    ),
//    'links' => array(
//      'id' => 'tab-links',
//      'label' => 'Links',
//      'function' => 'doc_manage_display_adaptive_links'
//    ),
//    'pdf-reactor' => array(
//      'id' => 'tab-pdf-reactor',
//      'label' => 'PDF Reactor',
//      'function' => 'doc_manage_render_pdf_options'
//    ),
  );

  // Remove pre-import panes
  if (!in_array($import_state, array('published', 'in_review'))) {
    unset($panes['document']);
  }

  // Add placeholders in the render array for the navbar and the tab content
  $bar['bar'] = array();
  $bar['container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('tab-content'),
    ),
  );

  // Add individual panes
  foreach ($panes as $key => $pane) {
    $pair = _get_tab_pair($pane['id'], FALSE, 'tab', $pane['label']);
    $tabs[$key] = $pair['tab'];

    $bar['container'][$key] = _render_workflow_tab_wrapper($pane['id'], FALSE, 'tab');
    $bar['container'][$key]['tab-' . $key]['content'] = $pane['function']($ticket);
  }
  // Render tabs
  $bar['bar'] = array(
    '#theme' => 'item_list',
    '#items' => $tabs,
    '#type' => 'ul',
    '#attributes' => array(
      'class' => array('nav', 'nav-tabs'),
    ),
  );

  // Set active pane
  // If the document is published, default to "Document" tab.

  if ($published) {
    $bar['bar']['#items']['document']['class'] = array('active');
    $bar['container']['document']['tab-document']['#attributes']['class'][] = 'in active';
    $bar['bar']['#items']['import']['class'] = array('pull-right');
  }
  else {
    $bar['bar']['#items']['import']['class'] = array('active');
    $bar['container']['import']['tab-import']['#attributes']['class'][] = 'in active';
  }

  return render($bar);

}


/**
 * Block display of document metadata. Just looks nicer than a panels layout
 * version.
 */
function doc_manage_metadata_table($ticket = NULL, $options = NULL) {

  if (!$ticket) {
    $ticket = menu_get_object();
  }

  $opts = array(
    'display_links' => TRUE,
  );

  global $base_path;
  global $user;


  // *** Build render array ***

  $wrap = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12'),
    ),
  );

  $wrap['title'] = array(
    '#markup' => '<h2>Document Metadata</h2>',
  );

  $wrap['body'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('row'),
    ),
  );

  $form['intro'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('alert', 'alert-info'),
    ),
  );

  $form['intro']['text'] = array(
    '#markup' => '<label>The metadata fields for this document are listed here.</label>
    <p>Note: Not all values are required. If a required value is missing, its row will be red. If a missing value isn\'t absolutely required but is typically used, its row will be yellow.</p>'
  );

  // Build field render array

  // Set up fields. Building these as arrays for future expansion -- it may be possible to auto-generate these.
  $fields[] = array(
    'label' => t('Document type'),
    'type' => 'term',
    'machine_name' => 'field_document_type',
    'missing_tooltip' => NULL,
    'present_tooltip' => NULL,
    'importance' => 'required',
    'editable' => TRUE,
  );
  $fields[] = array(
    'label' => t('Product(s)'),
    'type' => 'term',
    'machine_name' => 'field_products',
    'missing_tooltip' => 'Documents must be tagged with at least one product to be published',
    'present_tooltip' => NULL,
    'importance' => 'required',
    'editable' => TRUE,
  );
  $fields[] = array(
    'label' => t('Platform'),
    'type' => 'term',
    'machine_name' => 'field_platform',
    'missing_tooltip' => 'This should only be used if there are multiple platform versions (like iOS and Android)',
    'present_tooltip' => 'This should only be used if there are multiple platform versions (like iOS and Android)',
    'importance' => 'discouraged',
    'editable' => TRUE,
  );
  $fields[] = array(
    'label' => t('Publish date'),
    'type' => 'date',
    'machine_name' => 'field_publish_date',
    'missing_tooltip' => NULL,
    'present_tooltip' => NULL,
    'importance' => 'required',
    'editable' => TRUE,
  );
  $fields[] = array(
    'label' => t('Release version(s)'),
    'type' => 'term',
    'machine_name' => 'field_software_release',
    'missing_tooltip' => 'The release (or releases) this applies to; should almost never be left blank',
    'present_tooltip' => NULL,
    'importance' => 'recommended',
    'editable' => TRUE,
  );
  $fields[] = array(
    'label' => t('Part number'),
    'type' => 'term',
    'machine_name' => 'field_document_unique_id',
    'missing_tooltip' => 'Part numbers are not required by Technet, but are almost always required by definio',
    'present_tooltip' => NULL,
    'importance' => 'recommended',
    'editable' => TRUE,
  );
  $fields[] = array(
    'label' => t('Authoring tool (detected)'),
    'type' => 'string',
    'machine_name' => 'field_authoring_tool',
    'missing_tooltip' => 'The authoring tool is detected automatically during unpacking',
    'present_tooltip' => 'The authoring tool is detected automatically during unpacking',
    'importance' => 'required',
    'editable' => FALSE,
  );
  $fields[] = array(
    'label' => t('Document author(s)'),
    'type' => 'term',
    'machine_name' => 'field_document_author',
    'missing_tooltip' => NULL,
    'present_tooltip' => NULL,
    'importance' => 'required',
    'editable' => TRUE,
  );
  $fields[] = array(
    'label' => t('Assigned to'),
    'type' => 'entity reference',
    'machine_name' => 'field_assigned_to',
    'missing_tooltip' => 'The user responsible for publishing this document',
    'present_tooltip' => 'The user responsible for publishing this document',
    'importance' => 'required',
    'editable' => TRUE,
  );

  foreach ($fields as $field) {
    $field_rows[] = _doc_manage_build_report_row($ticket, $field);
  }

  $form['meta'] = array(
    '#theme' => 'table',
    '#attributes' => array(
      'class' => array('table'),
    ),
    //'#header' => array(t('Field'),t('Value')),
    '#rows' => $field_rows,
  );

  $wrap['body']['left'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-6'),
    ),
  );

  if ($options['meta_only']) {
    return $form['meta'];
  }

  $wrap['body']['left']['content'] = $form['meta'];


  // *** Files! ***

  $files[] = array(
    'label' => t('Document archive'),
    'type' => 'file',
    'machine_name' => 'field_document_source',
    'missing_tooltip' => 'The archive used to create HTML pages',
    'present_tooltip' => 'The archive used to create HTML pages',
    'importance' => 'required',
    'editable' => TRUE, // @TODO: make this variable based on unpack state
  );
  $files[] = array(
    'label' => t('Manual PDF'),
    'type' => 'file',
    'machine_name' => 'upload',
    'missing_tooltip' => 'A PDF you upload to Technet',
    'present_tooltip' => 'A PDF you upload to Technet',
    'importance' => 'required',
    'editable' => TRUE, // @TODO: make this variable based on unpack state
  );
  $files[] = array(
    'label' => t('Auto PDF'),
    'type' => 'file',
    'machine_name' => 'field_auto_pdf',
    'missing_tooltip' => 'A PDF generated automatically by Technet',
    'present_tooltip' => 'A PDF generated automatically by Technet',
    'importance' => 'required',
    'editable' => TRUE, // @TODO: make this variable based on unpack state
  );

  // Build file render array

  $form['file_head'] = array(
    '#markup' => '<h3 class="text-info">Files</h3>',
  );

  foreach ($files as $file) {
    $file_rows[] = _doc_manage_build_report_row($ticket, $file);
  }

  $form['files'] = array(
    '#theme' => 'table',
    '#attributes' => array(
      'class' => array('table'),
    ),
    //'#header' => array(t('Field'),t('Value')),
    '#rows' => $file_rows,
  );

  $wrap['body']['right'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-6'),
    ),
  );

  $wrap['body']['right']['content'] = $form['files'];

  return $wrap;
}

/**
 * Utility function that builds a table row for the render report.
 *
 * @param $wrapper
 *  The entity metadata wrapper object for this import ticket.
 *
 * @param $options
 *  An array of variables
 *
 * @return: A table row render array.
 *
 */
function _doc_manage_build_report_row($ticket, $vars) {

  global $base_path;

  $no_value = '<span class="text-danger">&mdash;</span>';

  $wrapper = entity_metadata_wrapper('node', $ticket);

  $label = $vars['label'];
  $field_machine_name = $vars['machine_name'];
  $missing_tooltip = $vars['missing_tooltip'];
  $present_tooltip = $vars['present_tooltip'];
  $type = $vars['type'];
  $required = $vars['importance'];
  $editable = $vars['editable'];
  // First, make sure the object is set (nasty crashy errros if it's not).
  if (isset($wrapper->$field_machine_name)) {
    $field = $wrapper->$field_machine_name->value();
  }


  //dpm($field_machine_name,'machine name');
  // If it's not set, stripe "none" value and return.
  if (!isset($field) || (is_array($field) && count($field) == 0)) {
    switch ($required) {
      case 'required':
        $row_class = 'danger';
        break;

      case 'recommended':
        $row_class = 'warning';
        break;
    }

    $value = $no_value;
    $tooltip = $missing_tooltip ? $missing_tooltip : NULL;
  }
  else {

    if ($required == 'discouraged') {
      $row_class = 'warning';
    }

    // When one value is given, return it by itself (no list).

    if (!is_array($field) || $type == 'file' || ($type != 'file' && count($field) == 1)) {
      if (is_array($field) && $type != 'file') {
        $field = reset($field);
      }
      else {

        // Special case: PDF uploads were initially configured as a generic file upload and allowed multiples. This is no longer
        // accurate, but the machine names and configurations have already been set in stone.
        // If this field is the 'upload' field, reset it to get down to the actual file array.
        if ($field_machine_name == 'upload') {
          $field = reset($field);
        }
      }
      $opts = array();
      $opts['display_links'] = FALSE;

      // Set the return value based on the incoming data type. For now, this is manually declared.
      switch ($type) {
        case 'string':
          $value = $field;
          break;

        case 'term':
          $path = isset($field->tid) ? taxonomy_term_uri($field) : NULL;
          $value = ($opts['display_links'] && isset($path)) ? l($field->name, $path['path']) : $field->name;
          break;

        case 'date':
          $value = date('M Y', $field);
          break;

        case 'file':
          $value = l('<span class="glyphicon glyphicon-download"></span> ' . str_ireplace('.' . array_pop(explode('/', $field['filemime'])), '', $field['filename']), file_create_url($field['uri']), array(
              'html' => TRUE,
              'attributes' => array(
                'class' => array(
                  'btn',
                  'btn-sm',
                  'btn-default',
                  'btn-block'
                )
              )
            ));
          break;

        case 'entity reference':
          $assignee = og_entity_getter($field, array(), 'entity')->value();
          $value = l($assignee->name, $base_path . 'user/' . $assignee->uid);
          break;
      }
      $tooltip = $present_tooltip ? $present_tooltip : NULL;
    }
    else {
      // When more than one value is given, render as an unordered list.

      foreach ($field as $item) {
        if (isset($item->tid)) {
          $path = taxonomy_term_uri($item);
        }

        // Set the return value based on the incoming data type. For now, this is manually declared.
        switch ($type) {
          case 'string':
            $list_items[] = $item;
            break;

          case 'term':
            $list_items[] = ($opts['display_links'] && isset($path)) ? l($item->name, $path['path']) : $item->name;
            break;

          case 'date':
            $list_items[] = date('M Y', $item);
            break;

        }
        $value = theme_item_list(array(
            'items' => $list_items,
            'title' => NULL,
            'type' => 'ul',
            'attributes' => array()
          ));
        $tooltip = $present_tooltip ? $present_tooltip : NULL;
      }
    }
  }

  // Base row render array
  $row = array(
    'data' => array('<b>' . $label . '</b>', $value),
    'class' => isset($row_class) ? array($row_class) : NULL,
  );


  // If tooltip is present, tack on the bootstrap options
  if ($tooltip) {
    $row = array_merge($row, array(
      'data-toggle' => 'tooltip',
      'data-placement' => 'right auto',
      'data-html' => 'true',
      'title' => $tooltip,
    ));
  }

  return $row;
}

/**
 * Implements hook_forms().
 *
 * Allows the workflow tab form to be repeated multiple times on a page.
 * See http://drupal.org/node/1970846.
 */
function doc_manage_forms($form_id, $args) {
  $forms = array();
  if (stripos($form_id, 'doc_manage_generic_field_form_') !== FALSE) {
    $forms[$form_id] = array('callback' => 'doc_manage_generic_field_form');
  }

  if (stripos($form_id, 'doc_manage_fetch_field_form_') !== FALSE) {
    $forms[$form_id] = array('callback' => 'doc_manage_fetch_field_form');
  }

  return $forms;

}

/**
 * AJAX load the specified field form.
 *
 */
function doc_manage_fetch_field_form($form, &$form_state, $field_machine_name, $ticket) {

  $form['wrap'] = array(
    '#type' => 'container',
    '#id' => 'field-form-' . $field_machine_name,
    '#attributes' => array(
      'class' => array('ajax-fetch', 'field-type-' . $field_machine_name),
    ),
  );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Edit'),
    '#ajax' => array(
      'callback' => array('doc_manage_fetch_field_return'),
      'wrapper' => ''
    ),
    '#attributes' => array(
      'class' => array('btn', 'btn-sm', 'btn-default'),
    ),
  );

  return $form;
}

function doc_manage_fetch_field_return($form, &$form_state) {
  // Generate the form required
  dpm($form_state, 'form_state');

  return $form;
}


/**
 * Generate form for specified field.
 *
 */
function doc_manage_generic_field_form($form, &$form_state, $field_machine_name, $ticket) {

  if ($form_state['sandbox']['node']) {
    $ticket = $form_state['sandbox']['node'];
  }
  else {
    $ticket = menu_get_object();
    $form_state['sandbox']['node'] = $ticket;
  }

  $form = field_default_form(
    'node',
    $ticket,
    field_info_field($field_machine_name),
    field_info_instance('node', $field_machine_name, $ticket->type),
    LANGUAGE_NONE,
    field_get_items('node', $ticket, $field_machine_name),
    $form,
    $form_state);

  // Remove description
  //unset ($form['field_document_source'][LANGUAGE_NONE][0]['#description']);

  // Add required includes
  if (empty($form_state['build_info']['files']) || !in_array('modules/node/node.pages.inc', $form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'node', 'node.pages');
    form_load_include($form_state, 'inc', 'doc_manage', 'doc_manage');
  }

  $form['#id'] = 'form-id-' . $field_machine_name;

  $form[$field_machine_name]['ticket_nid'] = array(
    '#type' => 'hidden',
    '#type' => 'input-nid-' . $ticket->nid,
    '#value' => $ticket->nid,
  );
  $form[$field_machine_name]['field_machine_name'] = array(
    '#type' => 'hidden',
    '#name' => 'input-field-' . $field_machine_name,
    '#value' => $field_machine_name,
  );
  $form[$field_machine_name]['submit'] = array(
    '#type' => 'submit',
    '#name' => 'submit-' . $field_machine_name,
    //'#ajax' => array(
    //  'callback' => '_doc_manage_upload_archive_submit'
    //  ),
    '#submit' => array('_doc_manage_generic_field_submit'),
    '#value' => 'Apply',
    '#attributes' => array(//'class' => array('invisible',)
    ),
  );

  //$form[$field_machine_name]['description'] = array(
  //  '#type' => 'markup',
  //  '#markup' => 'Upload (or remove) the prepared archive.',
  //  '#prefix' => '<br/><br/><div class="alert alert-info">',
  //  '#suffix' => '</div>',
  //);
  return $form;
}

function _doc_manage_generic_field_submit($form, &$form_state, $ticket = NULL, $field_machine_name = NULL) {
  dpm($form_state, 'form_state');
  $field = reset(reset($form_state['field']));
  $field_machine_name = $field['field']['field_name'];

  //dpm($field,'field');
  dpm($field_machine_name, 'field machine name');
  global $_SERVER;
  dpm($_POST, 'post');

  return $form;

  $new_fid = $form_state['values']['field_document_source'][LANGUAGE_NONE][0]['fid'];

  $node = $form_state['sandbox']['node'];

  $wrapper = entity_metadata_wrapper('node', $node);

  $original_file = $wrapper->field_document_source;
  $file = $original_file->value();
  $old_fid = $file['fid'];

  if ($new_fid == 0 & !isset($file)) {
    drupal_set_message('No file was uploaded.', 'error');

    return FALSE;
  }

  if ($new_fid == 0 && isset($file)) {

    // Remove the file.
    file_delete(file_load($old_fid));
    $wrapper->field_document_source->set(NULL);
    $wrapper->save(TRUE);
    field_attach_update('node', $node);
    $message .= '<li>The existing file attachment has been deleted.</li>';

  }
  else {

    // A new file is here!

    if ($old_fid != 0) {
      // First, remove the old one
      $wrapper->field_document_source->set(NULL);
      file_delete(file_load($old_fid));
      $wrapper->save(TRUE);
      field_attach_update('node', $node);
      $message .= '<li>The existing file attachment has been deleted.</li>';
    }

    // Add/save the new one

    // First, set the new file entry to permanent and indicate we're using it:
    $new_file = file_load($new_fid);
    $new_file->status = FILE_STATUS_PERMANENT;
    file_save($new_file);
    file_usage_add($new_file, 'doc_manage', 'node', $node->nid);
    $new_file_array = array('fid' => $new_fid, 'display' => TRUE);
    // Now add the file to the archive field
    $wrapper->field_document_source->set($new_file_array);
    $wrapper->save(TRUE);
    field_attach_update('node', $node);
    $message .= '<li>The new file attachment has been saved.</li>';

  }

  return $form;
}


/**
 * Block display of various adaptive and direct links to book content
 * Can only be called from an import_ticket page!
 *
 */
function doc_manage_display_adaptive_links($ticket) {

  $ticket = menu_get_object();
  $mode = 'in';
  $title = 'Document Links';
  $target = 'document-links';
  $id = $target;

  if ($ticket->type != 'import_ticket' || !module_exists('doc_redirect')) {
    return FALSE;
  }

  $links = longform_redirect_get_adaptive_permalink();

  // Build release-specific link
  if ($links['override']) {
    if ($ticket->field_products && $ticket->field_document_type) {
      $specific = '<p class="text-success">' . $links['override'] . '</p>';
    }
    else {
      $specific = '<p class="text-warning">A release-specific link will be available once the <b>document type</b> and <b>product type</b> have been assigned.</p>';
    }
  }
  else {
    $specific = '<p class="text-warning">This document has not been tagged with a <b>software version</b>, so no release-specific link is available.</p>';
  }

  // Build adaptive link
  $adaptive = ($ticket->field_products && $ticket->field_document_type) ? '<p class="text-success">' . $links['adaptive'] . '</p>' : '<p class="text-warning">An adaptive link will be available once the <b>document type</b> and <b>product type</b> have been assigned.</p>';

  // Build canonical link
  $canonical = ($links['cover']) ? '<p class="text-success">' . $links['cover'] . '</p>' : '<p class="text-warning">A canonical link to the cover page will be available once the archive has been imported, converted, and saved.</p>';

  $table = array(
    '#theme' => 'table',
    '#header' => array('Type', 'Link'),
    '#rows' => array(
      array(
        'data' => array('<b>Release-Specific</b>', $specific),
        'data-toggle' => 'tooltip',
        'data-placement' => 'auto',
        'data-html' => 'TRUE',
        'title' => t('The <b>release-specific</b> link should be used for support, unless it is unavailable'),
      ),
      array(
        'data' => array('<b>Canonical</b>', $canonical),
        'data-toggle' => 'tooltip',
        'data-placement' => 'auto',
        'data-html' => 'TRUE',
        'title' => t('Use the <b>Canonical</b> link if there is no release-specific link available'),
      ),
      array(
        'data' => array('<b>Adaptive</b>', $adaptive),
        'data-toggle' => 'tooltip',
        'data-placement' => 'auto',
        'data-html' => 'TRUE',
        'title' => t('This link is dynamic, and will only point to this document as long as it is current'),
      ),
    ),
    '#attributes' => array(
      'class' => array('table', 'table-hover'),
    ),
  );

  $render = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12'),
      'id' => 'adaptive-links',
    ),
  );

  if (!isset($collapsible)) {
    $render['head'] = array(
      '#markup' => '<h2>' . t($title) . '</h2>',
    );
  }

  $render['panel'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel', 'panel-default'),
    ),
  );

  // If in collapsing mode, render this as a triggering link.

  if (isset($collapsible)) {
    // Create heading link element. This will become the panel collapse trigger.
    $link_attrs = array(
      'html' => TRUE,
      'fragment' => $id,
      'external' => TRUE,
      'attributes' => array(
        'data-toggle' => 'collapse',
        'data-target' => '.' . $target,
        'class' => array('btn', 'btn-lg', 'btn-info', 'justify', $mode),
      ),
    );
    $heading = l('<span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' . t($title), '#', $link_attrs);

    $render['panel']['head'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('panel-heading'),
      ),
    );

    $render['panel']['head']['title'] = array(
      '#markup' => $heading,
    );
  }
  else {
  }


  $render['panel']['body'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('document-links', 'panel-body', $mode),
    ),
  );
  $render['panel']['body']['inner'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('inner'),
    ),
  );

  $render['panel']['body']['inner']['table'] = $table;

  return $render;
}

/**
 * Build and render importing workflow panels, with associated forms.
 *
 * Renders as tabbed nav panels.
 *
 */
function doc_manage_render_workflow($ticket) {

  $state = longform_get_current_workflow_state($ticket);

  // Build HTML render arrays

  $tabs = array();

  $out['head'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12'),
    ),
  );
  $out['head']['title'] = array(
    '#markup' => '<h2>Import, Review, and Publish</h2>',
  );
  if ($state->name == 'published') {
    $out['head']['alert'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('alert', 'alert-success'),
      ),
    );

    $out['head']['alert']['content'] = array(
      '#markup' => 'You\'re all done here! If you need to <b>unpublish</b> this document, click on the <b>unpublish</b> button below.',
    );
  }

  $out['bar'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-2'),
    ),
  );

  $out['contents'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('tab-content', 'col-md-10', 'left-rule'),
    ),
  );

  $import_tab_pair = _get_tab_pair('tab-task-import', _doc_manage_current_import_tab($state) == 'import', 'pill', 'Import');
  $tabs['tab-task-import'] = $import_tab_pair['tab'];
  $out['contents']['import'] = $import_tab_pair['wrapper'];
  $out['contents']['import']['tab-task-import']['contents'] = _doc_manage_render_import_panel($ticket, $state);

  $review_tab_pair = _get_tab_pair('tab-task-review', _doc_manage_current_import_tab($state) == 'review', 'pill', 'Review');
  $tabs['tab-task-review'] = $review_tab_pair['tab'];
  $out['contents']['review'] = $review_tab_pair['wrapper'];
  $out['contents']['review']['tab-task-review']['contents'] = _doc_manage_render_review_panel($ticket);

//  $publish_tab_pair = _get_tab_pair('tab-task-publish', _doc_manage_current_import_tab($state) == 'publish', 'pill', 'Publish');
//  $tabs['tab-task-publish'] = $publish_tab_pair['tab'];
//  $out['contents']['publish'] = $publish_tab_pair['wrapper'];
//  $out['contents']['publish']['tab-task-publish']['contents'] = _doc_manage_render_publish_panel($ticket, $states, $counts, $ready_to_publish, $published);
//

  // Fill in tabs when we're done
  $out['bar']['panel']['body']['tabs'] = array(
    '#theme' => 'item_list',
    '#items' => $tabs,
    '#type' => 'ul',
    '#attributes' => array(
      'class' => array('nav', 'nav-pills', 'nav-stacked'),
    ),
  );

  // Find the current task and add the "active" class to its list element
//  $active = strtolower(array_search(TRUE, _doc_manage_get_all_accordion_states($ticket)));

  $out['bar']['panel']['body']['tabs']['#items']['tab-task-' . _doc_manage_current_import_tab($state)]['class'] = array('active');

  return $out;

}

/**
 *
 * Helper function that sets the active import tab.
 *
 * @param $state
 *  The loaded workflow state object
 *
 * @return string
 *  The active pane
 */
function _doc_manage_current_import_tab($state) {

  switch ($state->name) {
    case 'unpack':
    case 'import':
    case 'convert':
    case 'output':
    case 'postprocess':
      $tab = 'import';
      break;

    case 'review':
      $tab = 'review';
      break;

    case 'approve':
      $tab = 'publish';
      break;

    default:
      $tab = 'import';
  }
  return $tab;
}


/**
 * Helper function that provides a matched tab and pane element.
 *
 * @param $id
 *  The CSS ID to use. Do not include the '#'.
 *
 * @param $state
 *  Boolean. Whether or not the pair should start in/active or closed/inactive.
 *
 * @param $type
 *   One of "pill" or "tab".
 *
 * @param $label
 *  The text label for the tab.
 *
 * @return matched pair as a keyed array
 *
 */
function _get_tab_pair($id, $state, $type, $label){

  return array(
    'wrapper' => _render_workflow_tab_wrapper($id, $state, $type),
    'tab' => array('data' => '<a href="#' . $id . '" data-id="' . $id . '" data-toggle="' . $type . '">' . $label . '</a>'),
  );
}



function _doc_manage_render_publish_panel($ticket, $states, $counts, $ready_to_publish, $published) {
  // ***Publish panel***

  $publish_panel['panel']['row']['top'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-xs-12', 'head-col'),
    ),
  );
  // Head region
  $publish_panel['panel']['row']['top']['head'] = array(
    '#markup' => '<h3>Publish</h3>',
  );

  switch ($ready_to_publish) {

    case TRUE:
      // Set up region wrappers

      $publish_panel['panel']['row']['left'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('col-md-8', 'right-col'),
        ),
      );

      $publish_panel['panel']['row']['right'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('col-md-4', 'left-col'),
        ),
      );

      // Set up warnings for incomplete information

      $warns = _doc_manage_metadata_warnings($ticket);

      if ($warns) {
        $warnings = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('alert', 'alert-warning')
          ),
        );
        $warnings['messages'] = array(
          '#markup' => '<label>Before publishing, review the following warnings:</label><ul>' . implode(PHP_EOL, $warns) . '</ul><br/><p>To review all document metadata, click the <b>Metadata</b> tab, above.</p>',
        );


      }
      else {
        $warnings = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('alert', 'alert-info'),
          ),
        );
        $warnings['panel']['row']['left']['warnings']['all_ok'] = array(
          '#markup' => '<p>Review the metadata below and click <b>publish</b> when you\'re ready.</p>',
        );
      }
      $publish_panel['panel']['row']['left']['warnings'] = $warnings;

      //$publish_panel['panel']['row']['left']['status'] = doc_manage_metadata_table($ticket, array('meta_only' => TRUE));

      $publish_panel['panel']['row']['right']['button'] = drupal_get_form('doc_manage_publish_button');


      // Right region
      $pdf_mode = ' 
	  <div class="alert"><label>PDF Mode</label><br/>' . flag_create_link('pdf_reactor_mode', $ticket->nid) . '<div class="alert text-muted"><p>If enabled, Technet will generate a PDF of this document automatically. Click to toggle.</p></div>' . '</div>';

      $pdf_sync = '<div class="alert"><label>PDF Sync Mode</label><br/>' . flag_create_link('pdf_reactor_live_sync', $ticket->nid) . '<div class="alert text-muted">If enabled, Technet will automatically re-generate PDFs when document content is changed. Click to toggle.<br/> <i class="text-warning">This setting has no effect if <b>pdf mode</b> is off.</i></div>' . '</div>';

      //$publish_panel['center'] ='<div class="col-xs-3 center-col">' . drupal_render(_doc_manage_render_pdf_options ($ticket)) .  ' </div>';


      break;

    default:
      // Not ready to publish yet!

      // Set up region wrapper
      $publish_panel['panel']['row']['msg'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('col-xs-12', 'col-md-8'),
        ),
      );
      $publish_panel['panel']['row']['msg']['content'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('alert', 'alert-warning'),
        ),
      );
      $publish_panel['panel']['row']['msg']['content']['text'] = array(
        '#markup' => '
	  <label>There\'s nothing to publish now.</label>
	  <p>Complete the importing and review steps above first &ndash; those processes create content that you will publish here.</p>'
      );

      break;
  }

  return $publish_panel;
}

/**
 * Helper function that returns an array of HTML list elements with metadata
 * warnings.
 *
 * @var $ticket
 *  The ticket object to check.
 *
 * @return array
 *  An array of <li> elements with relevant warnings.
 */
function _doc_manage_metadata_warnings($ticket) {
  $wrapper = entity_metadata_wrapper('node', $ticket);

  if ((!isset($wrapper->field_auto_pdf) || count($wrapper->field_auto_pdf->value()) == 0) && (!isset($wrapper->field_manual_pdf) || count($wrapper->field_manual_pdf->value()) == 0)) {
    $warns[] = '<li>' . t('<b>No PDFs have been uploaded</b>, and PDF Reactor is not set to automatically generate one.') . '</li>';
  }
  //dpm($wrapper->field_software_release->value(),'value test');

  if (!isset($wrapper->field_software_release) || count($wrapper->field_software_release->value()) == 0) {
    $warns[] = '<li>' . t('<b>No software release</b> has been specified.') . '</li>';
  }
  if (!isset($wrapper->field_products) || count($wrapper->field_products->value()) == 0) {
    $warns[] = '<li>' . t('<b>No product/device type</b> has been specified.') . '</li>';
  }
  if (!isset($wrapper->field_document_unique_id) || count($wrapper->field_document_unique_id->value()) == 0) {
    $warns[] = '<li>' . t('<b>No part number</b> has been specified.') . '</li>';
  }
  if (isset($wrapper->field_platform) && count($wrapper->field_platform->value()) == 1) {
    $warns[] = '<li>' . t('A <b>Platform</b> has been specified. This should only be done when multiple platform version exist; if there is only one (for example, if the only version is for Windows), the tag should be removed.') . '</li>';
  }

  return $warns;
}

function _doc_manage_render_review_panel($ticket) {
  // *** Review panel ***

  $state = longform_get_current_workflow_state($ticket);

  // Render panel content. If postprocessing is not done, show a placeholder message instead of the full content.

  $review_panel['panel'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12'),
    ),
  );

  $review_panel['panel']['head'] = array(
    '#markup' => '<h3>Review Pages</h3>',
  );

  switch ($state->name) {

    // Postprocessing is done; display the review forms and tables.
    case 'review':
    case 'approved':
    case 'publish':

      // Set up region wrappers
      $review_panel['panel']['row']['left'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('col-xs-9', 'left-col'),
        ),
      );
      $review_panel['panel']['row']['right'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('col-xs-3', 'right-col'),
        ),
      );

      // Display a context-aware help message telling the user where they are and what to do next.
      if ($state->name == 'review') {
        $review_panel['panel']['row']['left']['alert'] = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('alert', 'alert-info'),
          ),
        );
        $review_panel['panel']['row']['left']['alert']['msg'] = array(
          '#markup' => '
	    <label>The pages in your document are shown below, along with their current state.</label>
	    <p>Review and approve each page individually. When all pages are have been approved, you will move to the publishing stage to finish the process.</p>
	    <p>Click on a page title below to begin or continue your review.</p>'
        );
      }

      else {
        $review_panel['panel']['row']['left']['alert'] = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('alert', 'alert-success'),
          ),
        );
        $review_panel['panel']['row']['left']['alert']['msg'] = array(
          '#markup' => '
	    <label>You\'ve reviewed everything and are ready to publish.</label>
	    <p>The pages in your document are shown below, along with their current workflow state. You can still reset individual page states here if you find a problem later.</p>
	    '
        );
      }

      // Add the review table.
      $review_panel['panel']['row']['left']['table'] = display_book_relations($ticket);
      break;

    default:
      // Placeholder message; displayed if postprocessing has not been completed.

      // Set up region wrapper
      $review_panel['panel']['row']['msg'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('col-xs-12', 'col-md-8'),
        ),
      );
      $review_panel['panel']['row']['msg']['content'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('alert', 'alert-warning'),
        ),
      );
      $review_panel['panel']['row']['msg']['content']['text'] = array(
        '#markup' => '
	  <label>There\'s nothing to review now.</label>
	  <p>Complete the importing steps above first &ndash; that process creates the pages you will review here.</p>'
      );

      break;
  }

  return $review_panel;

}

function _doc_manage_render_import_panel($ticket, $state) {
  // Import settings subpanel
  $import_settings = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-xs-3', 'right-col'),
    ),
  );

  $import_settings['header'] = array(
    '#markup' => '<h3 class="text-warning">Importing Options</h3>',
  );

  // Import settings: file upload widget
  $import_settings['upload'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('well', 'well-sm', 'frosted-light'),
      'data-toggle' => 'tooltip',
      'data-trigger' => 'hover',
      'data-placement' => 'right auto',
      'data-html' => TRUE,
      'title' => "Upload (or remove and re-upload) a document archive",
    ),
  );

  if (longform_ticket_source_is_attached($ticket)) {
    $archive_upload_form = drupal_get_form('_doc_manage_upload_archive_form');
  }
  else {
    $archive_upload_form = array(
      '#markup' =>
        '<label>Document Archive</label><br/>
        <div class="alert alert-warning">The uploaded archive is now unpacked and cannot be changed.</div>'
    );
  }
  $import_settings['upload']['upload-form'] = $archive_upload_form;

  // Import settings: doc split form
  $import_settings['split'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('well', 'well-sm', 'frosted-light'),
      'data-toggle' => 'tooltip',
      'data-trigger' => 'hover',
      'data-placement' => 'right auto',
      'data-html' => TRUE,
      'title' => "Set the head level to use when building document pages; usually Head1",
    ),
  );


  // Assemble import panel  

  $import_panel = doc_manage_render_import_actions_panel($ticket);

  return $import_panel;
}

/**
 * Returns the wrapper for an accordion element.
 *
 *
 * @param $id
 *  The CSS ID of the element, WITHOUT the preceding '#'.
 *
 * @param $state
 *  Boolean. Determines whether the element is open or closed (TRUE == open).
 *
 * @param $type
 *  One of "pill" or "tab".
 *
 * @return a render array with the appropriate wrappers for an empty accordion
 *   element.
 */
function _render_workflow_tab_wrapper($id, $state = FALSE, $type) {

  $wrap[$id] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('tab-pane', 'fade'),
      'id' => $id,
    ),
  );

  if ($state) {
    $wrap[$id]['#attributes']['class'][] = 'in active';
  }

  return $wrap;
}


/**
 * Display the document TOC once it's published.
 *
 */
function doc_manage_display_published_toc($ticket) {


  // Get document's TOC
  $toc = drupal_json_decode($ticket->field_toc_array[LANGUAGE_NONE][0]['value'], TRUE);

  // Set up wrappers and regions

  // Right region
  // Use the 'no-pad' class override to drop all padding from the fieldset.
  $wrapper['row']['right'] = array(
    '#type' => 'fieldset',
    '#title' => t('Document TOC'),
    '#attributes' => array(
      'class' => array('logo-behind', 'no-pad'),
    ),
  );

  $wrapper['row']['right']['content'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('well', 'well-sm', 'frosted-light'),
    ),
  );
  $wrapper['row']['right']['content']['inner'] = array(
    '#markup' => theme_item_list(_book_nav_bootstrapify_toc($toc)),
  );

  return $wrapper;
}

/**
 * Render document tab
 *
 */
function doc_manage_render_document_pane() {
  $ticket = menu_get_object();

  $wrap = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12'),
    ),
  );
  $wrap['head'] = array(
    '#markup' => '<h2>Document Dashboard</h2>',
  );

  $wrap['left'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-4'),
    ),
  );
  $wrap['left']['content'] = doc_manage_render_document_status($ticket);

  $wrap['right'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-8'),
    ),
  );

  $wrap['right']['content'] = doc_manage_display_published_toc($ticket);

  return $wrap;
}

/**
 * Summary of important links and facts about a document.
 *
 * @param $ticket
 *  The ticket object.
 *
 * @return HTML render array
 */
function doc_manage_render_document_status($ticket) {

  $links = longform_redirect_get_adaptive_permalink();
  $published = workflow_node_current_state($ticket) == 6 ? TRUE : FALSE;
  $m = entity_metadata_wrapper('node', $ticket);

  $stages = _doc_manage_get_all_accordion_states($ticket);

  //$head['actions'] = array(
  //  '#type' => 'container',
  //  '#attributes' => array(
  //    'class' => array('panel', 'panel-default'),
  //  ),
  //);
  //$head['actions']['body'] = array(
  //  '#type' => 'container',
  //  '#attributes' => array(
  //    'class' => array('panel-body'),
  //  ),
  //);

  $head['actions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Actions'),
    '#attributes' => array(
      'class' => array('panel-default'),
    ),
  );

  $head['actions']['cover'] = array(
    '#markup' => l(t('Jump to the cover page'), $links['alias'], array(
        'html' => TRUE,
        'attributes' => array(
          'class' => array('btn', 'btn-primary', 'btn-lg', 'btn-block'),
          'data-toggle' => 'tooltip',
          'data-placement' => 'right auto',
          'data-trigger' => 'hover',
          'title' => t('Go directly to this document\'s cover page'),
        ),
      ))
  );

  $relation = doc_manage_get_book_pages($ticket);
  $head['actions']['bulk_edit'] = array(
    '#markup' => l(t('Bulk edit metadata'), 'bulk-edit/' . $relation[0], array(
        'html' => TRUE,
        'attributes' => array(
          'class' => array('btn', 'btn-default', 'btn-block'),
          'data-toggle' => 'tooltip',
          'data-placement' => 'right auto',
          'data-trigger' => 'hover',
          'title' => t('Modify the metadata values for the pages in this book'),
        ),
      )),
  );

  $links = longform_redirect_get_adaptive_permalink();
  $published = workflow_node_current_state($ticket) == 6 ? TRUE : FALSE;
  $m = entity_metadata_wrapper('node', $ticket);

  // Publish status
  $statii['published'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => 'btn-group',
      'data-toggle' => 'tooltip',
      'data-placement' => 'right auto',
      'title' => $published ? 'This document is live' : 'This document is not public',
      'data-trigger' => 'hover',
    ),
  );
  $statii['published']['label'] = array(
    '#markup' => '<button class="btn btn-default disabled">Publication status</button>',
  );
  $statii['published']['state'] = array(
    '#type' => 'submit',
    '#value' => $published ? t('Published') : t('Unpublished'),
    '#attributes' => array(
      'class' => array(
        'btn',
        'disabled',
        $published ? 'btn-success' : 'btn-danger'
      ),
    ),
  );
  $actions['published'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => 'btn-group',
    ),
  );

  // PDF status
  if (isset($m->field_manual_pdf)) {
    $pdf_value = $m->field_manual_pdf->value();
    $man_pdf = isset($pdf_value) ? $pdf_value : NULL;
  }

  $statii['man_pdf'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => 'btn-group',
      'class' => 'btn-group',
      'data-toggle' => 'tooltip',
      'data-placement' => 'auto',
      'title' => $man_pdf ? 'A PDF is available' : 'No PDF has been uploaded',
      'data-trigger' => 'hover',
    ),
  );
  $statii['man_pdf']['label'] = array(
    '#markup' => '<button class="btn btn-default disabled">Manual PDF</button>',
  );
  if ($man_pdf) {
    $statii['man_pdf']['state'] = array(
      '#markup' => '<button class="btn btn-success disabled">' . t('Available') . '</button>',
    );
    $actions['man_pdf'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => 'btn-group',
      ),
    );
    $actions['man_pdf']['download'] = array(
      '#markup' => l('<span class="glyphicon glyphicon-download"></span>', file_create_url($man_pdf['uri']), array(
          'html' => TRUE,
          'attributes' => array(
            'class' => array('btn', 'btn-default'),
            'data-toggle' => 'tooltip',
            'data-placement' => 'auto',
            'title' => t('Click to download'),
            'data-trigger' => 'hover',
          ),
        )),
    );

  }
  else {
    $statii['man_pdf']['state'] = array(
      '#type' => 'submit',
      '#value' => t('None'),
      '#attributes' => array(
        'class' => array('btn', 'disabled', 'btn-danger'),
      ),
    );
  }
  // Auto PDF status
  if (isset($m->field_auto_pdf)) {
    $auto_pdf = $m->field_auto_pdf->value();
  }

  $statii['auto_pdf'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => 'btn-group',
      'class' => 'btn-group',
      'data-toggle' => 'tooltip',
      'data-placement' => 'auto',
      'title' => $auto_pdf ? 'A PDF is available' : 'No PDF has been generated',
      'data-trigger' => 'hover',
    ),
  );
  $statii['auto_pdf']['label'] = array(
    '#markup' => '<button class="btn btn-default disabled">Automatic PDF</button>',
  );
  if ($auto_pdf) {
    $statii['auto_pdf']['state'] = array(
      '#markup' => '<button class="btn btn-success disabled">' . t('Available') . '</button>',
    );
    $actions['auto_pdf'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => 'btn-group',
      ),
    );
    $actions['auto_pdf']['download'] = array(
      '#markup' => l('<span class="glyphicon glyphicon-download"></span>', file_create_url($auto_pdf['uri']), array(
          'html' => TRUE,
          'attributes' => array(
            'class' => array('btn', 'btn-default'),
            'data-toggle' => 'tooltip',
            'data-placement' => 'auto',
            'title' => t('Click to download'),
            'data-trigger' => 'hover',
          ),
        )),
    );

  }
  else {
    $statii['auto_pdf']['state'] = array(
      '#type' => 'submit',
      '#value' => t('None'),
      '#attributes' => array(
        'class' => array('btn', 'disabled', 'btn-info'),
      ),
    );
  }

  // Software release state
  if (isset($m->field_release_state)) {
    $release_state = $m->field_release_state->value();
  }
  $state_field = field_info_field('field_release_state');
  $statii['release_state'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => 'btn-group',
      'class' => 'btn-group',
      'data-toggle' => 'tooltip',
      'data-placement' => 'auto',
      'title' => t('The current release state for this software version'),
      'data-trigger' => 'hover',
    ),
  );
  $statii['release_state']['label'] = array(
    '#markup' => '<button class="btn btn-default disabled">Release State</button>',
  );
  if (isset($release_state)) {
    $statii['release_state']['state'] = array(
      '#markup' => '<button class="btn btn-info disabled">' . $state_field['settings']['allowed_values'][$release_state] . '</button>',
    );
    $actions['release_state'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => 'btn-group',
      ),
    );
  }
  else {
    $statii['release_state']['state'] = array(
      '#type' => 'submit',
      '#value' => t('Unspecified'),
      '#attributes' => array(
        'class' => array('btn', 'disabled', 'btn-info'),
      ),
    );
  }

  foreach ($statii as $key => $status) {
    $rows[] = array(
      array('data' => render($status)),
      array('data' => render($actions[$key]))
    );
  }

  $table = array(
    '#type' => 'fieldset',
    '#title' => t('Document Status'),
    '#attributes' => array(
      'class' => array('panel-default'),
    ),
  );

  $table['content'] = array(
    '#theme' => 'table',
    '#attributes' => array(
      'class' => array('table'),
    ),
    //'#header' => array(t('Field'),t('Value')),
    '#rows' => $rows,
  );

  return array($head, $table);

}




/**
 * Approve all topics belonging to a given page node.
 *
 * var $node
 *  The book page node to query
 *
 * return array
 *  Assoc array of topic NIDs and their ending workflow states.
 */
function doc_manage_approve_topics($node) {

  $page_topics = doc_manage_get_page_topics($node);

  $topics = node_load_multiple(array_flip($page_topics));

  $count = 0;
  $published = 0;

  foreach ($topics as $topic) {

    $topic_current_state = workflow_node_current_state($topic);
    $return[$topic->nid]['old_state'] = $topic_current_state;

    // Promote component topics to "approved" unless they already have been.
    // Do not downgrade already-published topics to 'approved', either.
    if ($topic_current_state != WORKFLOW_TOPIC_APPROVED_STATE && $topic_current_state != WORKFLOW_TOPIC_PUBLISHED_STATE) {
      workflow_transition($topic, WORKFLOW_TOPIC_APPROVED_STATE, TRUE);
      entity_get_controller('node')->resetCache(array($topic->nid));
      $count++;
    }
    if ($topic_current_state == WORKFLOW_TOPIC_PUBLISHED_STATE) {
      $published++;
    }

    $return[$topic->nid]['new_state'] = workflow_node_current_state($topic);
  }
  if ($count) {
    drupal_set_message($count . ' topics on the page were also approved.');
  }
  if ($published_count) {
    drupal_set_message($published_count . ' topics on this page have already been published. These topics were not changed.');
  }

  // Reset the page cache too, so the new topic statii will show up properly
  entity_get_controller('node')->resetCache(array($node->nid));

  return $return;
}


/**
 * Render PDF options and actions panel.
 *
 * When complete, this should move into the PDF Reactor module.
 *
 * @param $ticket
 *  The ticket node object.
 *
 * @return
 *  Rendered HTML.
 */
function doc_manage_render_pdf_options($ticket) {

  $out['pdf_container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('pdf-options'),
    ),
  );

  $out['pdf_container']['head'] = array(
    '#markup' => '<h2>' . t('Automatic PDF Settings') . '</h2>',
  );
  $out['pdf_container']['alert']['beta_alert'] = array(
    '#markup' => '
      <label>PDF Reactor is in alpha mode.</label><br/>
      PDF Reactor will not automatically run in alpha mode, even if <i>PDF AutoGen</i> is on. You can manually run it by clicking the
      <b>Generate PDF</b> button below.',
    '#prefix' => '<div class="alert alert-warning">',
    '#suffix' => '</div>',
  );

  $out['pdf_container']['left'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-6'),
    ),
  );
  //$out['pdf_container']['left']['about'] = array(
  //  '#type' => 'container',
  //  '#attributes' => array(
  //    'class' => array('well'),
  //  ),
  //);

  // Temporary alert while PDF reactor implementation is in beta.
  // During this testing phase, PDF reactor will not fire automatically and must be manually triggered.
  // Remove this alert once PDF reactor is tested and ready.

  $out['pdf_container']['left']['about'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel', 'panel-default'),
    ),
  );
  $out['pdf_container']['left']['about']['head'] = array(
    '#markup' => '<h3 class="panel-title">About PDF Reactor</h3>',
    '#prefix' => '<div class="panel-heading">',
    '#suffix' => '</div>',
  );
  $out['pdf_container']['left']['about']['content'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel-body'),
    ),
  );
  $out['pdf_container']['left']['about']['content']['text'] = array(
    '#markup' => '
    <p>PDF Reactor is a service that automatically generates a PDF of your document. When <i>PDF AutoGen</i> is on, a PDF will be generated immediately when you publish your document, and will be available to users as a download.</p>
    <p>When <i>PDF Synchronization</i> is on, PDF Reactor will also re-generate and replace the PDF whenever any pages in the document are manually changed after publication.</p>',
  );

  $out['pdf_container']['middle']['settings_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('alert', 'alert-info'),
    ),
  );
  $out['pdf_container']['middle']['settings_wrapper']['pdf_mode_head'] = array(
    '#type' => 'markup',
    '#markup' => '<label>' . t('PDF Settings') . '</label>',
  );
  $out['pdf_container']['middle']['settings_wrapper']['pdf_mode'] = array(
    '#type' => 'markup',
    '#markup' => flag_create_link('pdf_reactor_mode', $ticket->nid) . '<br/>',
  );

  $out['pdf_container']['middle']['settings_wrapper']['pdf_sync_mode'] = array(
    '#type' => 'markup',
    '#markup' => flag_create_link('pdf_reactor_live_sync', $ticket->nid),
  );

  $out['pdf_container']['middle'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-3'),
    ),
  );
  $out['pdf_container']['middle']['settings_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('alert', 'alert-info'),
    ),
  );
  $out['pdf_container']['middle']['settings_wrapper']['pdf_mode_head'] = array(
    '#type' => 'markup',
    '#markup' => '<label>' . t('PDF Settings') . '</label>',
  );
  $out['pdf_container']['middle']['settings_wrapper']['pdf_mode'] = array(
    '#type' => 'markup',
    '#markup' => flag_create_link('pdf_reactor_mode', $ticket->nid) . '<br/>',
  );

  $out['pdf_container']['middle']['settings_wrapper']['pdf_sync_mode'] = array(
    '#type' => 'markup',
    '#markup' => flag_create_link('pdf_reactor_live_sync', $ticket->nid),
  );


  $out['pdf_container']['right'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-3'),
    ),
  );

  $out['pdf_container']['right']['actions_wrapper']['generate_pdf_intro'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('well', 'well-sm'),
    ),
  );

  $out['pdf_container']['right']['actions_wrapper']['generate_pdf_intro']['text'] = array(
    '#markup' => '<label>' . t('Force generate a PDF') . '</label><br/><p>' . t('To generate a PDF now, click here.') . '<br/><br/></p>',
  );

  $out['pdf_container']['right']['actions_wrapper']['generate_pdf_intro']['force_generate'] = drupal_get_form('doc_manage_pdfreactor_button_form');

  // If an manually-uploaded PDF is available, make it available for download.
  if (isset($ticket->field_manual_pdf[LANGUAGE_NONE][0])) {
    $manual_upload = TRUE;

    $out['pdf_container']['right']['downloads'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('well', 'well-sm'),
      ),
    );

    $file = $ticket->field_manual_pdf[LANGUAGE_NONE][0];
    $out['pdf_container']['right']['downloads']['download'] = array(
      '#markup' => l('<span class="glyphicon glyphicon-download"></span> ' . t('Download manual PDF'), file_create_url($file['uri']), array(
          'html' => TRUE,
          'attributes' => array(
            'class' => array(
              'btn',
              'btn-default',
              'btn-block'
            )
          )
        )),
    );
  }

  // If an auto-generated PDF is available, make it available for download.
  if (isset($ticket->field_auto_pdf[LANGUAGE_NONE][0])) {

    if ($manual_upload) {
      $out['pdf_container']['right']['downloads']['warning'] = array(
        '#markup' => '<div class="alert alert-danger"><label>' . t('Note:') . '</label></br>' . t('This PDF will not be seen by the public, because a manually-uploaded PDF exists.') . '</div>',
      );
    }
    $file = $ticket->field_auto_pdf[LANGUAGE_NONE][0];
    $out['pdf_container']['right']['downloads']['download'] = array(
      '#markup' => l('<span class="glyphicon glyphicon-download"></span> ' . t('Download auto PDF'), file_create_url($file['uri']), array(
          'html' => TRUE,
          'attributes' => array(
            'class' => array(
              'btn',
              'btn-default',
              'btn-block'
            )
          )
        )),
    );
  }


  return $out;
}


/**
 * Render import process panel.
 *
 * @param $ticket
 *  The ticket node object.
 *
 * @return
 *  Rendered HTML.
 */
function doc_manage_render_import_actions_panel($ticket) {

  $state = longform_get_current_workflow_state($ticket);
  $import_steps_info = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-xs-12'),
    ),
  );
  // Generate instruction block. There are three versions: No archive uploaded, In progress, and finished.
  $import_steps_info['head'] = array(
    '#markup' => '<h3>Document Import</h3>'
  );
  if ($state->name == 'review' || $state->name == 'publish') {
    // all done
    $import_steps_info['wrapper'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('alert', 'alert-success'),
      ),
    );
    $import_steps_info['wrapper']['content'] = array(
      '#markup' => '<label>You\'re done with this task!</label><p>There\'s nothing more to do here for now. If you need to, you can rewind the importing process by using the <button class="btn btn-sm btn-danger disabled">undo</button> buttons below.</p>'
    );
  }
  else {
    if ($state->name == 'unpack' && !longform_ticket_source_is_attached($ticket)) {

      // No archive has been uploaded yet. Provide instructions on what to do next.

      $import_steps_info['wrapper'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('alert', 'alert-warning', 'col-md-8'),
        ),
      );
      $import_steps_info['wrapper']['content'] = array(
        '#markup' => '
      <label>You can\'t start this process yet.</label>
      <p>This ticket is currently a stub, which means no document archive has been uploaded. To upload one, use the uploader on the right and follow these steps:</p><p>&nbsp;</p>
      <ol>
	<li>Click <b>choose file</b>, and locate the archive on your system. Click <b>open</b> or <b>ok</b> on your system\'s file window.</li>
	<li>Back on this screen, click <button class="btn btn-sm btn-default disabled">Upload</button> next to the filename. The file will be uploaded to the server.</li>
	<li>Click <button class="btn btn-sm btn-info disabled">Apply</button>. The uploaded archive will be attached and saved.</li>
      </ol>
      <div class="alert text-danger"><b>Note:</b> You must click both buttons (Upload <i>and</i> Apply), or the uploaded file will not be saved.</div>'
      );
      $import_steps_info['uploader'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('col-md-4'),
        ),
      );
      $import_steps_info['uploader']['content'] = drupal_get_form('_doc_manage_upload_archive_form');
    }
    else {
      // In progress

      $import_steps_info['wrapper'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('alert',/*'alert-info'*/),
        ),
      );
      $import_steps_info['wrapper']['content'] = array(
        '#markup' => 'Complete each step in sequence. When the last step is done, you will move to the <b>reviewing</b> stage.'
      );
    }
  }

  $import_steps_info['form'] = !longform_ticket_source_is_attached($ticket) ? '' : drupal_get_form('_doc_manage_import_steps_form', $ticket);

  return $import_steps_info;
}


/**
 * Provide pubish/unpublish button.
 */
function doc_manage_publish_button($form, &$form_state) {

  $node = menu_get_object();
  $sid = workflow_node_current_state($node);
  $form_state['sandbox']['node'] = $node;

  // sid (6) is published

  switch ($sid) {
    case 6:
      $form['intro'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="alert alert-danger"><label>This document has been published.</label><br/><p>Click here to unpublish and remove the document from public view. <b>This will not delete any information, only pull it offline for anonymous (public) viewers</b>.</p></div>',
      );


      $form['publish'] = array(
        '#type' => 'submit',
        '#id' => 'unpublish-now',
        '#submit' => array('_doc_manage_unpublish_submit'),
        '#attributes' => array(
          'class' => array('btn-lg', 'btn-danger', 'btn-block'),
        ),
        '#value' => 'Unpublish'
      );

      break;

    default:
      $form['intro'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="alert alert-success"><label>Publish this document now</label><br/><p>Clicking this button will trigger all publishing mechanisms and expose the document to visitors.</p></div>',
      );

      $form['publish'] = array(
        '#type' => 'submit',
        '#id' => 'publish-now',
        '#submit' => array('_doc_manage_publish_submit'),
        '#attributes' => array(
          'class' => array('btn-lg', 'btn-success', 'btn-block'),
        ),
        '#value' => 'Publish!'
      );

      break;

  }

  return $form;

}

/**
 * Bulk publish content.
 *
 * This is a submit callback for _doc_manage_publish_button().
 *
 */
function _doc_manage_publish_submit($form, &$form_state) {

  $node = $form_state['sandbox']['node'];
  $published_state = 6;
  $pdf_mode = flag_get_flag('pdf_reactor_mode');
  $pdf_mode_set = is_object($pdf_mode) ? $pdf_mode->is_flagged($node->nid) : FALSE;


  // @TODO: The next few statements (up to the batch) will execute even if the batch fails.
  // Move these into a new function (maybe batch finished?) to prevent bad things from happening.

  // Set page states to 'published'
  doc_manage_bulk_publish($ticket, 'flag');

  // Set ticket state to 'published'
  workflow_execute_transition($node, $published_state, 'Publish button clicked', TRUE);
  entity_get_controller('node')->resetCache(array($node->nid));

  // For now, shut off PDF REACTOR. Fake the "PDF REACTOR MODE" flag to true ( == do not generate).
  $pdf_mode_set = TRUE;

  //set up batch
  if (!$pdf_mode_set) {
    $operations[] = array('pdfreactor_import_ticket_generate', array($node));
  }
  $operations[] = array('doc_manage_bulk_publish', array($node, 'flag'));

  $title = !$pdf_mode_set ? t('Creating PDF and publishing nodes') : t('Publishing nodes (no PDF will be generated)...');

  $batch = array(
    'operations' => $operations,
    'file' => drupal_get_path('module', 'doc_manage') . '/doc_manage.inc',
    'finished' => '_doc_manage_publish_finished',
    'title' => $title,
    'init_message' => t('Starting processes...'),
    'progress_message' => t('Completed @current task out of @total.'),
    'error_message' => t('An error occurred while publishing nodes.'),
  );

  batch_set($batch);
  batch_process();

}

function _doc_manage_publish_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message('Publication tasks completed.');
  }
  else {
    drupal_set_message('An error occurred while publishing nodes or creating PDFs. Your PDF may not have been created and/or your pages may not be published.', 'error');
  }
}

/**
 * Bulk unpublish content.
 *
 * This is a submit callback for _doc_manage_publish_button().
 *
 */
function _doc_manage_unpublish_submit($form, &$form_state) {

  $node = menu_get_object();
  $approved_state = 5;

  // Set page states to 'published'
  doc_manage_bulk_publish($ticket, 'unflag');

  // Set ticket state to 'published'
  workflow_execute_transition($node, $approved_state, 'Unpublish button clicked', TRUE);
  entity_get_controller('node')->resetCache(array($node->nid));
}


/**
 * Generate file upload form for document archive.
 *
 */
function _doc_manage_upload_archive_form($form, &$form_state) {

//dpm($form_state,'file form state');
  if (isset($form_state['sandbox']) && $form_state['sandbox']['entity']) {
    $entity = $form_state['sandbox']['entity'];
  }
  else {
    $entity = longform_entities_get_entity();
    $form_state['sandbox']['entity'] = $entity;
  }
  // field_default_form($entity_type, $entity, $field, $instance, $langcode, $items, &$form, &$form_state, $get_delta = NULL)

  $form = field_default_form(
    $entity->type,
    $entity,
    field_info_field('field_document_source'),
    field_info_instance('longform_ticket', 'field_document_source', 'longform_ticket'),
    LANGUAGE_NONE,
    field_get_items('longform_ticket', $entity, 'field_document_source'),
    $form,
    $form_state
  );

  // Remove description 
  unset ($form['field_document_source'][LANGUAGE_NONE][0]['#description']);

  // Add required includes
  if (empty($form_state['build_info']['files']) || !in_array('modules/node/node.pages.inc', $form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'node', 'node.pages');
    form_load_include($form_state, 'inc', 'doc_manage', 'doc_manage');
  }

  $form['field_document_source']['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('_doc_manage_upload_archive_submit'),
    '#value' => 'Apply',
  );

  $form['field_document_source']['description'] = array(
    '#type' => 'markup',
    '#markup' => 'Upload (or remove) the prepared archive.',
    '#prefix' => '<br/><br/><div class="alert alert-info">',
    '#suffix' => '</div>',
  );

  $form['field_document_source']['feedback'] = array(
    '#type' => 'markup',
    '#markup' => '<div id="upload-feedback"></div>',
  );

  return $form;

}

/**
 * Submit handler for document upload.
 *
 */
function _doc_manage_upload_archive_submit($form, &$form_state) {

  $new_fid = $form_state['values']['field_document_source'][LANGUAGE_NONE][0]['fid'];

  $entity = $form_state['sandbox']['entity'];

  $wrapper = entity_metadata_wrapper($entity->type, $entity);
  $file = $wrapper->field_document_source->value();
  $old_fid = $file['fid'];

  if ($new_fid == 0 && !isset($file)) {
    drupal_set_message('No file was uploaded.', 'error');
    return FALSE;
  }


  if ($new_fid == 0 && isset($file)) {

    // Remove the file.
    file_delete(file_load($old_fid));
    $wrapper->field_document_source->set(NULL);
    $wrapper->save(TRUE);
    field_attach_update($entity->type, $entity);

  }
  else {

    // A new file is here!

    if ($old_fid != 0) {
      // First, remove the old one
      $wrapper->field_document_source->set(NULL);
      file_delete(file_load($old_fid));
      $wrapper->save(TRUE);
      field_attach_update($entity->type, $entity);
    }

    // Add/save the new one

    // Filesystem housekeeping
    $new_file = file_load($new_fid);
    $new_file->status = FILE_STATUS_PERMANENT;
    file_save($new_file);
    file_usage_add($new_file, 'doc_manage', $wrapper->type->value(), $wrapper->getIdentifier());
    $new_file_array = array('fid' => $new_fid, 'display' => TRUE);

    // Update the file field
    $wrapper->field_document_source->set($new_file_array);
    $wrapper->save(TRUE);
    field_attach_update($entity->type, $entity);

  }
}


/**
 * generate node import action form
 */
function _doc_manage_import_steps_form($form, &$form_state, $node) {

  $form['wrapper']['import'] = _publishing_actions_import($node, $form_state);

  return $form;
}

/**
 * Returns the wrapper for an accordion element.
 *
 * @param $title
 *  The title text to display in the header
 *
 * @param $id
 *  The CSS ID of the element, WITHOUT the preceding '#'.
 *
 * @param $state
 *  'in' or ''. Determines whether the element is open or closed.
 *
 * @param $parent
 *  The data-parent element. This is the ID of the containing panel-group,
 *   WITHOUT the preceding '#'.
 *
 * @return a render array with the appropriate wrappers for an empty accordion
 *   element.
 */
function _render_workflow_accordion_wrapper($title, $id, $state = '', $parent) {

  $wrapper = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel', 'panel-default', 'panel-nopad'),
    ),
  );
  $wrapper['heading'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel-heading'),
    ),
  );

  // Create heading link element. This will become the panel accordion trigger.
  $heading = l('<span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' . t($title), '#', array(
      'html' => TRUE,
      'fragment' => $id,
      'external' => TRUE,
      'attributes' => array(
        'data-toggle' => 'collapse',
        'data-parent' => '#' . $parent,
        'class' => array(
          'btn',
          'btn-lg',
          'justify',
          'btn-collapse-trigger',
          $mode
        ),
      ),
    ));


  $wrapper['heading']['content'] = array(
    '#type' => 'markup',
    '#markup' => $heading
  );

  $wrapper['panel'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel-collapse', 'collapse', $state),
      'id' => $id,
    ),
  );

  $wrapper['panel']['row'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel-body', 'row'),
    ),
  );

  return $wrapper;

}

function _publishing_actions_publish($node) {
  $form['import']['content'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="publish-actions-content" class="' . $toggle . '">',
    '#suffix' => '</div>',
    '#markup' => '<h1>Content is here!</h1>',
  );

  return $form;
}

function doc_mandage_unpack($form,&$form_state) {
  dpm('yes');
}
/**
 * Helper function that supplies the steps for importing a document.
 */
function _publishing_actions_import($node, &$form_state) {

  $options_array = array();

  $entity = longform_entities_get_entity();
  $state = longform_get_current_workflow_state($entity);

  // Build command render array.

  $button_class_base = array('btn-block', 'col-md-2');
  $button_classes = array(
    'locked' => array('btn-default'),
    'finished' => array('btn-default'),
    'current' => array('btn-success'),
    'undo' => array('btn-danger'),
  );

  $options_array['unpack'] = array(
    'title' => 'Unpack',
    'description' => '<p>Unpack the document archive.</p>',
    'include' => array('unpack'),
    'submit' => array(
      '#type' => 'submit',
      '#id' => 'unpack-submit',
      '#value' => 'Unpack',
      '#submit' => array('doc_manage_unpack'),
    ),
    'undo' => array(
      '#type' => 'submit',
      '#id' => 'unpack-undo',
      '#value' => 'Undo Unpack',
      '#submit' => array('doc_manage_undo_unpack'),
    ),

  );
  $options_array['split'] = array(
    'title' => 'Split',
    'description' => '<p>Splits the document into topics and saves them in the database.</p>',
    'include' => array('split'),
    'submit' => array(
      '#type' => 'submit',
      '#id' => 'split-submit',
      '#value' => 'Split',
      '#submit' => array('doc_import_split'),
    ),
    'undo' => array(
      '#type' => 'submit',
      '#id' => 'split-undo',
      '#value' => 'Undo Split',
      '#submit' => array('doc_manage_undo_split'),
    ),

  );
  $options_array['convert'] = array(
    'title' => 'Convert',
    'description' => '<p>Convert the raw HTML to Technet-o-riffic HTML. </p>',
    'include' => array('convert'),
    'submit' => array(
      '#type' => 'submit',
      '#id' => 'convert-submit',
      '#value' => 'Convert',
      '#submit' => array('doc_import_convert'),
    ),
    'undo' => array(
      '#type' => 'submit',
      '#id' => 'convert-undo',
      '#value' => 'Undo Convert',
      '#submit' => array('doc_manage_undo_convert'),
    ),
  );
  $options_array['save'] = array(
    'title' => 'Save',
    'description' => '<p>Assemble topics into pages. A new page will start at each heading specified as your split level.</p>',
    'include' => array('save'),
    'submit' => array(
      '#type' => 'submit',
      '#id' => 'save-submit',
      '#value' => 'Save',
      '#submit' => array('doc_manage_save_nodes'),
    ),
    'undo' => array(
      '#type' => 'submit',
      '#id' => 'save-undo',
      '#value' => 'Undo Save',
      '#submit' => array('doc_manage_undo_save'),
    ),
  );

  $options_array['post'] = array(
    'title' => 'Postprocess',
    'description' => '<p>Rebuild cross-references, add page links, and other pre-publication tasks.</p>',
    'include' => array('postprocess'),
    'submit' => array(
      '#type' => 'submit',
      '#id' => 'repair-xref-submit',
      '#value' => 'Postprocess',
      '#submit' => array('doc_import_postprocess'),
    ),
    'undo' => array(
      '#type' => 'submit',
      '#id' => 'postprocess-undo',
      '#value' => 'Undo Post',
      '#submit' => array('doc_manage_undo_postprocess'),
    ),
  );

  $table = array();
  $table['head'] = array(
//    array('data' => 'Step'),
    array('data' => 'Action'),
    array('data' => 'Description'),
  );

  $i = 0;
  foreach ($options_array as $name => $option) {
    $i++;

    // Bring in required includes
    foreach ($option['include'] as $include) {
      form_load_include($form_state, 'inc', 'doc_import', "doc_import.$include");
    }

    $step = $i;
    $submit = $option['submit'];
    $status = $state->name == $name ? 'current' : 'locked';
    $submit['#attributes']['class'] = array_merge($button_class_base, $button_classes[$status]);

    // Check for potential issues prior to saving content.
    if ($option['title'] == 'Save' && $option['state'] == 'next') {
      $warns = _doc_manage_metadata_warnings($node);
      if ($warns) {
        global $base_path;
        $warnings = array(
          '#markup' => '<label>Before saving, verify that these items are intended:</label><ul>' . implode(PHP_EOL, $warns) . '</ul>
			<p><b>If any of these are in error</b>, correct them by clicking <a href="' . $base_path . 'node/' . $node->nid . '/edit" class="btn btn-default btn-sm"><span class="glyphicon glyphicon-edit"></span>Edit</a> before saving.',
          '#prefix' => '<div class="alert alert-warning">',
          '#suffix' => '</div>',
        );
      }
    }

    $description = array(
      '#type' => 'markup',
      '#title' => $option['title'],
      '#markup' => '<div class="description">' . (isset($warnings) ? render($warnings) : $option['description']) . '</div>',
    );

    $buttons[] = $submit;
    $table['rows'][] = array(
//      array('data' => '<h2>' . $step . '</h2>'),
      array('data' => $submit),
      array('data' => $description),
    );
  }
  return $buttons;

  $form = array(
    '#theme' => 'table',
    '#header' => $table['head'],
    '#rows' => $table['rows'],
  );
  return $form;
}


/**
 * Returns state of individual processing steps.
 *
 * This call must originate from an import ticket form.
 *
 * @param $ticket
 *  The ticket object to check. If not supplied, the current page will be
 *   loaded.
 *
 * @return
 *  An associative array of steps and corresponding states.
 */
function _doc_manage_get_import_state($ticket = NULL) {

  $types = array('unpack', 'split', 'convert', 'save', 'post');

  if (!$ticket) {
    $ticket = menu_get_object();
  }
  $query = relation_query('node', $ticket->nid);
  $query->entityCondition('bundle', 'ticket_contains_topics');

  // Get topic relation
  $topic_relation = $query->execute();
  if (count($topic_relation) != 0) {
    $topic_relation = array_shift($topic_relation);
    $topic_relation = relation_load($topic_relation->rid);
  }
  // Get page relation
  $new_query = relation_query('node', $ticket->nid);
  $new_query->entityCondition('bundle', 'ticket_contains_book_pages');

  $page_relation = $new_query->execute();
  if (count($page_relation) != 0) {
    $page_relation = array_shift($page_relation);
    $page_relation = relation_load($page_relation->rid);
  }

  $states = array();

  $finished_count = 0;

  foreach ($types as $type) {
    switch ($type) {
      case 'unpack':
        //unpacking will be the next step if there is no controller directory.
        if (!isset($ticket->field_raw_html_master[LANGUAGE_NONE][0])) {
          // "Unpack" will be active if there is an archive attached. If there is not, it will be pending (along with everything else).
          if (!empty($ticket->field_document_source) && $ticket->field_document_source[LANGUAGE_NONE][0]['fid']) {
            $states[$type] = 'next';
          }
          else {
            $states[$type] = 'pending';
          }
        }
        else {
          $states[$type] = 'finished';
          $finished_count++;
        }
        break;

      case 'split':
        if ($states['unpack'] != 'finished') {
          $states[$type] = 'pending';
          continue;
        }

        if ($topic_relation) {
          $states['unpack'] = 'locked';
          $states[$type] = 'finished';
          $finished_count++;
          continue;
        }
        else {
          $states[$type] = 'next';
          continue;
        }
        break;

      case 'convert':

        if ($states['split'] != 'finished') {
          $states[$type] = 'pending';
          continue;
        }

        //test the first topic node to see if it has been converted. Since conversions are bulk processes, if the first has been done, they all have.
        if (file_exists("public://longform/imports/expanded/" . $ticket->nid . "/converted.lck")) {
          $states[$type] = 'finished';
          $states['split'] = 'locked';
          $finished_count++;
        }
        else {
          $states[$type] = 'next';
          continue;
        }
        break;

      case 'save':
        if ($states['convert'] != 'finished') {
          $states[$type] = 'pending';
          continue;
        }
        $test_topic = node_load($topic_relation->endpoints[LANGUAGE_NONE][1]['entity_id']);
        if ($test_topic->field_topic_processed_html) {

          if ($page_relation) {
            $states[$type] = 'finished';
            $states['convert'] = 'locked';
            $finished_count++;
          }
          else {
            $states[$type] = 'next';
          }
        }
        break;

      case 'post':
        if ($states['save'] != 'finished') {
          $states[$type] = 'pending';
          continue;
        }

        //'post' is either done or not now:
        if (file_exists("public://longform/imports/expanded/" . $ticket->nid . "/postprocessed.lck")) {
          $states[$type] = 'finished';
          $states['save'] = 'locked';
          $finished_count++;
        }
        else {
          $states[$type] = 'next';
        }
        break;

      case 'pdf':
        if ($states['post'] != 'finished') {
          $states[$type] = 'pending';
          continue;
        }
        else {
          $states[$type] = 'next';
        }
    }
  }
  if ($finished_count == count($types)) {
    $states['finished'] = TRUE;
  }

  return $states;
}


/**
 * Undo functions: Destroy archive and saved files
 *
 * @param $ticket
 *  Controlling import ticket. If nothing is passed, the current object will be
 *   loaded.
 */
function doc_manage_undo_unpack($form, &$form_state) {

  $ticket = menu_get_object();

  $resource_directory = "public://import-source-files/" . $ticket->nid . "/";
  $file = file_load($ticket->field_raw_html_master[LANGUAGE_NONE][0]);

  // Drop the raw HTML file from the field and update the ticket
  unset($ticket->field_raw_html_master[LANGUAGE_NONE][0]);
  node_save($ticket);
  // Delete the file from disk
  if ($file) {
    $success = file_delete($file);
  }

  // Recursively delete the resource directory. This removes all images, HTML files,
  // and anything else generated by the unpack operation.
  module_load_include('inc', 'doc_import');
  _rmdir_recursive($resource_directory);

  //document conversion complete, update workflow state
  $comment = t("State changed by import module (Undo Unpack operation)");
  $new_state = workflow_execute_transition($ticket, WORKFLOW_TICKET_CREATED_STATE, $comment, TRUE);
  entity_get_controller('node')->resetCache((array) $ticket->nid);

  drupal_set_message('Extracted files have been removed and resource directory deleted. The uploaded file archive is still attached to this ticket. <br/>
		     Document state has been set to "Created".');

}

/**
 * Undo functions: Bulk delete topics
 *
 * @param $ticket
 *  Controlling import ticket. If nothing is passed, the current object will be
 *   loaded.
 */
function doc_manage_undo_split($form, &$form_state) {

  $ticket = menu_get_object();
  $operations = array();

  //set up batch
  $topics = _doc_manage_get_topics($ticket);

  $operations[] = array(
    '_doc_manage_batch_delete_topics_op',
    array($topics, $ticket)
  );
  $batch = array(
    'operations' => $operations,
    'file' => drupal_get_path('module', 'doc_manage') . '/doc_manage.inc',
    'finished' => '_doc_manage_batch_delete_topics_finished',
    'title' => t('Deleting topics...'),
    'init_message' => t('Starting processes...'),
    'progress_message' => t('Completed @current task of @total.'),
    'error_message' => t('There was a problem while deleting topic nodes.'),
  );

  batch_set($batch);
  batch_process();
}

function _doc_manage_batch_delete_topics_op($topics, $ticket, &$context) {
  // information needed to track progression between successive calls.
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_topic'] = 0;

    // Save topic count for the termination message.
    $context['sandbox']['max'] = count($topics);

    // log controller node
    $context['results']['ticket'] = $ticket;
  }

  $result = array_slice($topics, $context['sandbox']['current_topic'], 10); //last parameter is $limit. Caps number of jobs that can be run at once.
  foreach ($result as $section) {
    node_delete($section->nid);
    // Store some results for post-processing in the 'finished' callback.
    $context['results']['topics'][] = $section->nid;
  }

  // Update our progress information.
  $context['sandbox']['progress']++;
  $context['sandbox']['current_topic']++;
  $context['message'] = 'Deleting item ' . $context['sandbox']['current_topic'] . ' of ' . $context['sandbox']['max'] . ': ' . $relation[$context['sandbox']['current_topic']]['entity_id'];

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }

}

function _doc_manage_batch_delete_topics_finished($success, $results, $operations) {
  if ($success) {
    $ticket = $results['ticket'];

    //document conversion complete, update workflow state
    $target_state = 26; //26 = "Importing" state
    $comment = t("State changed by import module (Undo Split Topics operation)");
    $new_state = workflow_execute_transition($ticket->nid, $target_state, $comment, TRUE);
    entity_get_controller('node')->resetCache((array) $ticket->nid);

    drupal_set_message('All topics generated from this archive have been deleted, and all database relations removed.<br/>
		     Document state has been reset to "Importing".');

  }
  else {
    drupal_set_message('Error while deleting topic nodes.', 'error');
  }
}


/**
 * Undo functions: Undo conversions
 *
 * @param $ticket
 *  Controlling import ticket. If nothing is passed, the current object will be
 *   loaded.
 */
function doc_manage_undo_convert($form, &$form_state) {

  $ticket = menu_get_object();
  //pick up the entities referenced by this controller, then drop the first element (which is the controller).
  $topics = _doc_manage_get_topics($ticket);

  $operations[] = array(
    '_doc_manage_batch_unconvert_op',
    array($topics, $ticket)
  );
  $batch = array(
    'operations' => $operations,
    'file' => drupal_get_path('module', 'doc_manage') . '/doc_manage.inc',
    'finished' => '_doc_manage_batch_unconvert_finished',
    'title' => t('Removing converted HTML...'),
    'init_message' => t('Starting processes...'),
    'progress_message' => t('Processed @current topic out of @total.'),
    'error_message' => t('There was a problem while unconverting topics.'),
  );

  batch_set($batch);
  batch_process();
}

function _doc_manage_batch_unconvert_op($topics, $ticket, &$context) {
  // information needed to track progression between successive calls.
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_topic'] = 0;

    // Save topic count for the termination message.
    $context['sandbox']['max'] = count($topics);

    // Pass ticket through for finished
    $context['results']['ticket'] = $ticket;
  }

  $result = array_slice($topics, $context['sandbox']['current_topic'], 1); //last parameter is $limit. Caps number of jobs that can be run at once.
  foreach ($result as $section) {
    $node = node_load($section->nid);
    $node->field_topic_processed_html = NULL;
    node_save($node);
    // Store some results for post-processing in the 'finished' callback.
    $context['results']['processed'][] = $node->nid;
  }

  // Update our progress information.
  $context['sandbox']['progress']++;
  $context['sandbox']['current_topic']++;
  $context['message'] = 'Processing item ' . $context['sandbox']['current_topic'] . ' of ' . $context['sandbox']['max'] . ': ' . $relation[$context['sandbox']['current_topic']]['entity_id'];

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }

}

function _doc_manage_batch_unconvert_finished($success, $results, $operations) {
  if ($success) {
    $ticket = $results['ticket'];
    // Remove lock file
    $lock = "public://longform/imports/expanded/" . $ticket->nid . "/converted.lck";
    if (file_exists($lock)) {
      $hooray = file_unmanaged_delete($lock);
    }
  }
  if ($hooray) {
    drupal_set_message('All related topics were unconverted.');
  }
  else {
    drupal_set_message('Error while unconverting topics.', 'error');
  }
}


/**
 * Undo functions: Bulk delete book pages
 *
 * @param $ticket
 *  Controlling import ticket. If nothing is passed, the current object will be
 *   loaded.
 */
function doc_manage_undo_save($form, &$form_state) {
  //pick up the entities referenced by this controller, then drop the first element (which is the controller).
  $endpoints = doc_manage_relation_query('ticket_contains_book_pages', $ticket);
  $dump = array_shift($endpoints);

  $operations = array();

  //set up batch
  $operations[] = array(
    '_doc_manage_batch_delete_nodes_op',
    array($endpoints, $dump)
  );
  $batch = array(
    'operations' => $operations,
    'file' => drupal_get_path('module', 'doc_manage') . '/doc_manage.inc',
    'finished' => '_doc_manage_batch_delete_nodes_finished',
    'title' => t('Deleting pages...'),
    'init_message' => t('Starting processes...'),
    'progress_message' => t('Deleted @current page out of @total.'),
    'error_message' => t('There was a problem while deleting page nodes.'),
  );

  batch_set($batch);
  batch_process();
}

function _doc_manage_batch_delete_nodes_op($endpoints, $ticket, &$context) {
  // information needed to track progression between successive calls.
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_topic'] = 0;

    // Save topic count for the termination message.
    $context['sandbox']['max'] = count($endpoints);

    // log controller node
    $context['results']['controller'] = $ticket;
  }

  $result = array_slice($endpoints, $context['sandbox']['current_topic'], 1); //last parameter is $limit. Caps number of jobs that can be run at once.
  foreach ($result as $section) {
    node_delete($section['entity_id']);
    // Store some results for post-processing in the 'finished' callback.
    $context['results']['endpoints'][] = $section['entity_id'];
  }

  // Update our progress information.
  $context['sandbox']['progress']++;
  $context['sandbox']['current_topic']++;
  $context['message'] = 'Deleting item ' . $context['sandbox']['current_topic'] . ' of ' . $context['sandbox']['max'] . ': ' . $relation[$context['sandbox']['current_topic']]['entity_id'];

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }

}

function _doc_manage_batch_delete_nodes_finished($success, $results, $operations) {
  if ($success) {
    //$query = relation_query('node', $results['controller']['entity_id'], 'ticket_contains_book_pages');
    //$topic_relation = $query->execute();
    //$topic_relation = array_shift($topic_relation);
    //relation_delete($topic_relation->rid);
    drupal_set_message('All selected nodes were deleted.');
  }
  else {
    drupal_set_message('Error while deleting nodes.', 'error');
  }
}


/**
 * Undo functions: destroy postprocessing data
 *
 * @param $ticket
 *  Controlling import ticket. If nothing is passed, the current object will be
 *   loaded.
 */
function doc_manage_undo_postprocess($form, &$form_state) {
  $ticket = menu_get_object();
  //pick up the entities referenced by this controller, then drop the first element (which is the controller).
  $endpoints = doc_manage_relation_query('ticket_contains_book_pages', $ticket);
  $parent_tmp = array_shift($endpoints);
  $cover_tmp = array_shift($endpoints);

  $nodes = node_load_multiple($endpoints);
  foreach ($nodes as $node) {
    $body = $node->body[LANGUAGE_NONE][0]['value'];
    $body = htmlqp($body, 'a.page-xref-anchor')
      ->remove()
      ->top()
      ->find('div.page-links')
      ->remove()
      ->top()
      ->find('body')
      ->innerHTML();
    $body = str_replace('<!--end link insert-->', '', $body);
    $node->body[LANGUAGE_NONE][0]['value'] = $body;
    node_save($node);
  }

  // Drop the TOC field from the import ticket.
  $wrap = entity_metadata_wrapper('node', $ticket);
  $wrap->field_toc_array->set(NULL);
  $wrap->save();

  // Remove lock file
  $lock = "public://longform/imports/expanded/" . $ticket->nid . "/postprocessed.lck";
  if (file_exists($lock)) {
    $success = file_unmanaged_delete($lock);
  }
  if ($success) {
    drupal_set_message('Postprocess functions undone.');
  }
  else {
    drupal_set_message('Could not undo postprocess functions.', 'error');
  }
}


/*
 * Generate a table of related book nodes for this ticket.
 * Also returns the workflow state of each node.
 */
function display_book_relations($ticket) {

  $element = array();
  global $base_path;

  $book_pages = longform_relation_get_other_endpoint_ticket_page($ticket, TRUE);
  foreach ($book_pages as $endpoint) {

    $flag = flag_get_flag('html_problem_alert');
    $flagged = $flag->is_flagged($endpoint);

    $flag_cell = $flagged ? '<div class="flag-error"><span class="glyphicon glyphicon-exclamation-sign"></span></div>' : '<div class="flag-ok"><span class="glyphicon glyphicon-ok-sign"></span></div>';

    $wrap = entity_metadata_wrapper('node', $endpoint);

    $cell['related-node']['href'] = 'node/' . $wrap->getIdentifier();
    $cell['related-node']['title'] = (reset($book_pages) == $endpoint) ? $wrap->title_field->value() . ' [Cover page automatically approved]' : $wrap->title_field->value();

    $depth = "book-depth-" . $endpoint->book['depth'];
    $attributes = array(
      'html' => TRUE,
      'class' => array('links', 'inline', $depth, $flagged ? 'flag-error' : '')
    );

    unset ($cell['path'], $cell['options']);
    $workflow_cell = longform_get_current_workflow_state($endpoint)->state;

    $rendered_cell = theme('links', array(
      'links' => $cell,
      'attributes' => $attributes
    ));

    $columns[] = array(
      'column-first' => $rendered_cell,
      /*'column-second' => theme_links($action_links),*/
      'column-third' => array(
        'data' => $workflow_cell,
        'class' => $workflow_cell == 'Approved' ? array('success') : array('warning'),
      ),
      'column-fourth' => $flag_cell
    );
  }

  $element['wrapper'][$delta]['content']['relation'] = array(
    '#theme' => 'table',
    '#header' => array(
      'Page',
      'Current State',
      '<span class="glyphicon glyphicon-flag"></span>'
    ),
    '#rows' => $columns,
  );

  return $element;
}


/*
 * Given a node ID or a node object, returns a styled link back to the document ticket
 */
function doc_manage_get_management_menu($node) {
  if (!is_object($node)) {
    $node = node_load($node);
  }

  $rid = $node->field_document_relation[LANGUAGE_NONE][0]['value'];
  $relation = (array) relation_load($rid);
  $uri = "node/" . $relation['endpoints'][LANGUAGE_NONE][0]['entity_id'];

  // Set global link options
  $options = array(
    'html' => TRUE,
    'attributes' => array(
      'class' => array('system-nav list-group-item'),
    )
  );

  // Create links. HTML for flag glyphicons is included in the configuration settings for each flag.

  $flags = array(
    'problem' => flag_create_link('html_problem_alert', $node->nid),
    'notranlsate' => flag_create_link('no_translate', $node->nid),
  );

  // Build list.
  // This is not run through the normal theming layer, which introduces tons of extra markup and is incompatible with bootstrap
  $management_links_new = '<div class="system-nav menu list-group">' . l('<span class="glyphicon glyphicon-home"></span> ' . t('Return to ticket'), $uri, $options) . l('<span class="glyphicon glyphicon-edit"></span> ' . t('Edit this page'), "node/" . $node->nid . "/edit", $options);

  foreach ($flags as $flag) {
    $management_links_new .= $flag;
  }

  $management_links_new .= '</div>';

  return $management_links_new;

}

/**
 * Provide a button that causes PDF reactor to generate, fetch, and attach a
 * PDF to the ticket.
 *
 *
 */
function doc_manage_pdfreactor_button_form($form, &$form_state) {
  $this_node = menu_get_object();

  // Put the current node into the form_state['sandbox'] array -- ensures it is available to potential ajax requests
  $form_state['sandbox']['node'] = $this_node;

  //  Build form
  $form = array();
  //$form['pdfreactor'] = array(
  //  '#type' => 'fieldset',
  //  '#title' => 'PDF Reactor',
  //);
  $form['pdfreactor']['source'] = array(
    '#type' => 'hidden',
    '#value' => $this_node->nid,
  );
  $form['pdfreactor']['go'] = array(
    '#type' => 'submit',
    '#value' => t('Generate PDF'),
    '#submit' => array('doc_manage_pdfreactor_button_callback'),
    '#attributes' => array(
      'class' => array('btn', 'btn-primary', 'btn-block'),
    ),
  );

  return $form;
}

/**
 * Execute PDF Reactor job. Callback for doc_manage_pdfreactor_button_form().
 *
 */
function doc_manage_pdfreactor_button_callback($form, &$form_state, $source = NULL) {

  // Load the object the form was called from (this is usually the import ticket):
  if (!$source) {
    $source = $form_state['sandbox']['node'];
  }

  // Generate the PDF.
  pdfreactor_import_ticket_generate($source);
}


/**
 * Check to see if a source document has been uploaded.
 *
 * @param $ticket
 *  The loaded ticket object
 *
 * @return bool
 *  TRUE if the archive is present, FALSE otherwise
 */
function longform_ticket_source_is_attached($ticket) {
  return !(!isset($ticket->field_document_source) || empty($ticket->field_document_source));
}