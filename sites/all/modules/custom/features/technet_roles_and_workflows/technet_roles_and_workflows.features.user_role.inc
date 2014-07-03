<?php
/**
 * @file
 * technet_roles_and_workflows.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function technet_roles_and_workflows_user_default_roles() {
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

  // Exported role: HTML Approver.
  $roles['HTML Approver'] = array(
    'name' => 'HTML Approver',
    'weight' => 6,
  );

  // Exported role: HTML Editor.
  $roles['HTML Editor'] = array(
    'name' => 'HTML Editor',
    'weight' => 4,
  );

  // Exported role: HTML Importer.
  $roles['HTML Importer'] = array(
    'name' => 'HTML Importer',
    'weight' => 3,
  );

  // Exported role: HTML Reviewer.
  $roles['HTML Reviewer'] = array(
    'name' => 'HTML Reviewer',
    'weight' => 5,
  );

  // Exported role: Importer.
  $roles['Importer'] = array(
    'name' => 'Importer',
    'weight' => 3,
  );

  // Exported role: Master Admin.
  $roles['Master Admin'] = array(
    'name' => 'Master Admin',
    'weight' => 13,
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
