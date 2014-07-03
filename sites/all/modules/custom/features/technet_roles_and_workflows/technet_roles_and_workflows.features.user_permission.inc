<?php
/**
 * @file
 * technet_roles_and_workflows.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function technet_roles_and_workflows_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'access workflow summary views'.
  $permissions['access workflow summary views'] = array(
    'name' => 'access workflow summary views',
    'roles' => array(
      'Content Publisher' => 'Content Publisher',
      'HTML Approver' => 'HTML Approver',
      'HTML Editor' => 'HTML Editor',
      'HTML Importer' => 'HTML Importer',
      'HTML Reviewer' => 'HTML Reviewer',
      'Master Admin' => 'Master Admin',
      'Super Administrator' => 'Super Administrator',
      'Translation Manager' => 'Translation Manager',
    ),
    'module' => 'workflow_views',
  );

  // Exported permission: 'administer workflow'.
  $permissions['administer workflow'] = array(
    'name' => 'administer workflow',
    'roles' => array(
      'Master Admin' => 'Master Admin',
      'Super Administrator' => 'Super Administrator',
    ),
    'module' => 'workflow_admin_ui',
  );

  // Exported permission: 'participate in workflow'.
  $permissions['participate in workflow'] = array(
    'name' => 'participate in workflow',
    'roles' => array(
      'Basic User' => 'Basic User',
      'Content Publisher' => 'Content Publisher',
      'HTML Approver' => 'HTML Approver',
      'HTML Editor' => 'HTML Editor',
      'HTML Importer' => 'HTML Importer',
      'HTML Reviewer' => 'HTML Reviewer',
      'Master Admin' => 'Master Admin',
      'Super Administrator' => 'Super Administrator',
      'Translation Manager' => 'Translation Manager',
      'anonymous user' => 'anonymous user',
      'authenticated user' => 'authenticated user',
    ),
    'module' => 'workflow_admin_ui',
  );

  // Exported permission: 'schedule workflow transitions'.
  $permissions['schedule workflow transitions'] = array(
    'name' => 'schedule workflow transitions',
    'roles' => array(
      'Content Publisher' => 'Content Publisher',
      'HTML Approver' => 'HTML Approver',
      'HTML Editor' => 'HTML Editor',
      'HTML Importer' => 'HTML Importer',
      'Master Admin' => 'Master Admin',
      'Super Administrator' => 'Super Administrator',
    ),
    'module' => 'workflow',
  );

  // Exported permission: 'show workflow state form'.
  $permissions['show workflow state form'] = array(
    'name' => 'show workflow state form',
    'roles' => array(),
    'module' => 'workflow',
  );

  return $permissions;
}
