<?php
/**
 * @file
 * document_manage_core_feature.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function document_manage_core_feature_user_default_roles() {
  $roles = array();

  // Exported role: Basic User.
  $roles['Basic User'] = array(
    'name' => 'Basic User',
    'weight' => 2,
  );

  // Exported role: Content Approver.
  $roles['Content Approver'] = array(
    'name' => 'Content Approver',
    'weight' => 6,
  );

  // Exported role: Content Publisher.
  $roles['Content Publisher'] = array(
    'name' => 'Content Publisher',
    'weight' => 7,
  );

  // Exported role: HTML Editor.
  $roles['HTML Editor'] = array(
    'name' => 'HTML Editor',
    'weight' => 4,
  );

  // Exported role: Importer.
  $roles['Importer'] = array(
    'name' => 'Importer',
    'weight' => 3,
  );

  // Exported role: Super Administrator.
  $roles['Super Administrator'] = array(
    'name' => 'Super Administrator',
    'weight' => 12,
  );

  // Exported role: Translation Manager.
  $roles['Translation Manager'] = array(
    'name' => 'Translation Manager',
    'weight' => 8,
  );

  // Exported role: anonymous user.
  $roles['anonymous user'] = array(
    'name' => 'anonymous user',
    'weight' => 0,
  );

  // Exported role: authenticated user.
  $roles['authenticated user'] = array(
    'name' => 'authenticated user',
    'weight' => 1,
  );

  return $roles;
}
