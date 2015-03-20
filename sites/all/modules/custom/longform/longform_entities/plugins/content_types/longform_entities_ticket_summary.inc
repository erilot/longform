<?php

$plugin = array(
  'single' => TRUE,  // Just do this one, it is needed.
  'title' => t('Summary table of tickets in a set'), 
  'description' => t('Provide a summary table with all the tickets contained in a set. Admins get special magic.'), 
  'category' => t('Longform'), 
  'edit form' => 'longform_entities_ticket_summary_pane_edit_form',
  'render callback' => 'longform_entities_ticket_summary_pane_render',
//  'admin info' => 'longform_entities_ticket_summary_pane_admin_info', // A function that will return the information displayed on the admin screen (optional).
  'defaults' => array( // Array of defaults for the settings form.
    'button_label' => t('replace_this'), // Replace this with something actually useful, if there is something
  ),
  'all contexts' => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function longform_entities_ticket_summary_pane_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

//  $form['button_label'] = array(
//    '#type' => 'textfield',
//    '#title' => ('Button label'),
//    '#description' => t('Label to display in the "add ticket" button, which triggers the modal and form'),
//    '#default_value' => $conf['button_label'],
//    );
  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets saved
 * Notice, the magic that automatically does that for you.
 */
function longform_entities_ticket_summary_pane_edit_form_submit(&$form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

/**
 * Run-time rendering of the body of the block (content type)
 */
function longform_entities_ticket_summary_pane_render($subtype, $conf, $args, $contexts) {
dpm(libraries_info('underscore'),'ibraries');
  global $user;
  $user_is_admin = user_access('administer tickets', $user);
  $context = reset($contexts);
  $set = $context->data;

  $endpoints = longform_relation_get_other_endpoint_set_ticket($set);
  $header = array(
    array('data' => 'Type', 'class' => array('col-md-1')),
    array('data' => 'Title', 'class' => array('col-md-3')),
    array('data' => 'Owned by', 'class' => array('col-md-1')),
    array('data' => 'Publish Date', 'class' => array('col-md-1')),
    array('data' => 'State', 'class' => array('col-md-1')),
  );
  if ($user_is_admin) {
    $header[] = array('data' => 'Admin', 'class' => array('col-md-1'));
  }
  $rows = array();

  foreach ($endpoints as $entity) {

    $ticket = entity_metadata_wrapper($entity->type, $entity->tid);

    // Build the row of cells.
    // Administrators will get extra forms and options here, depending on their permissions settings.
    // Note: Fields that are generated multiple times must be called with the form name and a "_uniqueID" suffix,
    // such as an ID. Example: drupal_get_form('longform_entities_ticket_title_edit_form_' . $tid).
    $cells = array(
      /* Type */  array('data' => '<b>' . l($ticket->field_document_type[0]->label(), 'ticket/' . $ticket->tid->value(),array('html' => TRUE, 'attributes' => array('class' => array('btn', 'btn-primary', 'btn-block'))))),
      /* Title */ array('data' => $user_is_admin ? drupal_get_form('longform_entities_ticket_title_edit_form_' . $ticket->tid->value(), $ticket) : $ticket->title->value()),
      /* User */  array('data' => l($ticket->uid->value()->name, 'user/' . $ticket->uid->value()->uid, array('attributes' => array('class' => array('btn', 'btn-default'))))),
      /* Publish */array('data' => date('M. d, Y', $ticket->field_publish_date->value())),
      /* State */ array('data' => longform_workflow_progress_bar($entity, 'info')),
    );

    if ($user_is_admin) {
      $button_opts = array(
        'attributes' => array(
          'class' => array('btn', 'btn-default', 'text-danger'),
        ),
        'html' => TRUE,
        'query' => array('destination' => current_path()),
      );
      $cells[]['data'] = array(
        'group' => array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => 'btn-group',
          ),
          'delete' => array(
            '#markup' => l('<span class="glyphicon glyphicon-remove text-danger"></span>', 'admin/longform/ticket/manage/' . $ticket->tid->value() . '/delete', $button_opts),
          ),
        ),
      );
    }
    $rows[] = $cells;
  }

  if (count($rows) == 0) {
    $rows[]['data'][] = array(
      'data' => array('#markup' => '<label class="text-warning">None yet!</label>'),
      'colspan' => count($header),
      'class' => array('warning'),
    );
  }


  $table = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array(
      'class' => array('table', 'table-condensed'),
    ),
  );

  $out = array();
  $out['title'] = array(
    '#markup' => '<h3>' . t('Tickets in this set') . '</h3>',
  );
  $out['table'] = $table;



  $block = new stdClass();
  $block->title = 'Summary table';
  $block->content = $out;

  // dev //


  // end dev //

  return $block;
}

