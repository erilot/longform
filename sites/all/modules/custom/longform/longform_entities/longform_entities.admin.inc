<?php
/**
 * Created by PhpStorm.
 * User: ericlotze
 * Date: 2014-10-17
 * Time: 5:54 PM
 */

function longform_ticket_form($form, &$form_state, $ticket, $op = 'edit'){

  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => isset($ticket->title) ? $ticket->title : '',
    '#description' => t('Ticket title.'),
    '#required' => TRUE,
    '#weight' => -50,
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => isset($ticket->tid) ? t('Update ticket') : t('Save ticket'),
    ),
  );

  $form['set_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('panel', 'panel-default'),
    ),
  );
  $form['set_wrapper']['add_doctype'] = array(
    '#type' => 'select',
    '#title' => t('Add a document'),
    '#options' => taxonomy_allowed_values(field_info_field('field_document_type')),
    '#description' => t('Select a document type to add'),
  );

  field_attach_form('longform_ticket', $ticket, $form, $form_state);
  return $form;
}

function longform_ticket_form_validate($form, &$form_state) {

}

function longform_ticket_form_submit($form, &$form_state) {
  global $user;
  $ticket = entity_ui_form_submit_build_entity($form, $form_state);
  $ticket->save();
  drupal_set_message(t('The ticket <i>@ticket</i> has been saved.', array('@ticket' => $ticket->title)),'status');
  $form_state['redirect'] = 'admin/longform/ticket';

}

function longform_set_form($form, &$form_state, $set, $op = 'edit'){

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => isset($set->name) ? $set->name : '',
    '#description' => t('Set name.'),
    '#required' => TRUE,
    '#weight' => -50,
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => isset($set->sid) ? t('Update set') : t('Save set'),
    ),
  );

  field_attach_form('longform_set', $set, $form, $form_state);
  return $form;
}

function longform_set_form_validate($form, &$form_state) {

}

function longform_set_form_submit($form, &$form_state) {
  global $user;
  $set = entity_ui_form_submit_build_entity($form, $form_state);
  $set->save();
  drupal_set_message(t('The set <i>@set</i> has been saved.', array('@set' => $set->name)),'status');
  $form_state['redirect'] = 'admin/longform/set';

}