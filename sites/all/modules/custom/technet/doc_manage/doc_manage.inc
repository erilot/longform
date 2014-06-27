<?php

define('WORKFLOW_PAGE_REVIEW_STATE', 13);
define('WORKFLOW_PAGE_PUBLISHED_STATE', 36);
define('WORKFLOW_PAGE_APPROVED_STATE', 14);

define('WORKFLOW_TICKET_PUBLISHED_STATE', 6);

define('WORKFLOW_TOPIC_REVIEW_STATE', 46);
define('WORKFLOW_TOPIC_APPROVED_STATE', 56);
define('WORKFLOW_TOPIC_PUBLISHED_STATE', 61);

/**
 * Display and control main ticket nav
 */
function doc_manage_render_main_nav($ticket = NULL, $opts = NULL) {

  /**
   * Panes included by default:
   *
   * Import: 	Contains the entire import-review-publish cycle, using vertical tabs. The title of this tab changes
   * 		to display the current task, so it will be one of "Import", "Review", or "Publish.
   *
   * Document: 	This is the administrative document display. Contains a link to the cover page, a copy of the TOC,
   * 		and status indicators. This pane is hidden until the import process is complete.
   *
   * Metadata: 	Displays listing of document metadata. In the future, this will include options for modifying existing documents.
   *
   * Links: 	Displays the link information for the current document.
   */

   global $user;
   
   if (!user_is_logged_in()) return;
   
  if (!$ticket) {
    $ticket = menu_get_object();
  }
  
  // Insert status summary bar at top
  
  $bar['summary'] = doc_manage_render_manage_toolbar($ticket);
  $import_states = _doc_manage_get_all_accordion_states($ticket);
  $ticket_state = workflow_node_current_state($ticket, 'node');

  // Define panes that will be included
  $panes = array(
    'import' => array('id' => 'tab-import', 'label' => ($ticket_state == 6 ? 'Publication Tasks' : array_search(TRUE, $import_states)), 'function' => 'doc_manage_render_workflow'),
    'document' => array('id' => 'tab-document', 'label' => 'Document', 'function' => 'doc_manage_render_document_pane'),
    'metadata' => array('id' => 'tab-metadata', 'label' => 'Metadata', 'function' => 'doc_manage_metadata_table'),
    'links' => array('id' => 'tab-links', 'label' => 'Links', 'function' => 'doc_manage_display_adaptive_links'),
    'pdf-reactor' => array('id' => 'tab-pdf-reactor', 'label' => 'PDF Reactor', 'function' => 'doc_manage_render_pdf_options'),
    );
  
  // Remove pre-import panes
  if ($import_states['Import']) {
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
  
  if(workflow_node_current_state($ticket) == 6) {
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
 * Block display of document metadata. Just looks nicer than a panels layout version.
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
    'machine_name' => 'taxonomy_vocabulary_8',
    'missing_tooltip' => NULL,
    'present_tooltip' => NULL,
    'importance' => 'required',
    'editable' => TRUE,
  );
  $fields[] = array(
    'label' => t('Product(s)'),
    'type' => 'term',
    'machine_name' => 'taxonomy_vocabulary_12',
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
    'machine_name' => 'field_part_number',
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

  foreach($fields as $field) {
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
  
  if($options['meta_only']) {
    return $form['meta'];
  }
  
  $wrap['body']['left']['content'] = $form['meta'];
  
  
  // *** Files! ***
  
  $files[] = array(
    'label' => t('Document archive'),
    'type' => 'file',
    'machine_name' => 'field_document_archive',
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
  
  foreach($files as $file) {
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
 * 	The entity metadata wrapper object for this import ticket.
 * 	
 * @param $options
 * 	An array of variables 
 * @return: A table row render array.
 * 
 */
function _doc_manage_build_report_row($ticket, $vars) {
  
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
  if(isset($wrapper->$field_machine_name)) {
    $field = $wrapper->$field_machine_name->value();    
  }
  
  
  //dpm($field_machine_name,'machine name');
  // If it's not set, stripe "none" value and return.
  if(!isset($field) || (is_array($field) && count($field) == 0)) {
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
      if(is_array($field) && $type != 'file') {
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
	  $value = l(
	    '<span class="glyphicon glyphicon-download"></span> ' .
	    str_ireplace('.' . array_pop(explode('/',$field['filemime'])),'',$field['filename']),
	      file_create_url($field['uri']),
	      array('html' => TRUE, 'attributes' => array('class' => array ('btn','btn-sm','btn-default', 'btn-block')))
	  );
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
      
      foreach($field as $item) {
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
	$value = theme_item_list(array('items' => $list_items, 'title' => NULL, 'type' => 'ul', 'attributes' => array()));
	$tooltip = $present_tooltip ? $present_tooltip : NULL;
      }
    }
  }

  // Base row render array
  $row = array(
    'data' => array('<b>' . $label . '</b>', $value),
    'class' => array($row_class),
  );

  // *** Work in progress ***
  //
  // For authorized users, present an "edit" button in a third table cell. Clicking the button AJAX-loads the field form and displays it
  // either in a dropdown or a modal.
  //
  // Challenges:
  //  	* Some fields use Hierarchical Select, which is itself AJAXified, and JS must be manually fired for it
  //    * Demand-loading forms and triggering modals at the same time is tricky
  //    * Could load all forms at once (this has been tried successfully), but Hierarchical Select has difficulty if more than one instance
  //      is on the page at the same time. Figuring this out may be easier than figuring out the rest.
  
//  // If alterations are allowed, add form element
//  if ($editable) {
//    //$change_form = drupal_get_form('doc_manage_generic_field_form_' . $field_machine_name, $field_machine_name, $ticket);
//    $change_form = drupal_get_form('doc_manage_fetch_field_form_' . $field_machine_name, $field_machine_name, $ticket);
//    
//    $insert['data']['change'] = array(
//      '#type' => 'container',
//      '#attributes' => array(
//	'class' => array('row-form'),
//      ),
//    );
//    
//    $insert['data']['change']['trigger'] = array(
//      '#type' => 'submit',
//      '#attributes' => array(
//	'class' => array('btn', 'btn-sm', 'btn-default'),
//	'data-toggle' => 'modal',
//	'data-target' => '#modal-' . $field_machine_name,
//      ),
//      '#value' => t('Change'),
//      '#ajax' => array(
//	'callback' => array('doc_manage_fetch_field_return'),
//      ),
//    );
//    
//    $insert['data']['change']['modal'] = array(
//      '#type' => 'container',
//      '#attributes' => array(
//	'class' => array('modal', 'fade'),
//	'tabindex' => -1,
//	'role' => 'dialog',
//	'aria-hidden' => 'true',
//	'id' => 'modal-' . $field_machine_name,
//      ),
//    );
//    $insert['data']['change']['modal']['dialog'] = array(
//      '#type' => 'container',
//      '#attributes' => array(
//	'class' => array('modal-dialog'),
//      ),
//    );
//    $insert['data']['change']['modal']['dialog']['content'] = array(
//      '#type' => 'container',
//      '#attributes' => array(
//	'class' => array('modal-content'),
//      ),
//    );
//    $insert['data']['change']['modal']['dialog']['content']['head'] = array(
//      '#type' => 'container',
//      '#attributes' => array(
//	'class' => array('modal-header'),
//      ),
//    );
//    $insert['data']['change']['modal']['dialog']['content']['head']['content'] = array(
//      '#markup' => '<h4 class="modal-title">Edit ' . $label . '</h4>',
//    );
//    
//    $insert['data']['change']['modal']['dialog']['content']['body'] = array(
//      '#type' => 'container',
//      '#attributes' => array(
//	'class' => array('modal-body'),
//      ),
//    );
//   $insert['data']['change']['modal']['dialog']['content']['body']['content'] = $change_form;
//  }
//  else {
//    $insert['data'] = array(
//      '#markup' => '<span class="text-danger btn btn-sm disabled">Locked</button>',
//    );
//  }
//  
//    
//  //$insert['data']['change']['dropdown_container']['form'] = $change_form;
//  
//  $row['data'][] = $insert;
  
  // If tooltip is present, tack on the bootstrap options
  if($tooltip) {
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
function doc_manage_fetch_field_form($form, &$form_state, $field_machine_name, $ticket){

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
  dpm($form_state,'form_state');
  return $form;
}



/**
 * Generate form for specified field.
 *
 */
function doc_manage_generic_field_form($form, &$form_state, $field_machine_name, $ticket) {

    if($form_state['sandbox']['node']) {
      $ticket = $form_state['sandbox']['node'];
    }
    else{
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
	      $form_state
	    );
    //$form[$field_machine_name]['#attributes']['name'] = 'form-name-' . $field_machine_name;
    
    // Remove description 
    //unset ($form['field_document_archive'][LANGUAGE_NONE][0]['#description']);
    
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
    '#attributes' => array(
      //'class' => array('invisible',)
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

function _doc_manage_generic_field_submit ($form, &$form_state, $ticket = NULL, $field_machine_name = NULL) {
  dpm($form_state,'form_state');
  $field = reset(reset($form_state['field']));
  $field_machine_name = $field['field']['field_name'];

  //dpm($field,'field');
  dpm($field_machine_name,'field machine name');
  global $_SERVER;
  dpm($_POST,'post');
  return $form;

  $new_fid = $form_state['values']['field_document_archive'][LANGUAGE_NONE][0]['fid'];
  
  $node = $form_state['sandbox']['node'];
  
  $wrapper = entity_metadata_wrapper('node',$node);
  
  $original_file = $wrapper->field_document_archive;
  $file = $original_file->value();
  $old_fid = $file['fid'];
    
  if ($new_fid == 0 & !isset($file)) {
    drupal_set_message('No file was uploaded.', 'error');
    return FALSE;
  }
  
  if ($new_fid == 0 && isset($file)) {

    // Remove the file.
    file_delete(file_load($old_fid));
    $wrapper->field_document_archive->set(NULL);
    $wrapper->save(TRUE);
    field_attach_update('node', $node);
    $message .= '<li>The existing file attachment has been deleted.</li>';
    
  }
  else{
    
    // A new file is here!
    
    if($old_fid != 0) {
      // First, remove the old one
      $wrapper->field_document_archive->set(NULL);
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
    $wrapper->field_document_archive->set($new_file_array);
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
function doc_manage_display_adaptive_links($ticket){
  
  $controller = menu_get_object();
  $mode = 'in';
  $title = 'Document Links';
  $target = 'document-links';
  $id = $target;
  
  if($controller->type != 'import_ticket' || !module_exists('doc_redirect')) return FALSE;
  
  $links = doc_redirect_get_adaptive_permalink();

  // Build release-specific link
  if($links['override']){
    if($controller->taxonomy_vocabulary_12 && $controller->taxonomy_vocabulary_8){
      $specific = '<p class="text-success">' . $links['override'] . '</p>';
    }
    else{
      $specific = '<p class="text-warning">A release-specific link will be available once the <b>document type</b> and <b>product type</b> have been assigned.</p>';      
    }
  }
  else{
    $specific = '<p class="text-warning">This document has not been tagged with a <b>software version</b>, so no release-specific link is available.</p>';
  }

  // Build adaptive link
  $adaptive = ($controller->taxonomy_vocabulary_12 && $controller->taxonomy_vocabulary_8)
      ? '<p class="text-success">' . $links['adaptive']. '</p>'
      : '<p class="text-warning">An adaptive link will be available once the <b>document type</b> and <b>product type</b> have been assigned.</p>';
      
  // Build canonical link
  $canonical = ($links['cover'])
    ? '<p class="text-success">' . $links['cover'] . '</p>'
    : '<p class="text-warning">A canonical link to the cover page will be available once the archive has been imported, converted, and saved.</p>';
  
  $table = array(
    '#theme' => 'table',
    '#header' => array('Type','Link'),
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
	'title' => t(
	  'Use the <b>Canonical</b> link if there is no release-specific link available'),
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
  
  if (!$collapsible) {
    $render['head'] = array(
      '#markup' => '<h2>'  . t($title) . '</h2>',
    );
  }
  
  $render['panel'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel','panel-default'),
    ),
  );

  // If in collapsing mode, render this as a triggering link.
  
  if ($collapsible) {
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
    $heading = l(
	       '<span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' . t($title) ,
	       '#',
		$link_attrs
	      );
  
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
 * NEW
 * Build and render importing workflow panels, with associated forms.
 *
 * Renders as tabbed nav panels.
 * 
 */
function doc_manage_render_workflow($ticket = NULL){

  if (!$ticket) {
    $ticket = menu_get_object();
  }
  if(!$ticket) return;
  
  $states = _doc_manage_get_import_state($ticket);
  
  // Check to see if we're ready to move on to publishing. To move forward, all book pages must be in either state 14 (approved) or 36 (published).
  $counts = doc_manage_workflow_entity_counts($ticket, 'book');
  $ready_to_publish = ($counts[14] + $counts[36] == $counts['total'] && $states['post'] == 'finished') ? TRUE : FALSE;  
  
  // The document may already be published; set a flag if it is
  $published = workflow_node_current_state($ticket) == 6 ? TRUE : FALSE;
  
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
  if($published) {
    $out['head']['alert'] = array(
      '#type' => 'container',
      '#attributes' => array(
	'class' => array('alert', ($published ? 'alert-success' : 'alert-info')),
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
  
  $import_tab_pair = _get_tab_pair('tab-task-import', _doc_manage_accordion_state(1, $states, $ready_to_publish) ? TRUE : FALSE, 'pill', 'Import');
  $tabs['tab-task-import'] = $import_tab_pair['tab'];
  $out['contents']['import'] = $import_tab_pair['wrapper'];
  $out['contents']['import']['tab-task-import']['contents'] = _doc_manage_render_import_panel($ticket, $states, $counts, $ready_to_publish, $published);

  $review_tab_pair = _get_tab_pair('tab-task-review', _doc_manage_accordion_state(2, $states, $ready_to_publish) ? TRUE : FALSE, 'pill', 'Review');
  $tabs['tab-task-review'] = $review_tab_pair['tab'];
  $out['contents']['review'] = $review_tab_pair['wrapper']; 
  $out['contents']['review']['tab-task-review']['contents'] = _doc_manage_render_review_panel($ticket, $states, $counts, $ready_to_publish, $published);
  
  $publish_tab_pair = _get_tab_pair('tab-task-publish', _doc_manage_accordion_state(3, $states, $ready_to_publish) ? TRUE : FALSE, 'pill', 'Publish');
  $tabs['tab-task-publish'] = $publish_tab_pair['tab'];
  $out['contents']['publish'] = $publish_tab_pair['wrapper']; 
  $out['contents']['publish']['tab-task-publish']['contents'] = _doc_manage_render_publish_panel($ticket, $states, $counts, $ready_to_publish, $published);


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
  $active = strtolower(array_search(TRUE, _doc_manage_get_all_accordion_states($ticket)));
  
  $out['bar']['panel']['body']['tabs']['#items']['tab-task-' . $active]['class'] = array('active');
  
  return $out; 
 
}

/**
 * Helper function that provides a matched tab and pane element.
 *
 * @param $id
 * 	The CSS ID to use. Do not include the '#'.
 *
 * @param $state
 * 	Boolean. Whether or not the pair should start in/active or closed/inactive.
 *
 * @param $type
 *  	One of "pill" or "tab".
 *
 * @param $label
 * 	The text label for the tab. 
 *
 * @return matched pair as a keyed array
 * 
 */
function _get_tab_pair($id, $state, $type, $label) {
  
  return array(
	  'wrapper' => _render_workflow_tab_wrapper($id, $state, $type),
	  'tab' => array('data' => '<a href="#' . $id . '" data-toggle="' . $type . '">' . $label . '</a>'),
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

  switch($ready_to_publish){
  
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
	  '#markup' => '<label>Before publishing, review the following warnings:</label><ul>' . implode(PHP_EOL,$warns) . '</ul><br/><p>To review all document metadata, click the <b>Metadata</b> tab, above.</p>',
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
	  <div class="alert"><label>PDF Mode</label><br/>'
	. flag_create_link('pdf_reactor_mode', $ticket->nid)
	. '<div class="alert text-muted"><p>If enabled, Technet will generate a PDF of this document automatically. Click to toggle.</p></div>'
	. '</div>';
	
      $pdf_sync =
	  '<div class="alert"><label>PDF Sync Mode</label><br/>'
	. flag_create_link('pdf_reactor_live_sync', $ticket->nid)    
	. '<div class="alert text-muted">If enabled, Technet will automatically re-generate PDFs when document content is changed. Click to toggle.<br/> <i class="text-warning">This setting has no effect if <b>pdf mode</b> is off.</i></div>'
	.'</div>';

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
 * Helper function that returns an array of HTML list elements with metadata warnings.
 *
 * @var $ticket
 * 	The ticket object to check.
 *
 * @return array
 * 	An array of <li> elements with relevant warnings.
 */
function _doc_manage_metadata_warnings($ticket) {
  $wrapper = entity_metadata_wrapper('node', $ticket);

  if ((!isset($wrapper->field_auto_pdf) || count($wrapper->field_auto_pdf->value()) == 0) && (!isset($wrapper->upload) || count($wrapper->upload->value()) == 0)) {
    $warns[] = '<li>' . t('<b>No PDFs have been uploaded</b>, and PDF Reactor is not set to automatically generate one.') . '</li>';
  }
    //dpm($wrapper->field_software_release->value(),'value test');

  if (!isset($wrapper->field_software_release) || count($wrapper->field_software_release->value()) == 0) {
    $warns[] = '<li>' . t('<b>No software release</b> has been specified.') . '</li>';
  }
  if (!isset($wrapper->taxonomy_vocabulary_12) || count($wrapper->taxonomy_vocabulary_12->value()) == 0) {
    $warns[] = '<li>' . t('<b>No product/device type</b> has been specified.') . '</li>';
  }
  if (!isset($wrapper->field_part_number) || count($wrapper->field_part_number->value()) == 0) {
    $warns[] = '<li>' . t('<b>No part number</b> has been specified.') . '</li>';	
  }
  if (isset($wrapper->field_platform) && count($wrapper->field_platform->value()) == 1) {
    $warns[] = '<li>' . t('A <b>Platform</b> has been specified. This should only be done when multiple platform version exist; if there is only one (for example, if the only version is for Windows), the tag should be removed.') . '</li>';	
  }

  return $warns;  
}

function _doc_manage_render_review_panel($ticket, $states, $counts, $ready_to_publish, $published) {
  // *** Review panel ***

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
  
  switch($states['post']) {
    
    // Postprocessing is done; display the review forms and tables.
    case 'finished':
      
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

      // Add the workflow actions form
      $review_panel['panel']['row']['right']['form'] = drupal_get_form('doc_manage_bulk_workflow_actions');
      
      // Display a context-aware help message telling the user where they are and what to do next.
      if(!$ready_to_publish) {
	$review_panel['panel']['row']['left']['alert'] = array(
	  '#type' => 'container',
	  '#attributes' => array(
	    'class' => array('alert', 'alert-info'),
	  ),
	);
	$review_panel['panel']['row']['left']['alert']['msg'] = array(
	  '#markup' =>'
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
	  '#markup' =>'
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

function _doc_manage_render_import_panel($ticket, $states, $counts, $ready_to_publish, $published){
  
  // Import settings subpanel
  $import_settings = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-xs-3', 'right-col', /*'well', 'well-sm'*/),
    ),
  );
  
  $import_settings['header'] = array(
    '#markup' => '<h3 class="text-warning">Importing Options</h3>',
  );
  
  // Import settings: file upload widget
  $import_settings['upload'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('well','well-sm','frosted-light'),
      'data-toggle' => 'tooltip',
      'data-trigger' => 'hover',
      'data-placement' => 'right auto',
      'data-html' => TRUE,
      'title' => "Upload (or remove and re-upload) a document archive",
    ),
  );
  
  $archive_upload_form = ($states['unpack'] == 'pending' || $states['unpack'] == 'next') ? drupal_get_form('_doc_manage_upload_archive_form') : array('#markup' => '<label>Document Archive</label><br/><div class="alert alert-warning">The uploaded archive is now unpacked and cannot be changed.</div>');
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
  
  // Include split level form if the document hasn't been split yet. If it has, include a status message instead.
  if ($states['save'] == 'pending' || $states['save'] == 'next') {
    $heading_split_form = drupal_get_form('_doc_manage_heading_split_form');
  }
  else {
    $level = taxonomy_term_load($ticket->field_heading_split_level[LANGUAGE_NONE][0]['tid']);
    
    $heading_split_form = array(
      '#markup' => '<label>Heading Split Level</label><br/>
		    <div class="alert alert-warning">The document has already been split at <b>' . $level->name . '</b>.</div>',);
  }
  
  $import_settings['split']['split-form'] = $heading_split_form;
  
  // Assemble import panel  

  $import_panel = doc_manage_render_import_actions_panel($ticket);
  //$import_panel['panel']['row']['right'] = $import_settings;
  
  return $import_panel;
}

/**
 * Returns the wrapper for an accordion element.
 *
 * 	
 * @param $id
 * 	The CSS ID of the element, WITHOUT the preceding '#'.
 *
 * @param $state
 * 	Boolean. Determines whether the element is open or closed (TRUE == open).
 *
 * @param $type
 * 	One of "pill" or "tab".
 *
 * @return a render array with the appropriate wrappers for an empty accordion element.
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
function doc_manage_display_published_toc($ticket){
  
  
  // Get document's TOC
  $toc = drupal_json_decode($ticket->field_toc_array[LANGUAGE_NONE][0]['value'], TRUE);

  // Set up wrappers and regions

  // Right region
  // Use the 'no-pad' class override to drop all padding from the fieldset.
  $wrapper['row']['right'] = array(
    '#type' => 'fieldset',
    '#title' => t('Document TOC'),
    '#attributes' => array(
      'class' => array ('logo-behind', 'no-pad'),
    ),
  );
  
  $wrapper['row']['right']['content'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('well', 'well-sm', 'frosted-light'),
    ),
  );
  $wrapper['row']['right']['content']['inner']= array(
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
 * 	The ticket object.
 *
 * @return HTML render array
 */
function doc_manage_render_document_status($ticket) {
  
  $links = doc_redirect_get_adaptive_permalink();
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
    '#markup' => l(t('Jump to the cover page'),
		   $links['alias'],
		   array('html' => TRUE,
			 'attributes' => array(
			    'class' => array('btn', 'btn-primary', 'btn-lg', 'btn-block'),
			    'data-toggle' => 'tooltip',
			    'data-placement' => 'right auto',
			    'data-trigger' => 'hover',
			    'title' => t('Go directly to this document\'s cover page'),
			    ),
			)
		   )
  );
  
  $relation = doc_manage_get_book_pages($ticket);
  $head['actions']['bulk_edit'] = array(
    '#markup' => l(t('Bulk edit metadata'),
		   'bulk-edit/' . $relation[0],
		   array('html' => TRUE,
			 'attributes' => array(
			    'class' => array('btn', 'btn-default', 'btn-block'),
			    'data-toggle' => 'tooltip',
			    'data-placement' => 'right auto',
			    'data-trigger' => 'hover',
			    'title' => t('Modify the metadata values for the pages in this book'),
			    ),
			 )
		   ),
  );
  
  $links = doc_redirect_get_adaptive_permalink();
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
      'class' => array('btn', 'disabled', $published ? 'btn-success' : 'btn-danger'),
    ),
  );
  $actions['published'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => 'btn-group',
    ),
  );

  // PDF status
  if(isset($m->upload)) {
    $man_pdf = reset($m->upload->value());    
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
      '#markup' => l('<span class="glyphicon glyphicon-download"></span>', file_create_url($man_pdf['uri']),
	array('html' => TRUE,
	      'attributes' => array(
		'class' => array ('btn','btn-default'),
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
  if(isset($m->field_auto_pdf)) {
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
      '#markup' => l('<span class="glyphicon glyphicon-download"></span>', file_create_url($auto_pdf['uri']),
	array('html' => TRUE,
	      'attributes' => array(
		'class' => array ('btn','btn-default'),
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
  if(isset($m->field_release_state)) {
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
  
  foreach($statii as $key => $status) {
    $rows[] = array(array('data' => render($status)), array('data' => render($actions[$key])));
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
 * Display a document status summary bar.
 *
 * @param $ticket
 * 	The ticket object. If not supplied, the current object is loaded.
 *
 * @return HTML render array
 */
function doc_manage_render_manage_toolbar($ticket = NULL) {
  module_load_include('inc','doc_manage');
  module_load_include('inc','doc_manage', 'doc_manage.relation');

  // @TODO: Make this access check dependent on group membership, not just a plain logged-in check
  if (!user_is_logged_in()) return;
  
  if (!$ticket) {
    $ticket = menu_get_object();
  }
  $type = $ticket->type;  

  $book = doc_manage_relation_query('ticket_controls_nodes', $ticket);
  $parent = array_shift($book);
  $cover = array_shift($book);
    
  $stages = _doc_manage_get_all_accordion_states($ticket, $book);

  $task_label = '<b>' . array_search(TRUE, $stages) . '</b>';
  $task_mode = 'btn-info';
  
  // Modify results in two instances: document stub, and already published.
  if ($wf_state == WORKFLOW_TICKET_PUBLISHED_STATE) {
   $task_label = '<b>None!</b>';
   $task_mode = 'btn-success';
  }
  
  if ($wf_state == 21 && $states['unpack'] == 'pending') {
   $task_label = '<b>Upload File</b>';
   $task_mode = 'btn-danger';
  }
  
  $wrap = entity_metadata_wrapper('node', $ticket);
  if ($type == 'book') {
    $relation = isset($wrap->field_document_relation) ? relation_load($wrap->field_document_relation->value()) : FALSE;
    $parent = $relation->endpoints[LANGUAGE_NONE][0]['entity_id'];
  }
  
  $render = array(
   '#type' => 'container',
   '#prefix' => '<nav class="navbar navbar-default" role="navigation" id="doc-manage-toolbar">',
   '#suffix' => '</nav>',
   '#attributes' => array(
    'class' => 'container-fluid',
   ),
  );
  
  //$render['context'] = array(
  // '#markup' => '<span class="navbar-brand">Manage</span>',
  // );
  
  $render['opts'] = array(
   '#type' => 'container',
   '#attributes' => array(
    'class' => array('collapse', 'navbar-collapse'),
    'id' => 'doc-manage-toolbar',
   ),
  );

  $items[] = _doc_manage_toolbar_button_jump_to($ticket);	// "jump to" button
  
  $items[] = _doc_manage_toolbar_button_edit($ticket);		// "edit" button
  
  $items[] = _doc_manage_toolbar_button_flag_html($ticket);	// "html flag" button
  
  $items[] = _doc_manage_toolbar_button_approve($ticket); // "approve" form
  
  $items[] = drupal_get_form('_doc_manage_toolbar_button_approve_advance', $ticket); // "approve and next" form
  
  
  


  $render['opts']['list'] = array(
    '#theme' => 'item_list',
    '#items' => $items,
    '#attributes' => array(
      'class' => array('nav', 'navbar-nav'),
    ),
  );
  
  $render['opts']['status'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('btn-toolbar', 'navbar-btn', 'navbar-right'),
    ),
  );
  
  $render['opts']['status']['pdf_status'] = array(
   '#type' => 'container',
   '#attributes' => array(
    'class' => array('btn-group'),
   ),
  );

  $render['opts']['status']['pdf_status']['content']['label'] = array(
   '#type' => 'button',
   '#value' => t('PDF available'),
   '#attributes' => array(
    'class' => array('btn','btn-default','disabled'),
   ),
  );

  // PDF Status
  $pdf_ok = (isset($wrap->upload) && count($wrap->upload) > 0) || (isset($wrap->field_auto_pdf) && count($wrap->field_auto_pdf) > 0);
	     
  $render['opts']['status']['pdf_status']['content']['value'] = array(
   '#type' => 'button',
   '#value' => $pdf_ok ? t('Yes') : t('No'),
   '#attributes' => array(
    'class' => array('btn','disabled', $pdf_ok ? 'btn-success' : 'btn-danger'),
   ),
  );
  
  // Publish status
  
  // Get status of page
  $wf_state = workflow_node_current_state($ticket);
  $publish_mode = $wf_state == WORKFLOW_PAGE_PUBLISHED_STATE || $wf_state == WORKFLOW_TICKET_PUBLISHED_STATE ? 'btn-success' : 'btn-danger';
  $publish_label = $wf_state == WORKFLOW_PAGE_PUBLISHED_STATE || $wf_state == WORKFLOW_TICKET_PUBLISHED_STATE ? 'Published' : 'Unpublished';

  $render['opts']['status']['publish_status'] = array(
   '#type' => 'container',
   '#attributes' => array(
    'class' => array('btn-group'),
   ),
  );

  $render['opts']['status']['publish_status']['content']['label'] = array(
   '#type' => 'button',
   '#value' => t('Publication status'),
   '#attributes' => array(
    'class' => array('btn','btn-default','disabled'),
   ),
  );

  $render['opts']['status']['publish_status']['content']['value'] = array(
   '#type' => 'button',
   '#value' => $publish_label,
   '#attributes' => array(
    'class' => array('btn','disabled', $publish_mode),
   ),
  );
  
  return $render;
 
}

function _doc_manage_toolbar_button_jump_to($node) {

  $wrap = entity_metadata_wrapper('node', $node);
  
  // Go to Book/Return to ticket icon
  switch ($node->type) {

    case 'import_ticket':
    
    // For import tickets, this icon links to the cover page of the document. It's only available after postprocessing.
    
    // Get full book relation
    $endpoints =  doc_manage_relation_query('ticket_controls_nodes', $node);
    $ticket = array_shift($endpoints);
    $cover = array_shift($endpoints);
    $stages = _doc_manage_get_all_accordion_states($node);
    $tooltip = array(
      'data-toggle' => 'tooltip',
      'data-trigger' => 'hover',
      'data-placement' => 'top',
      'title' => !$stages['Import'] ? t('Go to the document') : t('Document pages haven\'t been created yet'),
    );
    
    $button = array(
	      'data' => l(
		'<span class="glyphicon glyphicon-arrow-right"></span>Cover page',
		(isset($cover['entity_id']) ? 'node/' . $cover['entity_id'] : ''),
		array(
		  'html' => TRUE,
		  'attributes' => array(
		    'class' => array($stages['Import'] ? 'disabled':'enabled'),
		  ))),
	      ) + $tooltip;
      
      break;
    
    case 'book':

      // For books, this icon sends users back to the import ticket.
      
    $ticket = doc_manage_get_ticket_from_page($node);
    $button = l(
      '<span class="glyphicon glyphicon-arrow-left"></span>' . t('Ticket'),
      'node/' . $ticket,
      array(
	'html' => TRUE,
	'attributes' => array(
	  'class' => array(),
	'data-toggle' => 'tooltip',
	'data-trigger' => 'hover',
	'data-placement' => 'top',
	'title' => t('Return to ticket'),
	)));
    break;
  }

  return $button;
}

function _doc_manage_toolbar_button_edit($node) {
  $button = l(
    '<span class="glyphicon glyphicon-edit"></span>' . ($node->type == 'book' ? t('Edit this page') : t('Edit Metadata')),
    'node/' . $node->nid . '/edit',
    array(
      'html' => TRUE,
      'attributes' => array(
	'class' => array(),
      'data-toggle' => 'tooltip',
      'data-trigger' => 'hover',
      'data-placement' => 'top',
      'title' => t('Edit this page'),
      )));

  return $button;
}

function _doc_manage_toolbar_button_flag_html($node) {
  switch ($node->type) {
  
    case 'book':
      
      // HTML Help Flag
      $help_flag = flag_get_flag('html_problem_alert');
      $help_status_class = $help_flag->is_flagged($node->nid) ? ' text-danger' : '';
      
      $flag = array(
	'#type' => 'container',
	'#attributes' => array(
	  'class' => array('dropdown-menu', 'prevent-close'),
	  ),
	);
      $flag['flag']['alert'] = array(
	'#type' => 'container',
	'#attributes' => array(
	  'class' => array(),
	  ),
	);
      $flag['flag']['alert']['message'] = array(
	'#markup' => 'If there is an HTML problem on this page, click this button to set an alert.<br/><br/>',
	);
      $flag['flag']['help'] = array(
	'#markup' => flag_create_link('html_problem_alert', $node->nid),
	);

      $button = array(
	'data' => '<a href="#" class="dropdown-toggle' . $help_status_class . '" data-toggle="dropdown"><span class="glyphicon glyphicon-flag"></span>Flag HTML</a>' . render($flag),
	'class' => array('flag-set', 'keep-alive'),
	'data-toggle' => 'tooltip',
	'data-placement' => 'top',
	'data-trigger' => 'hover',
	'title' => t('Flag an HTML problem on this page'),
      );
      break;
  }    
  return $button;
  
}

function _doc_manage_toolbar_button_approve($node) {
  // Approval button
  
  switch ($node->type){
    case 'book':
      $wf_choices = workflow_field_choices($node);
      $wf_state = workflow_node_current_state($node);
      $wf_label = $wf_choices[$wf_state];
      $states = _doc_manage_get_import_state($node);
    
      $published_state = $node->type == 'import_ticket' ? 6 : 36;
      
      // Get highest state the current user can move to
      $state = workflow_get_workflow_node_by_nid($node->nid);
      if (array_key_exists(36, $wf_choices)) {
	array_pop($wf_choices);
      }
      
      end($wf_choices);
      $highest_workflow_shift = key($wf_choices);
      
      $label = array(
	14 => t('Approve'),
	16 => t('Submit'),
	36 => t('Publish'),
      );
            
      $approve = array(
	'#type' => 'container',
	'#attributes' => array(
	  'class' => array('dropdown-menu', 'prevent-close'),
	  ),
	);
      $approve['approve']['alert'] = array(
	'#type' => 'container',
	'#attributes' => array(
	  'class' => array(),
	  ),
	);
      if ($state->sid != 14 && $state->sid != 36) {
	$approve['approve']['alert']['message'] = array(
	  '#markup' => '<p>To ' . strtolower($label[$highest_workflow_shift]) . ' this content, click
	    <button class="btn btn-sm disabled"><span class="glyphicon glyphicon-ok"></span></button>.
	    To approve and jump to the next unapproved page, click
	    <button class="btn btn-sm disabled"><span class="glyphicon glyphicon-ok"></span>& <span class="glyphicon glyphicon-arrow-right"></span></button>.
	    </p><br/>', //' . $label[$highest_workflow_shift] . '
	  );
	$approve['approve']['form'] = drupal_get_form('doc_manage_approve_content_form', $node, $label[$highest_workflow_shift], $highest_workflow_shift);
      }
      else {
	$approved = TRUE;
	$approve['approve']['alert']['message'] = array(
	  '#markup' => 'This content has been ' . strtolower($wf_choices[$highest_workflow_shift]) . ', and the workflow state can no longer be changed.',
	  );	
      }
      $button = array(
	'data' => '<a href="#" class="dropdown-toggle' . ($approved ? ' text-success' : '' ). '" data-toggle="dropdown"><span class="glyphicon glyphicon-ok"></span>' . ($approved ? t('Approved') : t('Approve')). '</a>' . render($approve),
	'class' => array('flag-set', 'keep-alive'),
	'data-toggle' => 'tooltip',
	'data-placement' => 'top',
	'data-trigger' => 'hover',
	'title' => $approved ? t('This content has already been approved') : t('Approve this content for publication'),
      );
      break;
  }
  return $button;
}

function doc_manage_approve_content_form($form, &$form_state, $node, $label, $target) {
  $next = doc_manage_get_next_unapproved_page($node);
  $form = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(''),
    ),
  );
  
  $form['comment'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter a comment (optional)'),
  );
  $form['nid'] = array(
    '#type' => 'hidden',
    '#name' => 'nid',
    '#value' => $node->nid,
  );
  $form['target'] = array(
    '#type' => 'hidden',
    '#name' => 'target',
    '#value' => $target,
  );
  $form['next'] = array(
    '#type' => 'hidden',
    '#name' => 'target',
    '#value' => $next,
  );
  $form['approve_container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('btn-group', 'pull-right'),
    ),
  );
  $form['approve_container']['approve'] = array(
    '#type' => 'submit',
    '#value' => '<span class="glyphicon glyphicon-ok"></span>'/*<br/>Approve'*/,
    '#attributes' => array(
      'class' => array('btn-success'),
      'data-toggle' => 'tooltip',
      'data-placement' => 'bottom auto',
      'data-trigger' => 'hover',
      'title' => 'Approve and stay on this page',
    ),
    '#submit' => array('doc_manage_approve_content_submit'),
  );
  if ($next) {
    $form['approve_container']['approve_next'] = array(
      '#type' => 'submit',
      '#value' => '<span class="glyphicon glyphicon-ok"></span> & <span class="glyphicon glyphicon-arrow-right"></span>'/*<br/>Approve & next'*/,
      '#attributes' => array(
	'class' => array('btn-success'),
	'data-toggle' => 'tooltip',
	'data-placement' => 'bottom auto',
	'data-trigger' => 'hover',
	'title' => 'Approve and jump to the next unapproved page',
      ),
      '#submit' => array('doc_manage_approve_content_submit_next'),
    );    
  }
  else {
  $form['approve_container']['approve_ticket'] = array(
    '#type' => 'submit',
    '#value' => '<span class="glyphicon glyphicon-ok"></span> & <span class="glyphicon glyphicon-arrow-left"></span>'/*<br/>Approve & return'*/,
    '#attributes' => array(
      'class' => array('btn-success'),
      'data-toggle' => 'tooltip',
      'data-placement' => 'bottom auto',
      'data-trigger' => 'hover',
      'title' => 'Approve and jump back to the import ticket (this is the last unapproved page)',
    ),
    '#submit' => array('doc_manage_approve_content_submit_return'),
  );

  }
  
  return $form;
}

function doc_manage_approve_content_submit_return($form, &$form_state, $opts) {
  $opts['return'] = TRUE;
  doc_manage_approve_content_submit($form, $form_state, $opts);
}

function doc_manage_approve_content_submit_next($form, &$form_state, $opts) {
  $opts['next'] = TRUE;
  doc_manage_approve_content_submit($form, $form_state, $opts);
}

function doc_manage_approve_content_submit($form, &$form_state, $opts) {
  
  $target = $form_state['values']['target'];
  $label = $form_state['values']['label'];
  $comment = $form_state['values']['comment'];
  $node = node_load($form_state['values']['nid']);
  $next = $form_state['values']['next'];

  workflow_transition($node, WORKFLOW_PAGE_APPROVED_STATE, TRUE);
  
  drupal_set_message('Workflow state updated.');
  
  // Approving a page approves all included topics.

  doc_manage_approve_topics($node);
  
  if (!$opts) {
    return $form;
  }
  else {
    if ($opts['next']) {
      drupal_goto('node/' . $next);
    }
    else if ($opts['return']) {
      drupal_goto('node/' . doc_manage_get_ticket_from_page($node));
    }
  }
  
  //development
  $commands[] = ajax_command_replace('#doc-manage-toolbar', doc_manage_render_manage_toolbar($node));
  return array('#type' => 'ajax', '#commands' => $commands);
}


/**
 * Approve all topics belonging to a given page node.
 *
 * var $node
 * 	The book page node to query
 *
 * return array
 * 	Assoc array of topic NIDs and their ending workflow states.
 */
function doc_manage_approve_topics($node) {
  
  $page_topics = doc_manage_get_page_topics($node);
    
  $topics = node_load_multiple(array_flip($page_topics));

  $count = 0;
  $published = 0;
  
  foreach($topics as $topic) {

    $topic_current_state = workflow_node_current_state($topic);
    $return[$topic->nid]['old_state'] = $topic_current_state;
    
    // Promote component topics to "approved" unless they already have been.
    // Do not downgrade already-published topics to 'approved', either.
    if($topic_current_state != WORKFLOW_TOPIC_APPROVED_STATE && $topic_current_state != WORKFLOW_TOPIC_PUBLISHED_STATE) {
      workflow_transition($topic, WORKFLOW_TOPIC_APPROVED_STATE, TRUE);
      entity_get_controller('node')->resetCache(array($topic->nid));
      $count++;
    }
    if($topic_current_state == WORKFLOW_TOPIC_PUBLISHED_STATE) {
      $published++;
    }
    
    $return[$topic->nid]['new_state'] = workflow_node_current_state($topic);
  }
  if($count) {
    drupal_set_message($count . ' topics on the page were also approved.');
  }
  if($published_count) {
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
 * 	The ticket node object.
 *
 * @return
 * 	Rendered HTML.
 */
function doc_manage_render_pdf_options ($ticket) {

  $out['pdf_container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array ('pdf-options'),
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
  
  $out['pdf_container']['right']['actions_wrapper']['generate_pdf_intro']['force_generate'] = array(
    '#markup' => drupal_render(drupal_get_form('doc_manage_pdfreactor_button_form')) ,
  );

  // If an manually-uploaded PDF is available, make it available for download.
  if (isset($ticket->upload[LANGUAGE_NONE][0])) {
    $manual_upload = TRUE;
    
    $out['pdf_container']['right']['downloads'] = array(
      '#type' => 'container',
      '#attributes' => array(
	'class' => array('well','well-sm'),
      ),
    );

    $file = $ticket->upload[LANGUAGE_NONE][0];
    $out['pdf_container']['right']['downloads']['download'] = array(
      '#markup' => l('<span class="glyphicon glyphicon-download"></span> ' . t('Download manual PDF'), file_create_url($file['uri']), array ('html' => TRUE, 'attributes' => array('class' => array('btn','btn-default','btn-block')))),
    );
  }
 
  // If an auto-generated PDF is available, make it available for download.
  if (isset($ticket->field_auto_pdf[LANGUAGE_NONE][0])) {

    if($manual_upload) {
      $out['pdf_container']['right']['downloads']['warning'] = array(
	'#markup' => '<div class="alert alert-danger"><label>' . t('Note:') . '</label></br>' . t('This PDF will not be seen by the public, because a manually-uploaded PDF exists.') . '</div>',
      );
    }
    $file = $ticket->field_auto_pdf[LANGUAGE_NONE][0];
    $out['pdf_container']['right']['downloads']['download'] = array(
      '#markup' => l('<span class="glyphicon glyphicon-download"></span> ' . t('Download auto PDF'), file_create_url($file['uri']), array ('html' => TRUE, 'attributes' => array('class' => array('btn','btn-default','btn-block')))),
    );
  }

  
  return $out;
}


/**
 * Render import process panel.
 *
 * @param $ticket
 * 	The ticket node object.
 *
 * @return
 * 	Rendered HTML.
 */
function doc_manage_render_import_actions_panel ($ticket) {
  
  $states = _doc_manage_get_import_state($ticket);
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
  if($states['post'] == 'finished') {
    // all done
    $import_steps_info['wrapper'] = array(
      '#type' => 'container',
      '#attributes' => array(
	'class' => array('alert','alert-success'),
      ),
    );
    $import_steps_info['wrapper']['content'] = array(
      '#markup' => '<label>You\'re done with this task!</label><p>There\'s nothing more to do here for now. If you need to, you can rewind the importing process by using the <button class="btn btn-sm btn-danger disabled">undo</button> buttons below.</p>'
    );
  }
  else if($states['unpack'] == 'pending') {
    
    // No archive has been uploaded yet. Provide instructions on what to do next.

    $import_steps_info['wrapper'] = array(
      '#type' => 'container',
      '#attributes' => array(
	'class' => array('alert','alert-warning', 'col-md-8'),
      ),
    );
    $import_steps_info['wrapper']['content'] = array(
      '#markup' =>'
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
    $import_steps_info['uploader']['content'] = array('#markup' => drupal_render(drupal_get_form('_doc_manage_upload_archive_form')));
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
    
    $import_steps_info['form'] = ($states['unpack'] == 'pending' ? '' : drupal_get_form('_doc_manage_import_steps_form', $ticket));

    return $import_steps_info;
}


/**
 * Helper function that determines which accordion panels are open on page display
 *
 * @param $step
 * 	The numeric step in the publishing process. 1=Import, 2=Review, 3=Publish.
 *
 * @param $states
 * 	The importing states report array
 *
 * @param $ready_to_publish
 * 	Boolean value: can publishing go?
 *
 * @return
 * 	Returns TRUE if the panel should be open.
 * 
 */
function _doc_manage_accordion_state($step, $states, $ready_to_publish = FALSE) {
  
  $ticket_state = workflow_node_current_state(menu_get_object());
  // Get importing status

  
  switch ($step) {
    case 1:
      return $states['post'] != 'finished' ? TRUE : FALSE;
      break;
    
    case 2:
      return $states['post'] == 'finished' && !$ready_to_publish ? TRUE : FALSE;
      break;
    
    case 3:
      return ($states['post'] == 'finished' && $ready_to_publish  /*&& $ticket_state != 6*/) ? TRUE : FALSE;
      break;
  }
  return $return;
}

/**
 * Helper function that returns an array of all publication task states.
 */
function _doc_manage_get_all_accordion_states ($ticket) {
  module_load_include('inc', 'doc_manage', 'doc_manage.workflow');
  $states = _doc_manage_get_import_state($ticket);
  $counts = doc_manage_workflow_entity_counts($ticket, 'book');
  $ready_to_publish = ($counts[14] + $counts[36] == $counts['total'] && $states['post'] == 'finished') ? TRUE : FALSE;
  
  $stages = array(1 => array('label' => 'Import'), 2 => array('label' => 'Review'), 3 => array('label' => 'Publish'));
  
  $return = array();
  
  foreach($stages as $key => $stage){
    $return[$stage['label']] = _doc_manage_accordion_state($key, $states, $ready_to_publish);
  }
  return $return;
}


/**
 * Provide bulk workflow state change buttons.
 */
function doc_manage_bulk_workflow_actions($form, &$form_state) {
  $node = menu_get_object();
  
  $workflow = workflow_get_workflow_type_map_by_type('book');
  $choices = workflow_field_choices($node);
  $workflow = workflow_get_workflows_by_wid($workflow->wid);
  $states = workflow_get_workflow_states_by_wid($workflow->wid);
  $current = workflow_node_current_state($node);
  
  $in_review = 13;
  $submitted = 16;
  $approved = 14;
  $published = 36;
  
  global $user;
  
  if (array_key_exists(3, $user->roles) || array_key_exists(16, $user->roles)) {
    
    $form['preface'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="alert alert-warning"><label>Bulk Assign States</label><br/><p>You can force all pages to any available state by clicking a button below.</p></div>',
    );
    
    foreach($states as $sid => $state) {
      
      // If the SID maps to "created", ignore it.
      if($sid == 11) {
	continue;
      }
      $form['buttons'][$sid] = array(
	'#type' => 'markup',
	'#prefix' => '<div>',
	'#suffix' => '</div>',
      );
      
      $form['buttons'][$sid]['target'] = array(
	'#type' => 'hidden',
	'#value' => $sid,
	'#name' => 'target-' . $sid,
      );
      
      $form['buttons'][$sid]['button'] = array(
	'#type' => 'submit',
	'#submit' => array('_doc_manage_bulk_shift_workflows'),
	'#attributes' => array(
	  'class' => array('btn-default','btn-block'),
	  'data-target' => $sid,
	),
	'#value' => $state->state,
	'#id' => 'bulk-workflow-to-' . $sid,
      );
      
      if ($sid == $current) {
	$form['buttons'][$sid]['#attributes']['class'][] = 'disabled';
      }
    }
  }
  return $form;
}

function _doc_manage_bulk_shift_workflows($form, &$form_state) {

  $target = $form_state['clicked_button']['#attributes']['data-target'];
  
  $ticket = menu_get_object();
  $book_pages = doc_manage_get_book_pages($ticket);
  doc_manage_bulk_publish($ticket, $target);
  drupal_set_message('Page states have been reset.');

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
	  'class' => array('btn-lg','btn-danger', 'btn-block'),
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
	  'class' => array('btn-lg','btn-success', 'btn-block'),
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
  $pdf_mode_set = $pdf_mode->is_flagged($node->nid);


  // @TODO: The next few statements (up to the batch) will execute even if the batch fails.
  // Move these into a new function (maybe batch finished?) to prevent bad things from happening.
  
   // Set page states to 'published'
  doc_manage_bulk_publish($ticket, 'flag');
  
  // Set ticket state to 'published'
  workflow_execute_transition($node, $published_state,'Publish button clicked', TRUE);
  entity_get_controller('node')->resetCache(array($node->nid)); 
 
  // For now, shut off PDF REACTOR. Fake the "PDF REACTOR MODE" flag to true ( == do not generate).
  $pdf_mode_set = TRUE;
  
  //set up batch
  if (!$pdf_mode_set) {
    $operations[] = array('pdfreactor_import_ticket_generate', array($node));
  }
  $operations[] = array('doc_manage_bulk_publish', array($node,'flag'));

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
  if($success){
    drupal_set_message('Publication tasks completed.');
  }
  else{
    drupal_set_message('An error occurred while publishing nodes or creating PDFs. Your PDF may not have been created and/or your pages may not be published.','error');
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
  if($form_state['sandbox']['node']) {
    $node = $form_state['sandbox']['node'];
  }
  else{
    $node = menu_get_object();
    $form_state['sandbox']['node'] = $node;
  }
  // field_default_form($entity_type, $entity, $field, $instance, $langcode, $items, &$form, &$form_state, $get_delta = NULL)
  
  $form = field_default_form(
	    'node',
	    $node,
	    field_info_field('field_document_archive'),
	    field_info_instance('node', 'field_document_archive', 'import_ticket'),
	    LANGUAGE_NONE,
	    field_get_items('node', $node, 'field_document_archive'),
	    $form,
	    $form_state
	    );

  // Remove description 
  unset ($form['field_document_archive'][LANGUAGE_NONE][0]['#description']);
  
  // Add required includes
  if (empty($form_state['build_info']['files']) || !in_array('modules/node/node.pages.inc', $form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'node', 'node.pages');
    form_load_include($form_state, 'inc', 'doc_manage', 'doc_manage');
  }

$form['field_document_archive']['submit'] = array(
  '#type' => 'submit',
  //'#ajax' => array(
  //  'callback' => '_doc_manage_upload_archive_submit'
  //  ),
  '#submit' => array('_doc_manage_upload_archive_submit'),
  '#value' => 'Apply',
  '#attributes' => array(
    //'class' => array('invisible',)
    ),
  );
  
  $form['field_document_archive']['description'] = array(
    '#type' => 'markup',
    '#markup' => 'Upload (or remove) the prepared archive.',
    '#prefix' => '<br/><br/><div class="alert alert-info">',
    '#suffix' => '</div>',
  );
  
  $form['field_document_archive']['feedback'] = array(
    '#type' => 'markup',
    '#markup' => '<div id="upload-feedback"></div>',
  );
  
  return $form;
  
}

/**
 * Submit handler for document upload.
 *
 */
function _doc_manage_upload_archive_submit ($form, &$form_state) {
  
  $new_fid = $form_state['values']['field_document_archive'][LANGUAGE_NONE][0]['fid'];

  $node = $form_state['sandbox']['node'];
  
  $wrapper = entity_metadata_wrapper('node',$node);
  $original_file = $wrapper->field_document_archive;
  $file = $original_file->value();
  $old_fid = $file['fid'];
    
  if ($new_fid == 0 & !isset($file)) {
    drupal_set_message('No file was uploaded.', 'error');
    return FALSE;
  }
  
  if ($new_fid == 0 && isset($file)) {

    // Remove the file.
    file_delete(file_load($old_fid));
    $wrapper->field_document_archive->set(NULL);
    $wrapper->save(TRUE);
    field_attach_update('node', $node);
    $message .= '<li>The existing file attachment has been deleted.</li>';
    
  }
  else{
    
    // A new file is here!
    
    if($old_fid != 0) {
      // First, remove the old one
      $wrapper->field_document_archive->set(NULL);
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
    $wrapper->field_document_archive->set($new_file_array);
    $wrapper->save(TRUE);
    field_attach_update('node', $node);
    $message .= '<li>The new file attachment has been saved.</li>';

  }
  //$commands[] = ajax_command_replace('#status-summary', doc_manage_status_summary($node));
  //$commands[] = ajax_command_replace('#importing-steps-container', doc_manage_render_import_actions_panel ($node));
  //$commands[] = ajax_command_replace('#upload-feedback', '<div class="alert alert-info"><ul>' . $message . '</ul></div>');
  //return array('#type' => 'ajax', '#commands' => $commands);

}

/**
 * Generate heading split level form.
 */
function _doc_manage_heading_split_form($form, &$form_state) {
  $node = menu_get_object();
  
  if($form_state['sandbox']['node']) {
    $node = $form_state['sandbox']['node'];
  }
  else{
    $node = menu_get_object();
    $form_state['sandbox']['node'] = $node;
  }
 
  $form['wrap'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('ajax-split-level'),
    ),    
  );
    
  $form['wrap']['field'] = field_default_form(
	    'node',
	    $node,
	    field_info_field('field_heading_split_level'),
	    field_info_instance('node', 'field_heading_split_level', 'import_ticket'),
	    LANGUAGE_NONE,
	    field_get_items('node', $node, 'field_heading_split_level'),
	    $form,
	    $form_state
	    );
    
  // Drop description
  unset ($form['wrap']['field']['field_heading_split_level'][LANGUAGE_NONE]['#description']);
 
  // Drop Heading 3 option
  unset ($form['wrap']['field']['field_heading_split_level'][LANGUAGE_NONE]['#options'][1108]);
  
  if (empty($form_state['build_info']['files']) || !in_array('modules/node/node.pages.inc', $form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'node', 'node.pages');
    form_load_include($form_state, 'inc', 'doc_manage', 'doc_manage');
  }
  
  $form['wrap']['field']['field_heading_split_level']['und']['#ajax'] = array(
      'callback' => '_doc_manage_heading_split_submit',
      );
  $form['wrap']['description'] = array(
    '#type' => 'markup',
    '#markup' => t('Set the split level to use when building your pages.'),
    '#prefix' => '<div class="alert alert-info">',
    '#suffix' => '</div>',
  );

  $form['feedback'] = array(
    '#type' => 'markup',
    '#markup' => '',
    '#prefix' => '<div id="ajax-split-feedback">',
    '#suffix' => '</div>',
  );
  
  global $base_path;
  
  if (empty($form_state['build_info']['files']) || !in_array('modules/node/node.pages.inc', $form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'node', 'node.pages');
    form_load_include($form_state, 'inc', 'doc_manage', 'doc_manage');
  }

  return $form;
}

function _doc_manage_heading_split_submit($form, &$form_state) {
  
  $included_node = $form_state['sandbox']['node'];
  $ticket = node_load($included_node->nid);

  $wrapper = entity_metadata_wrapper('node', $ticket);
  $raw_level = $wrapper->field_heading_split_level->value();
  $input_level = $form_state['values']['field_heading_split_level'][LANGUAGE_NONE][0]['tid'];
  
  // If the received value is different than the old one, update it.
  // use field_attach_update() instead of node_save() to avoid bumping timestamps, etc.
  if ($input_level != $raw_level->tid) {
    $wrapper->field_heading_split_level->set($input_level);    
    $wrapper->save(TRUE);
    field_attach_update('node', $ticket);
  }
  
  $commands[] = ajax_command_replace('#ajax-split-level', drupal_render($form));
  $commands[] = ajax_command_replace('#ajax-split-feedback', '<p class="alert alert-info">Heading state changed to <b>' . $wrapper->field_heading_split_level->value()->name . '</b>.</p>');
  return array('#type' => 'ajax', '#commands' => $commands);
}
/**
 * generate node import action form
 */
function _doc_manage_import_steps_form($form, &$form_state, $node){
  
  $form['wrapper']['import'] = _publishing_actions_import($node, $form_state);
  
  return $form;
}

/**
 * Returns the wrapper for an accordion element.
 *
 * @param $title
 * 	The title text to display in the header
 * 	
 * @param $id
 * 	The CSS ID of the element, WITHOUT the preceding '#'.
 *
 * @param $state
 * 	'in' or ''. Determines whether the element is open or closed.
 *
 * @param $parent
 * 	The data-parent element. This is the ID of the containing panel-group, WITHOUT the preceding '#'.
 *
 * @return a render array with the appropriate wrappers for an empty accordion element.
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
  $heading = l(
	       '<span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' . t($title) ,
	       '#',
	       array(
		'html' => TRUE,
		'fragment' => $id,
		'external' => TRUE,
		'attributes' => array(
		  'data-toggle' => 'collapse',
		  'data-parent' => '#' . $parent,
		  'class' => array('btn', 'btn-lg', 'justify', 'btn-collapse-trigger', $mode),
		),
	      )
	    );
  

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
      'class' => array('panel-body','row'),
    ),
  );
  return $wrapper;
  
}

function _publishing_actions_publish ($node) {
  $form['import']['content'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="publish-actions-content" class="' . $toggle . '">',
    '#suffix' => '</div>',
    '#markup' => '<h1>Content is here!</h1>',
  );  
  return $form;
}

/**
 * Helper function that supplies the steps for importing a document.
 */
function _publishing_actions_import ($node, &$form_state) {
  
  $options_array = array();
  
  // possible states are: pending, next, finished, locked
  
  $states = _doc_manage_get_import_state($node);
  if($states['finished'] && !doc_manage_core_check_admin()){
    return;
  }
  
  // Build command render array.
  
  $classes = array(
    'locked' => array('btn-default', 'col-md-2', 'disabled'),
    'finished' => array('btn-default','col-md-2'),
    'next' => array('btn-success','col-md-2'),
    'pending' => array('btn-warning','col-md-2', 'disabled'),
    'undo' => array('btn-danger', 'col-md-2'),
    'pdf' => array('btn-primary', 'col-md-2'),
  );
  
  $options_array['unpack'] = array('title' => 'Unpack',
                                    'description' => '<p>Unpack the document archive.</p>',
                                    'note' => '<p></p>',
                                    'state' => $states['unpack'],
				    'include' => array('unpack'),
                                    'submit' => array(
					'#type' => 'submit',
					'#id' => 'unpack-submit',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'Expand the uploaded archive and perform preliminary processing. This will save all image and HTML files on the server. The original archive is kept intact.',
					    ),
					'#value' => 'Unpack',
					'#submit' => array('doc_manage_unpack'),
					),
                                    'undo' => array(
					'#type' => 'submit',
					'#id' => 'unpack-undo',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'Destroys all content extracted from the document archive, and removes any generated HTML files. This does not remove the uploaded archive.',
					    ),
					'#value' => 'Undo Unpack',
					'#submit' => array('doc_manage_undo_unpack'),
					),

                                    );
  $options_array['split'] = array('title' => 'Split',
                                    'description' => '<p>Splits the document into topics and saves them in the database.</p>',
                                    'note' => '<p></p>',
                                    'state' => $states['split'],
				    'include' => array('split'),
                                    'submit' => array(
					'#type' => 'submit',
					'#id' => 'split-submit',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'Scans the imported HTML file for topics, splits them apart, and saves them as unique nodes in the database. They will be reassembled later into an HTML document.',
					    ),
					'#value' => 'Split',
					'#submit' => array('doc_import_split'),
					),
				    'undo' => array(
					'#type' => 'submit',
					'#id' => 'split-undo',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'Delete all topic nodes generated by the "Split" action, and remove all database relations. This does not delete file resources like images, and does not touch the archive or the HTML generated from it.',
					    ),
					'#value' => 'Undo Split',
					'#submit' => array('doc_manage_undo_split'),
					),

                                    );
  $options_array['convert'] = array('title' => 'Convert',
                                    'description' => '<p>Convert the raw HTML to Technet-o-riffic HTML. </p>',
                                    'note' => '<p></p>',
                                    'state' => $states['convert'],
				    'include' => array('convert'),
                                    'submit' => array(
					'#type' => 'submit',
					'#id' => 'convert-submit',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'This converts all tags to Technet styles and applies special processing to tables and lists.',
					    ),
					'#value' => 'Convert',
					'#submit' => array('doc_import_convert'),
					),
				    'undo' => array(
					'#type' => 'submit',
					'#id' => 'convert-undo',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'data-title' => 'Undo Convert',
					    'title' => 'Remove converted HTML from all topics. The original unconverted topics will be left intact.',
					    ),
					'#value' => 'Undo Convert',
					'#submit' => array('doc_manage_undo_convert'),
					),
                                    );
  $options_array['save'] = array('title' => 'Save',
                                    'description' => '<p>Assemble topics into pages. A new page will start at each heading specified as your split level.</p>',
                                    'note' => '<p>If you have already saved book nodes, this will create a duplicate set.</p>',
                                    'state' => $states['save'],
				    'include' => array('save'),
                                    'submit' => array(
					'#type' => 'submit',
					'#id' => 'save-submit',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'Generate a linked set of document pages from the converted topics. Topics will be grouped according to the heading level you specified.',
					    ),
					'#value' => 'Save',
					'#submit' => array('doc_manage_save_nodes'),
					),
				    'undo' => array(
					'#type' => 'submit',
					'#id' => 'save-undo',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'This will delete all document pages and remove all page relations from the database. It does not delete source topics or undo content conversion. YOU WILL NOT BE ASKED TO CONFIRM, so be sure you mean to do this!',
					    ),
					'#value' => 'Undo Save',
					'#submit' => array('doc_manage_undo_save'),
					),
                                  );
  
  $options_array['post'] = array('title' => 'Postprocess',
				    'description' => '<p>Rebuild cross-references, add page links, and other pre-publication tasks.</p>',
				    'note' => '<p><ul><li>Cross References must be rebuilt.</li>
						      <li>In-page links must be inserted.</li></ul></p>',
				    'state' => $states['post'],
				    'include' => array('postprocess'),
				    'submit' => array(
					'#type' => 'submit',
					'#id' => 'repair-xref-submit',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'Postprocessing re-connects all cross-reference links, generates "on this page" content, and other important pre-publication tasks.',
					    ),
					'#value' => 'Postprocess',
					'#submit' => array('doc_import_postprocess'),
					),
				    'undo' => array(
					'#type' => 'submit',
					'#id' => 'postprocess-undo',
				      	'#attributes' => array(
					    'data-toggle' => 'tooltip',
					    'data-placement' => 'right auto',
					    'title' => 'This unwinds the postprocessing function. Added links are removed, and the "on this page" section is deleted.',
					    ),
					'#value' => 'Undo Post',
					'#submit' => array('doc_manage_undo_postprocess'),
					),
				   );
    
  //if all steps are done, don't generate the form. Administrators get the form regardless.
  
  if($states['finished']){
    $in = ' collapse';
    $mode = ' collapsed';
    $comment = ' (completed)';
  }
  else{
    $in = ' in';
    $mode = '';
  }
    //build form
  
  $form['import'] = array(
  );
  $form['import']['content'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="import-actions-content" class=" ' . $toggle . '">',
    '#suffix' => '</div>',
);  
    

  $i = 0;
  foreach($options_array as $name=>$option){
    $i++;

    // Bring in required includes
    foreach($option['include'] as $include) {
      form_load_include($form_state, 'inc', 'doc_import', "doc_import.$include");
    }
    
    //submit buttons are only enabled if they are in the next step
    $disabled = FALSE;

    $disabled = $option['state'] != 'next' && !$override ? TRUE : FALSE;
    $undo_enabled = $option['state'] == 'finished' ? TRUE : FALSE;
    
    $form['import']['content'][$name] = array(
      '#prefix' => '<div class="import-commands row ' . $option['state'] . ' ' . $override . $option['status'] . '">',
      '#suffix' => '</div>',
    );
    $form['import']['content'][$name]['status'] = array(
      '#type' => 'markup',
      '#title' => $option['title'],
      '#markup' => '<div class="option-state col-sm-1 ' . $override . $option['state'] . '"><span class="image">&nbsp</span></div>',
      );   

    $form['import']['content'][$name]['number'] = array(
      '#type' => 'markup',
      '#title' => 'Step',
      '#markup' => "<h4 class=\"step col-sm-1\"> " . $i . "</h4>",
      );

    $option['submit']['#attributes']['class'] = $classes[$option['state']];

    
    if($option['status'] == 'disabled') {
      $option['submit']['#attributes']['class'][] = 'disabled';
    }
    
    $form['import']['content'][$name]['submit'] = $option['submit'];  
     
    $form['import']['content'][$name]['about'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="import-commands-about col-sm-7 hidden-xs">',
      '#suffix' => '</div>',
    );
    
    // Check for potential issues prior to saving content.
    if($option['title'] == 'Save' && $option['state'] == 'next') {
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
    
    $form['import']['content'][$name]['about']['description'] = array(
      '#type' => 'markup',
      '#title' => $option['title'],
      '#markup' => '<div class="description">' . ($warnings ? render($warnings) : $option['description']) . '</div>',
      );
    unset($warnings);
    $form['import']['content'][$name]['about']['note'] = array(
      '#type' => 'markup',
      '#title' => $option['title'],
      '#markup' => '<div class="note">' . $option['note'] . '</div>',
      );
    
    if ($undo_enabled) {
      $option['undo']['#attributes']['class'] = $classes['undo'];
      $form['import']['content'][$name]['submit'] = $option['undo'];
    }
  }
  return $form;
}


/**
 * Returns state of individual processing steps. Assumes the call is from a controller object.
 */
function _doc_manage_get_import_state($controller = NULL){

  $types = array('unpack','split','convert','save','post','pdf');
  
  if (!$controller) {
    $controller = menu_get_object();
  }

  $query = relation_query('node', $controller->nid);
  $query->entityCondition('bundle', 'uses_topic');
  
  // Get topic relation
  $topic_relation = $query->execute();
  $topic_relation = array_shift($topic_relation);
  $topic_relation = relation_load($topic_relation->rid);

  // Get page relation
  $new_query = relation_query('node', $controller->nid);
  $new_query->entityCondition('bundle','ticket_controls_nodes');
  
  $page_relation = $new_query->execute();
  $page_relation = array_shift($page_relation);
  $page_relation = relation_load($page_relation->rid);
  $states = array();
  
  foreach ($types as $type){
    switch($type){
      case 'unpack':
        //unpacking will be the next step if there is no controller directory.
        if(!file_exists('public://import-source-files/'.$controller->nid)) {
	  
	  // "Unpack" will be active if there is an archive attached. If there is not, it will be pending (along with everything else).
	  if($controller->field_document_archive[LANGUAGE_NONE][0]['fid']) {	    
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
        if($states['unpack'] != 'finished'){
          $states[$type] = 'pending';
          continue;
        }
        
        if ($topic_relation) {
	  $states['unpack'] = 'locked';
          $states[$type] =  'finished';
          $finished_count++;
          continue;
        }
        else {
          $states[$type] =  'next';
          continue;
        }
        break;
      
      case 'convert':
        
        if($states['split'] != 'finished'){
          $states[$type] = 'pending';
          continue;
        }
        
        //test the first topic node to see if it has been converted. Since conversions are bulk processes, if the first has been done, they all have.
        if (file_exists("public://import-source-files/".$controller->nid."/converted.lck")) {
          $states[$type] =  'finished';
	  $states['split'] = 'locked';
          $finished_count++;
        }
        else {
          $states[$type] =  'next';
          continue;
        }
        break;
      
      case 'save':
        if($states['convert'] != 'finished'){
          $states[$type] = 'pending';
          continue;
        }
        $test_topic = node_load($topic_relation->endpoints[LANGUAGE_NONE][1]['entity_id']);
        if($test_topic->field_topic_processed_html){

          if ($page_relation) {
            $states[$type] =  'finished';
	    $states['convert'] = 'locked';
          $finished_count++;
          }
          else {
            $states[$type] =  'next';
          }
        }
        break;
      
      case 'post':
        if ($states['save'] != 'finished') {
          $states[$type] = 'pending';
          continue;
        }
        
        //'post' is either done or not now:
        if(file_exists("public://import-source-files/".$controller->nid."/postprocessed.lck")) {
          $states[$type] =  'finished';
	  $states['save'] = 'locked';
          $finished_count++;
        }
        else {
          $states[$type] =  'next';
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
  if ($finished_count == count($types)){
    $states['finished'] = TRUE;
  }
  return $states;
}


/**
 * Undo functions: Destroy archive and saved files
 *
 * @param $ticket
 * 	Controlling import ticket. If nothing is passed, the current object will be loaded.
 */
function doc_manage_undo_unpack($form, &$form_state) {

  $ticket = menu_get_object();
  
  $resource_directory = "public://import-source-files/".$ticket->nid."/";
  $file = file_load($ticket->field_raw_html_master[LANGUAGE_NONE][0]);
  
  // Drop the raw HTML file from the field and update the ticket
  unset($ticket->field_raw_html_master[LANGUAGE_NONE][0]);
  node_save($ticket);
  // Delete the file from disk
  if($file) $success = file_delete($file);

  // Recursively delete the resource directory. This removes all images, HTML files,
  // and anything else generated by the unpack operation.
  module_load_include('inc', 'doc_import');
  _rmdir_recursive($resource_directory);
  
  //document conversion complete, update workflow state
  $target_state = 21; //21 = "Created" state
  $comment = t("State changed by import module (Undo Unpack operation)");
  $new_state = workflow_execute_transition($ticket, $target_state, $comment, TRUE);
  entity_get_controller('node')->resetCache((array)$ticket->nid);

  drupal_set_message('Extracted files have been removed and resource directory deleted. The uploaded file archive is still attached to this ticket. <br/>
		     Document state has been set to "Created".');

}

/**
 * Undo functions: Bulk delete topics
 *
 * @param $ticket
 * 	Controlling import ticket. If nothing is passed, the current object will be loaded.
 */
function doc_manage_undo_split($form, &$form_state) {

  $ticket = menu_get_object();
  $operations = array();
  
  //set up batch
  $topics = _doc_manage_get_topics($ticket);

  $operations[] = array('_doc_manage_batch_delete_topics_op', array($topics, $ticket));
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

function _doc_manage_batch_delete_topics_op($topics, $ticket, &$context){
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

function _doc_manage_batch_delete_topics_finished($success, $results, $operations){
  if($success){
  $ticket = $results['ticket'];
  
  //document conversion complete, update workflow state
  $target_state = 26; //26 = "Importing" state
  $comment = t("State changed by import module (Undo Split Topics operation)");
  $new_state = workflow_execute_transition($ticket->nid, $target_state, $comment, TRUE);
  entity_get_controller('node')->resetCache((array)$ticket->nid);

  drupal_set_message('All topics generated from this archive have been deleted, and all database relations removed.<br/>
		     Document state has been reset to "Importing".');

  }
  else{
    drupal_set_message('Error while deleting topic nodes.','error');
  }
}


/**
 * Undo functions: Undo conversions
 *
 * @param $ticket
 * 	Controlling import ticket. If nothing is passed, the current object will be loaded.
 */
function doc_manage_undo_convert($form, &$form_state) {
  
  $ticket = menu_get_object();
  //pick up the entities referenced by this controller, then drop the first element (which is the controller).
  $topics = _doc_manage_get_topics($ticket);

  $operations[] = array('_doc_manage_batch_unconvert_op', array($topics, $ticket));
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

function _doc_manage_batch_unconvert_op($topics, $ticket, &$context){
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

function _doc_manage_batch_unconvert_finished($success, $results, $operations){
  if($success){
    $ticket = $results['ticket'];
    // Remove lock file
    $lock = "public://import-source-files/".$ticket->nid."/converted.lck";
    if(file_exists($lock)) {
      $hooray = file_unmanaged_delete($lock);
    }
  }
  if ($hooray){
    drupal_set_message('All related topics were unconverted.');
  }
  else{
    drupal_set_message('Error while unconverting topics.','error');
  }
}


/**
 * Undo functions: Bulk delete book pages
 *
 * @param $ticket
 * 	Controlling import ticket. If nothing is passed, the current object will be loaded.
 */
function doc_manage_undo_save($form, &$form_state) {
  //pick up the entities referenced by this controller, then drop the first element (which is the controller).
  $endpoints = doc_manage_relation_query('ticket_controls_nodes',$ticket);
  $dump = array_shift($endpoints);

  $operations = array();
  
  //set up batch
  $operations[] = array('_doc_manage_batch_delete_nodes_op', array($endpoints, $dump));
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

function _doc_manage_batch_delete_nodes_op($endpoints, $controller, &$context){
  // information needed to track progression between successive calls.
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_topic'] = 0;

    // Save topic count for the termination message.
    $context['sandbox']['max'] = count($endpoints);
    
    // log controller node
    $context['results']['controller'] = $controller;
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

function _doc_manage_batch_delete_nodes_finished($success, $results, $operations){
  if($success){
  //$query = relation_query('node', $results['controller']['entity_id'], 'ticket_controls_nodes');
  //$topic_relation = $query->execute();
  //$topic_relation = array_shift($topic_relation);
  //relation_delete($topic_relation->rid);
    drupal_set_message('All selected nodes were deleted.');
  }
  else{
    drupal_set_message('Error while deleting nodes.','error');
  }
}


/**
 * Undo functions: destroy postprocessing data
 *
 * @param $ticket
 * 	Controlling import ticket. If nothing is passed, the current object will be loaded.
 */
function doc_manage_undo_postprocess($form, &$form_state) {
  $ticket = menu_get_object();
  //pick up the entities referenced by this controller, then drop the first element (which is the controller).
  $endpoints = doc_manage_relation_query('ticket_controls_nodes',$ticket);
  $parent_tmp = array_shift($endpoints);
  $cover_tmp = array_shift($endpoints);

  $nodes = node_load_multiple($endpoints);
  foreach($nodes as $node) {
    $body = $node->body[LANGUAGE_NONE][0]['value'];
    $body = htmlqp($body,'a.page-xref-anchor')->remove()->top()->find('div.page-links')->remove()->top()->find('body')->innerHTML();
    $body = str_replace('<!--end link insert-->', '', $body);
    $node->body[LANGUAGE_NONE][0]['value'] = $body;
    node_save($node);
  }
   
  // Drop the TOC field from the import ticket.
  $wrap = entity_metadata_wrapper('node', $ticket);
  $wrap->field_toc_array->set(NULL);
  $wrap->save();
  
  // Remove lock file
  $lock = "public://import-source-files/".$ticket->nid."/postprocessed.lck";
  if(file_exists($lock)) {
    $success = file_unmanaged_delete($lock);
  }
  if($success) {
    drupal_set_message('Postprocess functions undone.');
  }
  else {
    drupal_set_message('Could not undo postprocess functions.','error');
  }
}


/*
 * Generate a table of related book nodes for this ticket.
 * Also returns the workflow state of each node.
 */
function display_book_relations($ticket) {

  $element = array();
  global $base_path;
  
  $book_pages = doc_manage_get_book_pages($ticket);
    foreach ($book_pages as $endpoint) {
    
      $flag = flag_get_flag('html_problem_alert');
      $flagged = $flag->is_flagged($endpoint);
      
      $flag_cell = $flagged ? '<div class="flag-error"><span class="glyphicon glyphicon-exclamation-sign"></span></div>' : '<div class="flag-ok"><span class="glyphicon glyphicon-ok-sign"></span></div>';

      $node = node_load($endpoint);

      $cell['related-node']['href'] = 'node/' . $node->nid;
      $cell['related-node']['title'] = (reset($book_pages) == $endpoint) ? $node->title . ' [Cover page automatically approved]' : $node->title;
		  
		  
      $depth = "book-depth-" . $node->book['depth'];
      $attributes = array('html' => TRUE, 'class' => array('links', 'inline', $depth, $flagged ? 'flag-error' : ''));

      unset ($cell['path'],$cell['options']);
      $workflow_cell = get_node_workflow($node);
    
      $rendered_cell= theme('links',array(
				  'links' => $cell,
				  'attributes' => $attributes));

      $columns[] = array ('column-first' => $rendered_cell, /*'column-second' => theme_links($action_links),*/ 'column-third' => $workflow_cell, 'column-fourth' => $flag_cell);
    }
    
    $element['wrapper'][$delta]['content']['relation'] = array(
      '#theme' => 'table',
      '#header' => array('Page', 'Current State','<span class="glyphicon glyphicon-flag"></span>'),
      '#rows' => $columns,
    );
    
  return $element;
}


/*
 * Given a node ID or a node object, returns a styled link back to the document ticket
 */
function doc_manage_get_management_menu($node){
  if (!is_object($node)){
    $node = node_load($node);
  }
  
  $rid = $node->field_document_relation[LANGUAGE_NONE][0]['value'];
  $relation = (array)relation_load($rid);
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
   'notranlsate' => flag_create_link('notranslate', $node->nid),
   );
  
  // Build list.
  // This is not run through the normal theming layer, which introduces tons of extra markup and is incompatible with bootstrap
  $management_links_new = '<div class="system-nav menu list-group">'
    . l('<span class="glyphicon glyphicon-home"></span> ' . t('Return to ticket'), $uri, $options)
    . l('<span class="glyphicon glyphicon-edit"></span> ' . t('Edit this page'),"node/".$node->nid."/edit", $options);
    
  foreach($flags as $flag) {
    $management_links_new .= $flag;
  }    
    
  $management_links_new .= '</div>';
    
  return $management_links_new;

}

/**
 * Provide a button that causes PDF reactor to generate, fetch, and attach a PDF to the ticket.
 *
 * 	
 */
function doc_manage_pdfreactor_button_form ($form, &$form_state) {
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
      'class' => array ('btn','btn-primary', 'btn-block'),
    ),
  );
  return $form;
}

 /**
  * Execute PDF Reactor job. Callback for doc_manage_pdfreactor_button_form().
  *
  */
function doc_manage_pdfreactor_button_callback ($form, &$form_state, $source = NULL) {
 
  // Load the object the form was called from (this is usually the import ticket):
  if(!$source) {
    $source = $form_state['sandbox']['node'];
  }
    
  // Generate the PDF.
  pdfreactor_import_ticket_generate($source);
}

