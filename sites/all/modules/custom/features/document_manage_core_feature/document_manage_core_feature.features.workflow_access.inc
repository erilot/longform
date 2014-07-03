<?php
/**
 * @file
 * document_manage_core_feature.features.workflow_access.inc
 */

/**
 * Implements hook_workflow_access_features_default_settings().
 */
function document_manage_core_feature_workflow_access_features_default_settings() {
  $workflows = array();

  $workflows['Book Node Status'] = array();

  $workflows['Import and Publish'] = array();

  $workflows['Topic Status'] = array();

  return $workflows;
}
