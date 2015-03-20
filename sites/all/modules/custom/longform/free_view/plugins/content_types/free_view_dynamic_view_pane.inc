<?php

$plugin = array(
  'single' => TRUE,  // Just do this one, it is needed.
  'title' => t('Dynamic document view'),  // Title to show up on the pane screen.
  'description' => t('Provides a window for viewing dynamic documents based on the free_view module.'), // Description to show up on the pane screen.
  'category' => t('Dynamic Documents'), // A category to put this under.
  'edit form' => 'free_view_pane_custom_pane_edit_form', // A function that will return the settings form for the pane.
  'render callback' => 'free_view_pane_custom_pane_render', // A function that will return the renderable content.
  'admin info' => 'free_view_pane_custom_pane_admin_info', // A function that will return the information displayed on the admin screen (optional).
  'defaults' => array( // Array of defaults for the settings form.
    'text' => '',
  ),
  'all contexts' => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function free_view_pane_custom_pane_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $form['text'] = array(
    '#type' => 'textfield',
    '#title' => t('Panel Text'),
    '#description' => t('Text to display, it may use substitution strings'),
    '#default_value' => $conf['text'],
  );

  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets saved
 * Notice, the magic that automatically does that for you.
 */
function free_view_pane_custom_pane_edit_form_submit(&$form, &$form_state) {
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
function free_view_pane_custom_pane_render($subtype, $conf, $args, $contexts) {

  global $base_path;
  
  // Add waypoints library
  drupal_add_js(drupal_get_path('module', 'free_view') . '/js/libraries/waypoints/waypoints.min.js', 'file');
  drupal_add_js(drupal_get_path('module', 'free_view') . '/js/libraries/waypoints/waypoints-infinite.js', 'file');
  
  // Add free_view js and settings
		drupal_add_js(drupal_get_path('module', 'free_view') . '/js/free_view_dynamic_load.js','file');
  drupal_add_js(drupal_get_path('module', 'free_view') . '/js/free_view_content.js', 'file');
  drupal_add_js(array('freeView' => array('basePath' => $base_path)), 'setting');

  // Add css (stylesheets stolen from book_nav module)
  drupal_add_css(drupal_get_path('module', 'book_nav') . '/css/polycom.styles.css');
  drupal_add_css(drupal_get_path('module', 'book_nav') . '/css/doc_nav.css');
  
  // Update the strings to allow contexts.
  if (!empty($contexts)) {
    $content = ctools_context_keyword_substitute($conf['text'], array(), $contexts);
  }
  $term = $contexts['argument_term_1']->data;
  
  $wrapper['container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('dynamic-content-wrapper', 'row'),
      'id' => 'doc-content',
    ),
  );
  $wrapper['container']['content'] = free_view_render_topics($term, 3);
  $block = new stdClass();
  $block->title = ''; 
  $block->content = $wrapper;

  return $block;
}

