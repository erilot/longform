<?php
/**
 * @file
 * document_manager_groups.features.og_features_permission.inc
 */

/**
 * Implements hook_og_features_default_permissions().
 */
function document_manager_groups_og_features_default_permissions() {
  $permissions = array();

  // Exported og permission: 'node:documentation_group:add user'
  $permissions['node:documentation_group:add user'] = array(
    'roles' => array(
      'administrator member' => 'administrator member',
    ),
  );

  // Exported og permission: 'node:documentation_group:administer group'
  $permissions['node:documentation_group:administer group'] = array(
    'roles' => array(
      'administrator member' => 'administrator member',
    ),
  );

  // Exported og permission: 'node:documentation_group:approve and deny subscription'
  $permissions['node:documentation_group:approve and deny subscription'] = array(
    'roles' => array(
      'administrator member' => 'administrator member',
    ),
  );

  // Exported og permission: 'node:documentation_group:create announcement content'
  $permissions['node:documentation_group:create announcement content'] = array(
    'roles' => array(),
  );

  // Exported og permission: 'node:documentation_group:delete any announcement content'
  $permissions['node:documentation_group:delete any announcement content'] = array(
    'roles' => array(),
  );

  // Exported og permission: 'node:documentation_group:delete own announcement content'
  $permissions['node:documentation_group:delete own announcement content'] = array(
    'roles' => array(),
  );

  // Exported og permission: 'node:documentation_group:manage members'
  $permissions['node:documentation_group:manage members'] = array(
    'roles' => array(
      'administrator member' => 'administrator member',
    ),
  );

  // Exported og permission: 'node:documentation_group:manage permissions'
  $permissions['node:documentation_group:manage permissions'] = array(
    'roles' => array(
      'administrator member' => 'administrator member',
    ),
  );

  // Exported og permission: 'node:documentation_group:manage roles'
  $permissions['node:documentation_group:manage roles'] = array(
    'roles' => array(
      'administrator member' => 'administrator member',
    ),
  );

  // Exported og permission: 'node:documentation_group:subscribe'
  $permissions['node:documentation_group:subscribe'] = array(
    'roles' => array(
      'non-member' => 'non-member',
    ),
  );

  // Exported og permission: 'node:documentation_group:subscribe without approval'
  $permissions['node:documentation_group:subscribe without approval'] = array(
    'roles' => array(),
  );

  // Exported og permission: 'node:documentation_group:unsubscribe'
  $permissions['node:documentation_group:unsubscribe'] = array(
    'roles' => array(
      'member' => 'member',
    ),
  );

  // Exported og permission: 'node:documentation_group:update any announcement content'
  $permissions['node:documentation_group:update any announcement content'] = array(
    'roles' => array(),
  );

  // Exported og permission: 'node:documentation_group:update group'
  $permissions['node:documentation_group:update group'] = array(
    'roles' => array(
      'administrator member' => 'administrator member',
    ),
  );

  // Exported og permission: 'node:documentation_group:update own announcement content'
  $permissions['node:documentation_group:update own announcement content'] = array(
    'roles' => array(),
  );

  return $permissions;
}
