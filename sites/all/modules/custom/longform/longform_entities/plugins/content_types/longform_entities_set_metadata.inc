<?php

$plugin = array(
  'single' => TRUE,  // Just do this one, it is needed.
  'title' => t('View and manage set metadata'),
  'description' => t('View and manage set metadata'),
  'category' => t('Longform'), 
  'edit form' => 'longform_entities_manage_set_metadata_form', // A function that will return the settings form for the pane.
  'render callback' => 'longform_entities_manage_set_metadata_render', // A function that will return the renderable content.
//  'admin info' => 'longform_entities_pane_custom_pane_admin_info', // A function that will return the information displayed on the admin screen (optional).
  'defaults' => array( // Array of defaults for the settings form.
    //'button_label' => t('Add new tickets'),
  ),
  'all contexts' => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function longform_entities_manage_set_metadata_form($form, &$form_state) {
  $conf = $form_state['conf'];
  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets saved
 * Notice, the magic that automatically does that for you.
 */
function longform_entities_manage_set_metadata_form_submit(&$form, &$form_state) {
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
function longform_entities_manage_set_metadata_render($subtype, $conf, $args, $contexts) {

  global $user;
  $user_is_admin = user_access('administer tickets', $user);

  // Include underscore.js
  libraries_load('underscore');

  // Include CSS
  drupal_add_css(drupal_get_path('module', 'longform_entities') . '/css/metadata_blocks.style.css');

  $context = reset($contexts);
  $set = $context->data;
  $products = longform_entities_manage_product_field($set, $user, $user_is_admin);
  $publish_date = longform_entities_manage_publish_date_field($set, $user, $user_is_admin);
  $release = longform_entities_manage_release_field($set, $user, $user_is_admin);

  $content = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('row'),
    ),
  );

  $content['products'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-3', 'col-md-offset-3'),
    ),
  );
  $content['products']['field'] = $products;

  $content['publish_date'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-3'),
    ),
  );
  $content['publish_date']['field'] = $publish_date;

  $content['release'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-3'),
    ),
  );
  $content['release']['field'] = $release;

  $block = new stdClass();
  $block->title = '';
  $block->content = $content;

  return $block;
}


function longform_entities_manage_release_field($set, $user, $admin) {
  $out['wrap'] = _longform_entities_metadata_block_wrapper();

  $wrap = entity_metadata_wrapper($set->type, $set);
  $edit_button = '
    <div class="edit-trigger pull-right">
        <a data-toggle="collapse" data-target="#edit-release-form" class="btn btn-default btn-sm">
            <span class="glyphicon glyphicon-edit"></span>
        </a>
    </div>';

  if ($admin) {
    $out['wrap']['display'] =  array(
      '#markup' => '<h2><span class="text-muted">' . t('Release') . '</span>' .  $edit_button . '</h2>',
    );
  }
  else{
    $out['wrap']['display'] =  array(
      '#markup' => '<h2><span class="text-muted">' . t('Release') . '</span><br/>' . $wrap->field_software_release->value()->name . '</h2>',
    );
  }

  if (!$admin) {
    return $out;
  }

  // Otherwise, add the edit form.
  $out['wrap']['form'] = drupal_get_form('longform_entities_manage_release_edit_form', $set);

  return $out;
}

function longform_entities_manage_publish_date_field($set, $user, $admin) {
  $out['wrap'] = _longform_entities_metadata_block_wrapper();

  $wrap = entity_metadata_wrapper($set->type, $set);
  $date = date('M. d, Y', $wrap->field_publish_date->value());

  $edit_button = '
    <div class="edit-trigger pull-right">
        <a data-toggle="collapse" data-target="#edit-publish-date-form" class="btn btn-default btn-sm">
            <span class="glyphicon glyphicon-edit"></span>
        </a>
    </div>';

  if ($admin) {
    $out['wrap']['display'] =  array(
      '#markup' => '<h2><span class="text-muted">' . t('Publish Date') . '</span>' .  $edit_button . '</h2>',
    );
  }
  else{
    $out['wrap']['display'] =  array(
      '#markup' => '<h2><span class="text-muted">' . t('Publish Date') . '</span><br/>' . $date . '</h2>',
    );
    return $out;
  }

  // Otherwise, add the edit form.
  $out['wrap']['form'] = drupal_get_form('longform_entities_manage_publish_date_edit_form', $set);

  return $out;
}

function longform_entities_manage_product_field($set, $user, $admin) {

  $out['wrap'] = _longform_entities_metadata_block_wrapper();

  $modal_trigger = '<a class="btn btn-default pull-right" data-toggle="modal" data-target="#edit-products-modal"><span class="glyphicon glyphicon-edit"></span></a>';


  if ($admin) {
    $wrap = entity_metadata_wrapper($set->type, $set);
    $products = array();
    foreach($wrap->field_product->getIterator() as $product) {
      $products[] = $product->value()->name;
    }
    $product_list = '<h4><ul class="nav nav-stacked"><li>' . implode('</li><li>', longform_entities_semantic_list($products)) . '</li></ul></h4>';
    $out['wrap']['title'] = array(
      '#markup' => '<h2 class="text-muted">' . t('Products') . ($admin == TRUE ? $modal_trigger : ''). '</h2>' ,
    );
    $out['wrap']['products'] = array(
      '#markup' => $product_list,
    );
    $out['modal'] = drupal_get_form('longform_entities_manage_products_form', $set);
  }


  return $out;
}

function longform_entities_semantic_list($elements) {
  switch (count($elements)) {
    case '0':
      return FALSE;
      break;

    case '1':
      return $elements;
      break;

    case '2':
      $elements[0].= '<span class="text-muted">, ' . t('and') . '</span>';
      return $elements;
      break;

    default:
      $el = implode('<span class="text-muted">,</span>***', $elements);
      $elements = explode('***', $el);
      end($elements);
      prev($elements);
      $elements[key($elements)] .= ' <span class="text-muted">' . t('and') . '</span>';
      return $elements;
  }
}

// ======= Helper functions

/**
 * Helper function that generates a wrapper for a metadata block.
 *
 * By default, this wrapper is a small bootstrap well.
 *
 * @param null $classes
 *  Optional array of classes to use instead of the default; e.g., array('alert','alert-danger')
 *
 * @return array
 *  Render array element.
 */
function _longform_entities_metadata_block_wrapper($classes = NULL) {
  return array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => !isset($classes) ? array('well', 'well-sm', 'metadata-block') : array_merge($classes, array('metadata-block')),
    ),
  );
}
