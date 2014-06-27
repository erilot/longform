<?php
/**
 * @file
 * document_manage_core_feature.features.wysiwyg.inc
 */

/**
 * Implements hook_wysiwyg_default_profiles().
 */
function document_manage_core_feature_wysiwyg_default_profiles() {
  $profiles = array();

  // Exported profile: 1
  $profiles[1] = array(
    'format' => 1,
    'editor' => 'ckeditor_extended',
    'settings' => FALSE,
  );

  // Exported profile: 2
  $profiles[2] = array(
    'format' => 2,
    'editor' => 'ckeditor_extended',
    'settings' => array(
      'default' => TRUE,
      'user_choose' => FALSE,
      'show_toggle' => FALSE,
      'theme' => 'advanced',
      'language' => 'en',
      'block_formats' => '',
      'toolbar' => array(
        0 => array(
          0 => 'Source',
          1 => '|',
          2 => 'Bold',
          3 => 'Italic',
          4 => '|',
          5 => 'NumberedList',
          6 => 'BulletedList',
          7 => 'Blockquote',
          8 => 'Table',
          9 => 'SpecialChar',
          10 => 'DrupalImage',
          11 => '|',
          12 => 'JustifyLeft',
          13 => 'JustifyCenter',
          14 => 'JustifyRight',
          15 => '|',
          16 => 'Link',
          17 => 'Unlink',
          18 => '|',
          19 => 'Paste',
          20 => 'PasteFromWord',
          21 => '|',
          22 => 'Maximize',
        ),
      ),
      'format_list' => array(
        0 => 'p',
        1 => 'h1',
        2 => 'h2',
        3 => 'h3',
        4 => 'h4',
        5 => 'h5',
        6 => 'h6',
      ),
      'style_list' => array(
        0 => '',
      ),
    ),
  );

  return $profiles;
}
