<?php
/**
 * @file
 * document_manage_core_feature.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function document_manage_core_feature_default_rules_configuration() {
  $items = array();
  $items['rules_change_update_book_metadata'] = entity_import('rules_config', '{ "rules_change_update_book_metadata" : {
      "LABEL" : "Change\\/Update Book Metadata",
      "PLUGIN" : "action set",
      "OWNER" : "rules",
      "USES VARIABLES" : { "node" : { "label" : "Node", "type" : "node" } },
      "ACTION SET" : []
    }
  }');
  $items['rules_redirect_on_login'] = entity_import('rules_config', '{ "rules_redirect_on_login" : {
      "LABEL" : "Redirect on login",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "users" ],
      "REQUIRES" : [ "rules" ],
      "ON" : { "user_login" : [] },
      "DO" : []
    }
  }');
  $items['rules_user_logs_in'] = entity_import('rules_config', '{ "rules_user_logs_in" : {
      "LABEL" : "User logs in",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "user" ],
      "REQUIRES" : [ "rules" ],
      "ON" : { "user_login" : [] },
      "IF" : [
        { "NOT text_matches" : { "text" : [ "site:current-page:url" ], "match" : "user\\/reset" } }
      ],
      "DO" : [ { "redirect" : { "url" : "welcome", "force" : 0, "destination" : 1 } } ]
    }
  }');
  return $items;
}
