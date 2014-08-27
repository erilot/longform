<?php
/**
 * @file
 * technet_translation_tools.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function technet_translation_tools_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "relation" && $api == "relation_type_default") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_default_tmgmt_translator().
 */
function technet_translation_tools_default_tmgmt_translator() {
  $items = array();
  $items['file'] = entity_import('tmgmt_translator', '{
    "name" : "file",
    "label" : "File translator (auto created)",
    "description" : "File translator that exports and imports files.",
    "weight" : "0",
    "plugin" : "file",
    "settings" : {
      "auto_accept" : 1,
      "export_format" : "xlf",
      "xliff_processing" : 0,
      "allow_override" : 1,
      "scheme" : "public",
      "remote_languages_mappings" : {
        "en" : "en",
        "zh-hans" : "zh-hans",
        "zh-hant" : "zh-hant",
        "fr" : "fr",
        "de" : "de",
        "it" : "it",
        "ja" : "ja",
        "ko" : "ko",
        "nb" : "nb",
        "pl" : "pl",
        "pt-br" : "pt-br",
        "ru" : "ru",
        "es" : "es"
      }
    }
  }');
  return $items;
}
