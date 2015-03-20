<?php

$plugin = array(
  'single' => TRUE,  // Just do this one, it is needed.
  'title' => t('Dynamic document generate button'),  // Title to show up on the pane screen.
  'description' => t('Provides a simple form with settings and a trigger to force creation of a dynamic document.'), // Description to show up on the pane screen.
  'category' => t('Dynamic Documents'), // A category to put this under.
  'edit form' => 'free_view_pane_generate_doc_pane_pane_edit_form', // A function that will return the settings form for the pane.
  'render callback' => 'free_view_pane_generate_doc_pane_pane_render', // A function that will return the renderable content.
  'admin info' => 'free_view_pane_generate_doc_pane_pane_admin_info', // A function that will return the information displayed on the admin screen (optional).
  'defaults' => array( // Array of defaults for the settings form.
    'text' => '',
  ),
  'all contexts' => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function free_view_pane_generate_doc_pane_pane_edit_form($form, &$form_state) {
  //$conf = $form_state['conf'];
  //
  //$form['text'] = array(
  //  '#type' => 'textfield',
  //  '#title' => t('Panel Text'),
  //  '#description' => t('Text to display, it may use substitution strings'),
  //  '#default_value' => $conf['text'],
  //);

  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets saved
 * Notice, the magic that automatically does that for you.
 */
function free_view_pane_generate_doc_pane_pane_edit_form_submit(&$form, &$form_state) {
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
function free_view_pane_generate_doc_pane_pane_render($subtype, $conf, $args, $contexts) {

  $block = new stdClass();

  // initial content is blank
  $block->title = t('Generate a dynamic document'); // This will be overridden by the user within the panel options.
  $block->content = drupal_get_form('free_view_generate_dynamic_document_form');

  return $block;
}