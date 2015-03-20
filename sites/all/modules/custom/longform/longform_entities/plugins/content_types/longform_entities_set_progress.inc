<?php

$plugin = array(
  'single' => TRUE,  // Just do this one, it is needed.
  'title' => t('Show set progress report'),
  'description' => t('Show metrics related to finishing a document set'),
  'category' => t('Longform'), 
  'edit form' => 'longform_entities_set_progress_form', // A function that will return the settings form for the pane.
  'render callback' => 'longform_entities_set_progress_render', // A function that will return the renderable content.
//  'admin info' => 'longform_entities_pane_custom_pane_admin_info', // A function that will return the information displayed on the admin screen (optional).
  'defaults' => array( // Array of defaults for the settings form.
    //'button_label' => t('Add new tickets'),
  ),
  'all contexts' => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function longform_entities_set_progress_form($form, &$form_state) {
  $conf = $form_state['conf'];
  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets saved
 * Notice, the magic that automatically does that for you.
 */
function longform_entities_set_progress_form_submit(&$form, &$form_state) {
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
function longform_entities_set_progress_render($subtype, $conf, $args, $contexts) {

  global $user;
  $user_is_admin = user_access('administer tickets', $user);
  $context = reset($contexts);
  $set = $context->data;

  $tickets = longform_relation_get_other_endpoint_set_ticket($set);

  if (empty($tickets)) {
    return FALSE;
  }

  foreach($tickets as $ticket) {
    $state = longform_get_current_workflow_state($ticket);
    if ($state->name != 'published') {
      $unpublished[] = $ticket->title;
    }
    else {
      $published[] = $ticket->title;
    }
  }

  $progress = round((count($published) / count($tickets) * 100), 0);
  $label = count($published) . ' of ' . count($tickets) . ' (' . $progress . '%)';

  $content = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('row'),
    ),
  );

  $content['progress_bar'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12'),
    ),
  );

  $content['progress_bar']['bar'] = array(
    '#markup' => '
        <div class="progress"><label>' . (count($published) == 0 ? t('No documents are published yet.') : t('Progress')) . '</label>
          <div class="'. ($progress == 100 ? 'progress-bar-success' : 'progress-bar-info') . '" style="width:' . $progress . '%">' . $label . '
          </div>
        </div>',
  );

  $block = new stdClass();
  $block->title = '';
  $block->content = $content;

  return $block;
}
