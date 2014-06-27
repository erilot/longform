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
  $workflows['Book Node Status']['Submitted'] = array();
  $workflows['Book Node Status']['Submitted']['Importer'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Approved'] = array();
  $workflows['Book Node Status']['Approved']['anonymous user'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Approved']['authenticated user'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Approved']['Basic User'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Approved']['Importer'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Approved']['HTML Editor'] = array(
    'grant_view' => 0,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Book Node Status']['Approved']['Content Approver'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Approved']['Content Publisher'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Approved']['Translation Manager'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Approved']['Super Administrator'] = array(
    'grant_view' => 0,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Book Node Status']['Approved']['workflow_features_author_name'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Book Node Status']['Published'] = array();
  $workflows['Book Node Status']['Published']['Content Publisher'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );

  $workflows['Import and Publish'] = array();
  $workflows['Import and Publish']['Created'] = array();
  $workflows['Import and Publish']['Created']['Importer'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Created']['HTML Editor'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Created']['Content Approver'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Created']['Content Publisher'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Created']['Translation Manager'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Created']['Super Administrator'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Created']['workflow_features_author_name'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Created']['anonymous user'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Created']['authenticated user'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Created']['Basic User'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Importing'] = array();
  $workflows['Import and Publish']['Importing']['Importer'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Importing']['HTML Editor'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Importing']['Content Approver'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Importing']['Content Publisher'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Importing']['Translation Manager'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Importing']['Super Administrator'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Importing']['workflow_features_author_name'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Importing']['anonymous user'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Importing']['authenticated user'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Importing']['Basic User'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Reviewing'] = array();
  $workflows['Import and Publish']['Reviewing']['Importer'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Reviewing']['HTML Editor'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Reviewing']['Content Approver'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Reviewing']['Content Publisher'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Reviewing']['Super Administrator'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Reviewing']['workflow_features_author_name'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Reviewing']['anonymous user'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Reviewing']['authenticated user'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Reviewing']['Basic User'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Reviewing']['Translation Manager'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved'] = array();
  $workflows['Import and Publish']['Approved']['Importer'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved']['HTML Editor'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved']['Content Approver'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved']['Content Publisher'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved']['Translation Manager'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved']['Super Administrator'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Approved']['workflow_features_author_name'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved']['anonymous user'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved']['authenticated user'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Approved']['Basic User'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published'] = array();
  $workflows['Import and Publish']['Published']['anonymous user'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published']['authenticated user'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published']['Importer'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published']['HTML Editor'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published']['Content Approver'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published']['Content Publisher'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published']['Translation Manager'] = array(
    'grant_view' => 1,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published']['Super Administrator'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );
  $workflows['Import and Publish']['Published']['Basic User'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );
  $workflows['Import and Publish']['Published']['workflow_features_author_name'] = array(
    'grant_view' => 0,
    'grant_update' => 0,
    'grant_delete' => 0,
  );

  $workflows['Topic Status'] = array();
  $workflows['Topic Status']['Published'] = array();
  $workflows['Topic Status']['Published']['Super Administrator'] = array(
    'grant_view' => 1,
    'grant_update' => 1,
    'grant_delete' => 1,
  );

  return $workflows;
}
