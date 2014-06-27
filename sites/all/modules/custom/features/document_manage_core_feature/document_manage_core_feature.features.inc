<?php
/**
 * @file
 * document_manage_core_feature.features.inc
 */

/**
 * Implements hook_flag_default_flags().
 */
function document_manage_core_feature_flag_default_flags() {
  $flags = array();
  // Exported flag: "HTML Problem Alert".
  $flags['html_problem_alert'] = array(
    'entity_type' => 'node',
    'title' => 'HTML Problem Alert',
    'global' => 1,
    'types' => array(
      0 => 'book',
      1 => 'topic',
    ),
    'flag_short' => '<span class="glyphicon glyphicon-flag"></span> <span class="lg">HTML is ok</span><span class="sm">HTML ok</span>',
    'flag_long' => 'Help! This HTML has a problem I can\'t fix.',
    'flag_message' => 'This page has been flagged. <em>Please leave a comment below explaining the problem.</em>',
    'unflag_short' => '<span class="glyphicon glyphicon-flag"></span> <span class="lg">HTML help!</span><span class="sm">Help!</span>',
    'unflag_long' => 'When this page is fixed, remove the flag.',
    'unflag_message' => 'The problem flag has been removed. <em>Please leave a comment below explaining the solution.</em>',
    'unflag_denied_text' => '',
    'link_type' => 'toggle',
    'weight' => 0,
    'show_in_links' => array(),
    'show_as_field' => FALSE,
    'show_on_form' => 0,
    'access_author' => '',
    'show_contextual_link' => FALSE,
    'i18n' => 0,
    'api_version' => 3,
    'module' => 'document_manage_core_feature',
    'locked' => array(
      0 => 'name',
    ),
  );
  return $flags;

}

/**
 * Implements hook_node_info().
 */
