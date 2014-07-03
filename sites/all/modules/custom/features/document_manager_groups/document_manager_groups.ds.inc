<?php
/**
 * @file
 * document_manager_groups.ds.inc
 */

/**
 * Implements hook_ds_field_settings_info().
 */
function document_manager_groups_ds_field_settings_info() {
  $export = array();

  $ds_fieldsetting = new stdClass();
  $ds_fieldsetting->api_version = 1;
  $ds_fieldsetting->id = 'node|documentation_group|default';
  $ds_fieldsetting->entity_type = 'node';
  $ds_fieldsetting->bundle = 'documentation_group';
  $ds_fieldsetting->view_mode = 'default';
  $ds_fieldsetting->settings = array(
    'og_members_block' => array(
      'weight' => '0',
      'label' => 'hidden',
      'format' => 'default',
    ),
  );
  $export['node|documentation_group|default'] = $ds_fieldsetting;

  return $export;
}

/**
 * Implements hook_ds_layout_settings_info().
 */
function document_manager_groups_ds_layout_settings_info() {
  $export = array();

  $ds_layout = new stdClass();
  $ds_layout->api_version = 1;
  $ds_layout->id = 'node|documentation_group|default';
  $ds_layout->entity_type = 'node';
  $ds_layout->bundle = 'documentation_group';
  $ds_layout->view_mode = 'default';
  $ds_layout->layout = 'panels-two_brick';
  $ds_layout->settings = array(
    'regions' => array(
      'two_brick_left_above' => array(
        0 => 'og_members_block',
      ),
      'two_brick_top' => array(
        1 => 'group_group',
        2 => 'body',
      ),
    ),
    'fields' => array(
      'og_members_block' => 'two_brick_left_above',
      'group_group' => 'two_brick_top',
      'body' => 'two_brick_top',
    ),
    'classes' => array(
      'layout_class' => array(
        '' => '',
      ),
    ),
    'wrappers' => array(
      'two_brick_top' => 'div',
      'two_brick_left_above' => 'div',
      'two_brick_right_above' => 'div',
      'two_brick_middle' => 'div',
      'two_brick_left_below' => 'div',
      'two_brick_right_below' => 'div',
      'two_brick_bottom' => 'div',
    ),
    'layout_wrapper' => 'div',
    'layout_attributes' => '',
    'layout_attributes_merge' => 1,
  );
  $export['node|documentation_group|default'] = $ds_layout;

  return $export;
}
