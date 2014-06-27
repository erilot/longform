<?php
/**
 * @file
 * document_manage_core_feature.features.og_features_role.inc
 */

/**
 * Implements hook_og_features_default_roles().
 */
function document_manage_core_feature_og_features_default_roles() {
  $roles = array();

  // Exported OG Role: 'node:technet_user_groups:editor'.
  $roles['node:technet_user_groups:editor'] = array(
    'gid' => 0,
    'group_type' => 'node',
    'group_bundle' => 'technet_user_groups',
    'name' => 'editor',
  );

  // Exported OG Role: 'node:technet_user_groups:publisher'.
  $roles['node:technet_user_groups:publisher'] = array(
    'gid' => 0,
    'group_type' => 'node',
    'group_bundle' => 'technet_user_groups',
    'name' => 'publisher',
  );

  return $roles;
}