function document_manage_core_feature_node_info() {
  $items = array(
    'book' => array(
      'name' => t('Book page'),
      'base' => 'node_content',
      'description' => t('<p>
	A <em>book page</em> is a page of content, organized into a collection of related entries collectively known as a <em>book</em>. A <em>book page</em> automatically displays links to adjacent pages, providing a simple navigation system for organizing and reviewing structured content.</p>
'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'import_ticket' => array(
      'name' => t('Import Ticket'),
      'base' => 'node_content',
      'description' => t('This is a meta-content type designed to track and process document imports. Each document to be imported is attached to an import ticket record, which is then processed and advanced through workflow stages.'),
      'has_title' => '1',
      'title_label' => t('Document Title'),
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implements hook_workflow_default_workflows().
 */
function document_manage_core_feature_workflow_default_workflows() {
  $workflows = array();

  // Exported workflow: Book Node Status
  $workflows['Book Node Status'] = array(
    'name' => 'Book Node Status',
    'tab_roles' => '',
    'options' => 'a:4:{s:16:"comment_log_node";i:0;s:15:"comment_log_tab";i:0;s:13:"name_as_title";i:0;s:12:"watchdog_log";i:1;}',
    '' . "\0" . 'Workflow' . "\0" . 'creation_sid' => 11,
    '' . "\0" . 'Workflow' . "\0" . 'creation_state' => NULL,
    '' . "\0" . 'Workflow' . "\0" . 'item' => NULL,
    'states' => array(
      0 => array(
        'state' => '(creation)',
        'weight' => -50,
        'sysid' => 1,
        'status' => 1,
        'name' => 'Book Node Status',
      ),
      1 => array(
        'state' => 'In Review',
        'weight' => -19,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Book Node Status',
      ),
      2 => array(
        'state' => 'Submitted',
        'weight' => -18,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Book Node Status',
      ),
      3 => array(
        'state' => 'Approved',
        'weight' => -17,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Book Node Status',
      ),
      4 => array(
        'state' => 'Published',
        'weight' => -16,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Book Node Status',
      ),
    ),
    'transitions' => array(
      0 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'In Review',
      ),
      1 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Submitted',
      ),
      2 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Approved',
      ),
      3 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Published',
      ),
      4 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'In Review',
        'target_state' => 'Approved',
      ),
      5 => array(
        'roles' => 'HTML Editor,Content Publisher,Super Administrator',
        'state' => 'In Review',
        'target_state' => 'Submitted',
      ),
      6 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'In Review',
        'target_state' => 'Published',
      ),
      7 => array(
        'roles' => 'HTML Editor,Content Approver,Content Publisher,Super Administrator',
        'state' => 'Submitted',
        'target_state' => 'In Review',
      ),
      8 => array(
        'roles' => 'Content Approver,Content Publisher,Super Administrator',
        'state' => 'Submitted',
        'target_state' => 'Approved',
      ),
      9 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Submitted',
        'target_state' => 'Published',
      ),
      10 => array(
        'roles' => 'Content Approver,Content Publisher,Super Administrator',
        'state' => 'Approved',
        'target_state' => 'In Review',
      ),
      11 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Approved',
        'target_state' => 'Submitted',
      ),
      12 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Approved',
        'target_state' => 'Published',
      ),
      13 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Published',
        'target_state' => 'In Review',
      ),
      14 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Published',
        'target_state' => 'Submitted',
      ),
      15 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Published',
        'target_state' => 'Approved',
      ),
    ),
    'node_types' => array(
      0 => 'book',
    ),
  );

  // Exported workflow: Import and Publish
  $workflows['Import and Publish'] = array(
    'name' => 'Import and Publish',
    'tab_roles' => 'author,6,7,11,10,8,3,16',
    'options' => 'a:4:{s:16:"comment_log_node";i:1;s:15:"comment_log_tab";i:1;s:13:"name_as_title";i:0;s:12:"watchdog_log";i:1;}',
    '' . "\0" . 'Workflow' . "\0" . 'creation_sid' => 1,
    '' . "\0" . 'Workflow' . "\0" . 'creation_state' => NULL,
    '' . "\0" . 'Workflow' . "\0" . 'item' => NULL,
    'states' => array(
      0 => array(
        'state' => '(creation)',
        'weight' => -50,
        'sysid' => 1,
        'status' => 1,
        'name' => 'Import and Publish',
      ),
      1 => array(
        'state' => 'Created',
        'weight' => -19,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Import and Publish',
      ),
      2 => array(
        'state' => 'Importing',
        'weight' => -18,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Import and Publish',
      ),
      3 => array(
        'state' => 'Reviewing',
        'weight' => -14,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Import and Publish',
      ),
      4 => array(
        'state' => 'Approved',
        'weight' => -13,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Import and Publish',
      ),
      5 => array(
        'state' => 'Published',
        'weight' => -12,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Import and Publish',
      ),
    ),
    'transitions' => array(
      0 => array(
        'roles' => 'Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Approved',
      ),
      1 => array(
        'roles' => 'Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Published',
      ),
      2 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      3 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      4 => array(
        'roles' => 'workflow_features_author_name,Importer,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Created',
      ),
      5 => array(
        'roles' => 'Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Importing',
      ),
      6 => array(
        'roles' => 'Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Reviewing',
      ),
      7 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      8 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      9 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      10 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      11 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      12 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      13 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      14 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      15 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      16 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => NULL,
      ),
      17 => array(
        'roles' => 'Importer,Super Administrator',
        'state' => 'Created',
        'target_state' => 'Importing',
      ),
      18 => array(
        'roles' => 'Super Administrator',
        'state' => 'Created',
        'target_state' => 'Reviewing',
      ),
      19 => array(
        'roles' => 'Super Administrator',
        'state' => 'Created',
        'target_state' => 'Approved',
      ),
      20 => array(
        'roles' => 'Super Administrator',
        'state' => 'Created',
        'target_state' => 'Published',
      ),
      21 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      22 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      23 => array(
        'roles' => 'Importer,Super Administrator',
        'state' => 'Importing',
        'target_state' => 'Created',
      ),
      24 => array(
        'roles' => 'Importer,Super Administrator',
        'state' => 'Importing',
        'target_state' => 'Reviewing',
      ),
      25 => array(
        'roles' => 'Super Administrator',
        'state' => 'Importing',
        'target_state' => 'Approved',
      ),
      26 => array(
        'roles' => 'Super Administrator',
        'state' => 'Importing',
        'target_state' => 'Published',
      ),
      27 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      28 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      29 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      30 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      31 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      32 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      33 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      34 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      35 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      36 => array(
        'roles' => 'workflow_features_author_name,Importer,Content Publisher,Super Administrator',
        'state' => 'Importing',
        'target_state' => NULL,
      ),
      37 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      38 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      39 => array(
        'roles' => 'Super Administrator',
        'state' => 'Reviewing',
        'target_state' => 'Created',
      ),
      40 => array(
        'roles' => 'Super Administrator',
        'state' => 'Reviewing',
        'target_state' => 'Importing',
      ),
      41 => array(
        'roles' => 'Content Approver,Content Publisher,Super Administrator',
        'state' => 'Reviewing',
        'target_state' => 'Approved',
      ),
      42 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Reviewing',
        'target_state' => 'Published',
      ),
      43 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      44 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      45 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      46 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      47 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      48 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      49 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      50 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      51 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      52 => array(
        'roles' => 'Content Publisher',
        'state' => 'Reviewing',
        'target_state' => NULL,
      ),
      53 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Approved',
        'target_state' => 'Published',
      ),
      54 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      55 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      56 => array(
        'roles' => 'Super Administrator',
        'state' => 'Approved',
        'target_state' => 'Created',
      ),
      57 => array(
        'roles' => 'Super Administrator',
        'state' => 'Approved',
        'target_state' => 'Importing',
      ),
      58 => array(
        'roles' => 'Content Approver,Super Administrator',
        'state' => 'Approved',
        'target_state' => 'Reviewing',
      ),
      59 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      60 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      61 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      62 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      63 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      64 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      65 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      66 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      67 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      68 => array(
        'roles' => 'Content Publisher',
        'state' => 'Approved',
        'target_state' => NULL,
      ),
      69 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Published',
        'target_state' => 'Approved',
      ),
      70 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      71 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      72 => array(
        'roles' => 'Super Administrator',
        'state' => 'Published',
        'target_state' => 'Created',
      ),
      73 => array(
        'roles' => 'Super Administrator',
        'state' => 'Published',
        'target_state' => 'Importing',
      ),
      74 => array(
        'roles' => 'Super Administrator',
        'state' => 'Published',
        'target_state' => 'Reviewing',
      ),
      75 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      76 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      77 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      78 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      79 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      80 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      81 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      82 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      83 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
      84 => array(
        'roles' => '',
        'state' => 'Published',
        'target_state' => NULL,
      ),
    ),
    'node_types' => array(
      0 => 'import_ticket',
    ),
  );

  // Exported workflow: Topic Status
  $workflows['Topic Status'] = array(
    'name' => 'Topic Status',
    'tab_roles' => '8,3,16',
    'options' => 'a:6:{s:13:"name_as_title";s:1:"0";s:8:"schedule";s:1:"0";s:17:"schedule_timezone";s:1:"1";s:16:"comment_log_node";s:1:"1";s:15:"comment_log_tab";s:1:"1";s:12:"watchdog_log";s:1:"0";}',
    '' . "\0" . 'Workflow' . "\0" . 'creation_sid' => 41,
    '' . "\0" . 'Workflow' . "\0" . 'creation_state' => NULL,
    '' . "\0" . 'Workflow' . "\0" . 'item' => NULL,
    'states' => array(
      0 => array(
        'state' => '(creation)',
        'weight' => -50,
        'sysid' => 1,
        'status' => 1,
        'name' => 'Topic Status',
      ),
      1 => array(
        'state' => 'In Review',
        'weight' => -19,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Topic Status',
      ),
      2 => array(
        'state' => 'Submitted',
        'weight' => -18,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Topic Status',
      ),
      3 => array(
        'state' => 'Approved',
        'weight' => -17,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Topic Status',
      ),
      4 => array(
        'state' => 'Published',
        'weight' => -16,
        'sysid' => 0,
        'status' => 1,
        'name' => 'Topic Status',
      ),
    ),
    'transitions' => array(
      0 => array(
        'roles' => 'workflow_features_author_name,Importer,HTML Editor,Content Approver,Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'In Review',
      ),
      1 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Submitted',
      ),
      2 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Approved',
      ),
      3 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => '(creation)',
        'target_state' => 'Published',
      ),
      4 => array(
        'roles' => 'Importer,HTML Editor,Content Approver,Content Publisher,Super Administrator',
        'state' => 'In Review',
        'target_state' => 'Submitted',
      ),
      5 => array(
        'roles' => 'Content Approver,Content Publisher,Super Administrator',
        'state' => 'In Review',
        'target_state' => 'Approved',
      ),
      6 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'In Review',
        'target_state' => 'Published',
      ),
      7 => array(
        'roles' => 'Importer,HTML Editor,Content Approver,Content Publisher,Super Administrator',
        'state' => 'Submitted',
        'target_state' => 'In Review',
      ),
      8 => array(
        'roles' => 'Content Approver,Content Publisher,Super Administrator',
        'state' => 'Submitted',
        'target_state' => 'Approved',
      ),
      9 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Submitted',
        'target_state' => 'Published',
      ),
      10 => array(
        'roles' => 'Content Approver,Content Publisher,Super Administrator',
        'state' => 'Approved',
        'target_state' => 'In Review',
      ),
      11 => array(
        'roles' => 'Content Approver,Content Publisher,Super Administrator',
        'state' => 'Approved',
        'target_state' => 'Submitted',
      ),
      12 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Approved',
        'target_state' => 'Published',
      ),
      13 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Published',
        'target_state' => 'In Review',
      ),
      14 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Published',
        'target_state' => 'Submitted',
      ),
      15 => array(
        'roles' => 'Content Publisher,Super Administrator',
        'state' => 'Published',
        'target_state' => 'Approved',
      ),
    ),
    'node_types' => array(
      0 => 'topic',
    ),
  );

  return $workflows;
}
