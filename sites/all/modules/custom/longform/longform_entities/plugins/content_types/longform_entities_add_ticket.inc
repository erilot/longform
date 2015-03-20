<?php

$plugin = array(
  'single' => TRUE,
  // Just do this one, it is needed.
  'title' => t('Add new tickets to a set'),
  'description' => t('Add new tickets to a document set'),
  'category' => t('Longform'),
  'edit form' => 'longform_entities_pane_custom_pane_edit_form',
  // A function that will return the settings form for the pane.
  'render callback' => 'longform_entities_pane_custom_pane_render',
  // A function that will return the renderable content.
//  'admin info' => 'longform_entities_pane_custom_pane_admin_info', // A function that will return the information displayed on the admin screen (optional).
  'defaults' => array( // Array of defaults for the settings form.
    'button_label' => t('Add new tickets'),
  ),
  'all contexts' => TRUE,
  // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function longform_entities_pane_custom_pane_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $form['button_label'] = array(
    '#type' => 'textfield',
    '#title' => ('Button label'),
    '#description' => t('Label to display in the "add ticket" button, which triggers the modal and form'),
    '#default_value' => $conf['button_label'],
  );

  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets
 * saved Notice, the magic that automatically does that for you.
 */
function longform_entities_pane_custom_pane_edit_form_submit(&$form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

/**
 * Run-time rendering of the body of the block (content type)
 * See ctools_plugin_examples for more advanced info
 */
function longform_entities_pane_custom_pane_render($subtype, $conf, $args, $contexts) {

  global $user;
  $user_is_admin = user_access('administer tickets', $user);

  if (!$user_is_admin) {
    return NULL;
  }

  $context = reset($contexts);
  $set = $context->data;
  $content['trigger'] = array(
    '#markup' => '<a class="btn btn-success" data-toggle="modal" data-target="#add-ticket-modal">' . $conf['button_label'] . '</a>'
  );

  $content['form'] = drupal_get_form('longform_set_add_ticket_form', $set);
  $block = new stdClass();
  $block->title = '';
  $block->content = $content;

  return $block;
}

function longform_set_add_ticket_form($form, &$form_state, $set) {

  $form_state['longform_set'] = $set;
  $form_id = $form_state['build_info']['form_id'];

  // JS settings array. LongformEntities => thisElement => settings
  $settings = array(
    'longformEntities' => array(
      $form_id => array(
        'minimumSelectionCount' => 1,
        'maximumSelectionCount' => NULL,
        'allowMultipleInstances' => FALSE,
        'selectDeepestOnly' => TRUE,
        'existingTerms' => NULL,
        'dataStoreElement' => 'data-store-' . $form_id,
      ),
    ),
  );
  drupal_add_js($settings, 'setting');

  $form = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('modal', 'fade', 'multiselect'),
      'id' => 'add-ticket-modal',
    ),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'longform_entities') . '/js/term-multiselect.js',),
      'css' => array(drupal_get_path('module', 'longform_entities') . '/css/term-multiselect.style.css',),
    ),
  );
  $form['dialog'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('modal-dialog', 'modal-lg'),
    ),
  );

  $form['dialog']['modal-content'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('modal-content'),
    ),
  );
  $form['dialog']['modal-content']['modal-header'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('modal-header'),
    ),
  );
  $form['dialog']['modal-content']['modal-header']['content'] = array(
    '#markup' =>
      '<button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button>
        <h4 class="modal-title">' . t('Add new documents') . '</h4>',
  );
  $form['dialog']['modal-content']['modal-body'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('modal-body'),
    ),
  );
  $form['dialog']['modal-content']['modal-body']['content'] = array(
    'switcher' => array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('panel'),
      ),
      'selector' => array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('panel-body'),
        ),
        'build_container' => array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('build-container'),
          ),
          'container-inner' => array(
            '#type' => 'container',
            '#attributes' => array(
              'class' => array('build-container-inner'),
            ),
            'build-list' => array('#markup' => '<ul class="build-container-list nav nav-pills"></ul>'),
          ),
        ),
        'contents' => array('#markup' => book_nav_vocab_nav_block('document_type', 'term'))
      ),
    ),
    'term_selection' => array(
      '#type' => 'hidden',
      '#value' => '',
      '#attributes' => array(
        'class' => array('data-store'),
        'id' => 'data-store-' . $form_id,
      ),
    )
  );

  $form['dialog']['modal-content']['modal-footer'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('modal-footer'),
    ),
  );
  $form['dialog']['modal-content']['modal-footer']['message'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-9', 'message-container'),
    ),
  );

  $form['dialog']['modal-content']['modal-footer']['cancel'] = array(
    '#markup' => '<button type="button" class="btn btn-default ticket-add-cancel" data-dismiss="modal">' . t('Cancel') . '</button>',
  );

  $form['dialog']['modal-content']['modal-footer']['submit'] = array(
    '#type' => 'submit',
    '#id' => 'add-ticket-submit',
    '#submit' => array('longform_add_ticket'),
    '#value' => t('Add tickets'),
    '#attributes' => array(
      'class' => array('disabled'),
    ),
  );

  return $form;

}

/**
 * Submit function for longform_administer_ticket_form(): add tickets
 *
 * @param $form
 * @param $form_state
 */
function longform_add_ticket($form, &$form_state) {
  $set = $form_state['longform_set'];
  $set_wrap = entity_metadata_wrapper('longform_set', $set->sid);
  $terms_to_add = explode(',', $form_state['input']['term_selection']);
  $added = '';

  try {
    $release = $set_wrap->field_software_release->value();
    if (count($release) == 0) {
      drupal_set_message('Tickets can\'t be added before the document set\'s software release is set.', 'error');

      return FALSE;
    }

  }
  catch (Exception $e) {
  }

  if (count($terms_to_add) === 0) {
    drupal_set_message('No terms were added.', 'error');

    return FALSE;
  }
  $common_fields = longform_get_common_fields('longform_ticket', 'longform_ticket', 'longform_set', 'longform_set');
  global $user;
  foreach ($terms_to_add as $term) {

    // Build title elements
    // Releases
    $rels = array();

    $rels[] = $set_wrap->field_software_release->value()->name;

    // Products
    $products = $set_wrap->field_product->value();
    $prods = array();
    foreach ($products as $prod) {
      $prods[] = $prod->name;
    }

    $title = taxonomy_term_load($term)->name . ' ' . t('for') . ' ' . implode(', ', $prods) . ': ' . (count($rels) == 1 ? t('Version') : t('Versions')) . ' ' . implode(', ', $rels);
    $properties = array(
      'uid' => $user->uid,
      'title' => $title,
    );
    $added .= '<li>' . $title . '</li>';

    $ticket = entity_create('longform_ticket', $properties);

    // Field values are inherited from the parent docset
    foreach ($common_fields as $field) {
      $ticket->$field = $set->$field;
    }
    $ticket = entity_metadata_wrapper('longform_ticket', $ticket);
    $ticket->field_document_type->set(array($term));
    $ticket->save();
    $tickets[] = $ticket->tid->value();

    // Set initial workflow state
    longform_workflow_set_state($ticket->value(), 'tid', 'unpack', NULL, $comment = 'Initial value, automatically set');

    // Add relation
    $relation = longform_relation_create($set_wrap->type->value(), $set_wrap->getIdentifier(), $ticket->type->value(), $ticket->getIdentifier(), 'set_ticket');
  }

  drupal_set_message('<p>' . (count($tickets) == 1 ? 'A ticket was ' : count($tickets) . ' tickets were ') . 'added: </p><ul>' . $added . '</ul>');

}


