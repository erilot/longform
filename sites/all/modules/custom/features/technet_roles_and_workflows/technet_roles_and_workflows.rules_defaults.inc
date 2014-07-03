<?php
/**
 * @file
 * technet_roles_and_workflows.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function technet_roles_and_workflows_default_rules_configuration() {
  $items = array();
  $items['rules_set_import_flag'] = entity_import('rules_config', '{ "rules_set_import_flag" : {
      "LABEL" : "Set Import Flag",
      "PLUGIN" : "reaction rule",
      "ACTIVE" : false,
      "OWNER" : "rules",
      "TAGS" : [ "document import" ],
      "REQUIRES" : [ "workflow_rules", "flag" ],
      "ON" : { "workflow_state_changed" : [] },
      "IF" : [
        { "workflow_check_state" : { "node" : [ "node" ], "workflow_state" : { "value" : { "7" : "7" } } } }
      ],
      "DO" : [
        { "flag_flagnode" : {
            "flag" : "queued_for_import",
            "node" : [ "node" ],
            "flagging_user" : [ "site:current-user" ],
            "permission_check" : 1
          }
        }
      ]
    }
  }');
  return $items;
}
