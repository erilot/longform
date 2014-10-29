<?php
/**
 * Created by PhpStorm.
 * User: ericlotze
 * Date: 2014-10-16
 * Time: 6:19 PM
 */


function longform_translation_toolbar_machine($node) {

  drupal_add_library('system', 'drupal.ajax');

  //translatable strings
  $block_title = t("Languages");
  $machine_translation_title = t("Machine Translations");

  $output = '<div id="machine-translations" class="translation-block menu-drop-options">';

  // If the node is flagged as non-translatable, return without generating anything
  if ($node->type != 'book' || flag_get_flag('notranslate')->is_flagged($node->nid)) {
    $output .= '<ul class="nav nav-pills nav-stacked"><li class="disabled">This page does not allow machine translations.</li></ul></div>';

    return $output;
  }

  //if GT cookie exists, don't load disclaimer
  if (!$_COOKIE['google-translate-ok']) {
    $output .= '<ul class="machine-options nav nav-pills nav-stacked prevent-collapse"><li id="gt-disclaimer">' . gt_disclaimer() . '</a></li> </ul>' . gt_widget();
  }
  else {
    $output .= gt_widget();
  }

  $output .= '</div>';

  return $output;
}

function longform_translation_toolbar_formal($node) {

  drupal_add_library('system', 'drupal.ajax');

  //translatable strings
  $formal_translation_title = t("Formal Translations");
  $formal_translation_options = longform_get_node_languages();

  if (!$formal_translation_options['items']) {
    $sad = t('None');
    $formal_translation_options = "<li class=\"disabled\"><a>$sad</a></li>";
  }
  else {
    $formal_translation_options = theme_item_list($formal_translation_options);
  }

  $formal_block = '<div id="formal-translations" class="translation-block menu-drop-options"> ' . $formal_translation_options . '</div>';

  $ls_output = '<div id="language-options" class="menu-drop-options-container">';
  $ls_output .= $formal_block;
  $ls_output .= '</div>';

  return $ls_output;
}
