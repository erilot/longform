<?php

/**
 * Site home page building blocks
 *
 */

/**
 * Implements hook_menu()
 *
 * Adds JSON output location for simple rendered views
 */


/**
 * Render the main site dashboard.
 *
 * Add or subtract elements here, based on user role.
 */
function longform_site_dashboard() {

  drupal_add_css(drupal_get_path('module', 'doc_manage') . '/dashboard/css/longform_dashboard.css', array(
      'type' => 'file',
      'every_page' => TRUE
    ));
  drupal_add_js(drupal_get_path('module', 'doc_manage') . '/dashboard/js/longform_dashboard.js', 'file');

  $anonymous = user_is_anonymous();
  $administrator = user_is_administrator();
  global $user;

  $out = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('row', 'technet-dashboard-wrapper'),
    ),
  );

  // This alert is temporary -- remove it after changes have settled in!
  // =================
  $out['alert'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('alert', 'alert-info'),
    ),
  );
  $out['alert']['content'] = array(
    '#markup' => '<label>New Technet Dashboards</label><br/>
				Dashboards are being added to the LSG front page to make your Technet work easier to manage.<br/>
				<b>Important</b>: These dashboards don\'t change your import procedures or workflows; they are just here for easy tracking and follow-up! If you think of a way to make this more useful, or if something important is missing, please pass your feedback on to <a href="mailto:eric.lotze@polycom.com">Eric</a> or <a href="mailto:megg.morin@polycom.com">Megg</a>.',
  );
  // =================

  $out['bar'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12', 'tab-wrapper'),
    ),
  );
  $out['panes'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12', 'tab-content'),
    ),
  );
  $tabs = array();
  $panes = array();

  //foreach(array_reverse($user->roles) as $rid => $role) {
  foreach ($user->roles as $rid => $role) {
    $safe_role = strtolower(str_replace(' ', '_', $role));
    $function = 'longform_dashboard_' . $safe_role;

    // If the role doesn't have a dashboard, skip it.
    if (!function_exists($function)) {
      continue;
    }

    $temp = _get_tab_pair($safe_role, FALSE, 'tab', $role);

    $tabs[] = $temp['tab'];
    $panes[] = $safe_role;
    $out['panes'] += $temp['wrapper'];
    $out['panes'][$safe_role]['content'] = $function($role . ' Dashboard');
  }

  $tab_list = array(
    '#theme' => 'item_list',
    '#type' => 'ul',
    '#items' => $tabs,
    '#title' => NULL,
    '#attributes' => array(
      'class' => array('nav', 'nav-tabs'),
    ),
  );
  $out['bar']['tabs'] = $tab_list;

  // Set the active tab.
  // Right now, this will always be the first available tab.
  // @TODO: make this configurable!
  $out['bar']['tabs']['#items'][0]['class'] = array('active');
  $out['panes'][reset($panes)]['#attributes']['class'][] = 'active in';

  return $out;
}


/**
 * Get the publisher dashboard.
 *
 */
function longform_dashboard_publisher($title) {
  global $user;
  //$opts['title'] = $title;
  $opts['intro']['content'] = array(
    '#markup' => '<b>Welcome to the Technet Publisher dashboard</b>!<br/>This tab provides you with several views for keeping track of your HTML documents. If there\'s something additional you\'d like to see here, please <a href="mailto:eric.lotze@polycom.com">let me know</a>.',
  );
  $opts['panes'] = array(
    'tab-publisher-title' => array(
      'type' => 'title',
      'title' => 'Your Documents'
    ),
    'tab-publisher-stubs' => array(
      'type' => 'view',
      'view' => 'documents_by_user',
      'display' => 'stubs_publisher',
      'title' => '<span class="glyphicon glyphicon-inbox text-muted"></span> ' . t('Stubs'),
      'args' => array($user->uid)
    ),
    //'tab-publisher-divider' => array('type' => 'divider'),
    'tab-publisher-pre-import' => array(
      'type' => 'view',
      'view' => 'documents_by_user',
      'display' => 'pre_import_publisher',
      'title' => '<span class="glyphicon glyphicon-stop text-danger"></span> ' . t('Not Imported'),
      'args' => array($user->uid)
    ),
    'tab-publisher-in-process' => array(
      'type' => 'view',
      'view' => 'documents_by_user',
      'display' => 'in_progress_publisher',
      'title' => '<span class="glyphicon glyphicon-pause text-warning"></span> ' . t('In Review'),
      'args' => array($user->uid)
    ),
    'tab-publisher-recent-docs' => array(
      'type' => 'view',
      'view' => 'documents_by_user',
      'display' => 'recently_published_publisher',
      'title' => '<span class="glyphicon glyphicon-play text-success"></span> ' . t('Published'),
      'args' => array($user->uid)
    ),
  );

  longform_dashboard_add_js($title, $opts);

  return longform_dashboard_pane_create($opts);
}

/**
 * Get the editor dashboard.
 *
 */
function longform_dashboard_editor($title) {
  global $user;
  //$opts['title'] = $title;

  $opts['intro']['content'] = array(
    '#markup' => '<b>Welcome to the Technet editor dashboard</b>!<br/>This tab provides you with several global-scope views for tracking all site content. If there\'s something additional you\'d like to see here, please <a href="mailto:eric.lotze@polycom.com">let me know</a>.',
  );
  $opts['panes'] = array(
    'tab-editor-title' => array('type' => 'title', 'title' => 'All Documents'),
    'tab-editor-stubs' => array(
      'type' => 'view',
      'view' => 'editor_documents_view',
      'display' => 'stubs_by_user',
      'title' => '<span class="glyphicon glyphicon-inbox text-muted"></span> ' . t('Stubs'),
      'args' => array($user->uid)
    ),
    //'tab-editor-divider' => array('type' => 'divider'),
    'tab-editor-pre-import' => array(
      'type' => 'view',
      'view' => 'editor_documents_view',
      'display' => 'pre_import_by_user',
      'title' => '<span class="glyphicon glyphicon-stop text-danger"></span> ' . t('Not Imported'),
      'args' => array($user->uid)
    ),
    'tab-editor-in-process' => array(
      'type' => 'view',
      'view' => 'editor_documents_view',
      'display' => 'in_progress_by_user',
      'title' => '<span class="glyphicon glyphicon-pause text-warning"></span> ' . t('In Review'),
      'args' => array($user->uid)
    ),
    'tab-editor-recent-docs' => array(
      'type' => 'view',
      'view' => 'editor_documents_view',
      'display' => 'recently_published_by_user',
      'title' => '<span class="glyphicon glyphicon-play text-success"></span> ' . t('Published'),
      'args' => array($user->uid)
    ),
  );
  longform_dashboard_add_js($title, $opts);

  return longform_dashboard_pane_create($opts);
}

/**
 * Get the editor dashboard.
 *
 */
function longform_dashboard_technical_support($title) {
  global $user;
  //$opts['title'] = $title;
  $opts['intro']['content'] = array(
    '#markup' => '<b>Welcome to the Technet technical support dashboard</b>!<br/>This tab shows tech support views for following up on HTML Help flags (for now). If there\'s something additional you\'d like to see here, please <a href="mailto:eric.lotze@polycom.com">let me know</a>.',
  );
  $opts['panes'] = array(
    'tab-support-title' => array('type' => 'title', 'title' => 'HTML Flags'),
    'tab-support-alerts-document' => array(
      'type' => 'view',
      'view' => 'tech_support_views',
      'display' => 'all_html_flags_by_document',
      'title' => '<span class="glyphicon glyphicon-book text-danger"></span> ' . t('Help by doc'),
      'args' => array($user->uid)
    ),
    'tab-support-alerts-user' => array(
      'type' => 'view',
      'view' => 'tech_support_views',
      'display' => 'all_html_flags_by_user',
      'title' => '<span class="glyphicon glyphicon-user text-danger"></span> ' . t('Help by user'),
      'args' => array($user->uid)
    ),
  );
  longform_dashboard_add_js($title, $opts);

  return longform_dashboard_pane_create($opts);
}


/**
 * Get the translation manager dashboard.
 *
 * If the role exists but the module is disabled, return an empty array.
 *
 */
function longform_dashboard_translation_manager($title) {
  if (module_exists('longform_translation')) {
    module_load_include('inc', 'longform_translation', 'longform_translation_dashboard');

    return longform_translation_dashboard($title);
  }
}

/**
 * Send options arrays to javascript
 */
function longform_dashboard_add_js($title, $opts) {
  foreach ($opts['panes'] as $id => $pane) {
    if ($pane['divider']) {
      continue;
    }
    $js['longformDashboard'][$id] = array(
      'id' => $id,
      'view' => $pane['view'],
      'display' => $pane['display'],
      'args' => $pane['args']
    );
  }
  drupal_add_js($js, 'setting');
}

/**
 * Built-In Dashboards
 */

/**
 * Get the content publisher dashboard.
 *
 */


function longform_dashboard_pane_create($opts) {

  $out['wrap'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('wrapper'),
    ),
  );

  if ($opts['title']) {
    $out['wrap']['title'] = array(
      '#markup' => '<div class="col-md-12"><h2>' . $opts['title'] . '</h2></div>',
    );
  }

  if ($opts['intro']) {
    $out['wrap']['intro'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('well', 'well-sm'),
      ),
    );
    if (gettype($opts['intro']) == 'string') {
      $out['wrap']['intro']['content'] = array(
        '#markup' => $opts['intro'],
      );
    }
    else {
      $out['wrap']['intro']['content'] = $opts['intro'];
    }
  }

  $out['wrap']['bar'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-2'),
    ),
  );

  $out['wrap']['contents'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('tab-content', 'col-md-10', 'left-rule'),
    ),
  );

  // Create the default pre-loading pane
  $pre_load = _get_tab_pair('pre-load', TRUE, 'pill', 'Pre-load placeholder');
  $out['wrap']['contents'] += $pre_load['wrapper'];
  $out['wrap']['contents']['pre-load'] = longform_dashboard_placeholder_pane();

  foreach ($opts['panes'] as $id => $pane) {

    switch ($pane['type']) {
      case 'divider':
        $tabs[] = array('data' => '', 'class' => array('divider'));
        continue;
        break;

      case 'title':
        $tabs[] = array('data' => '<h4>' . $pane['title'] . '</h4>');
        continue;
        break;

      case 'view':
        $panel = _get_tab_pair($id, $pane['show'], 'pill', $pane['title']);
        $tabs[$id] = $panel['tab'];
        $contents = longform_dashboard_embed_view($pane);
        break;
    }

    // Mark the first pane ID as a backup in case all panes come back empty
    if (!isset($first_pane)) {
      $first_pane = $id;
    }

    // Check to see if the view returned is empty (contains "class='view-empty'").
    // Wait for the first one that is NOT empty, and flag it as active. Note the ID to preset the tab, too.
    if (strpos($contents['wrap']['markup']['#markup'], 'class="view-empty"') === FALSE && !isset($active_pane)) {
      $active_pane = current(array_keys($panel['wrapper']));
      $panel['wrapper'][$id]['#attributes']['class'][] = 'active in';
    }
    else {
      //$tabs[$active_pane]['class'][] = 'disabled';
    }
    $out['wrap']['contents'] += $panel['wrapper'];
    $out['wrap']['contents'][$id]['contents'] = $contents;

  }

  if (isset($active_pane)) {
    //$tabs[$active_pane]['class'][] = 'active';
  }
  else {
    //$tabs[$first_pane]['class'][] = 'active';
    $out['wrap']['contents'][$id]['#attributes']['class'][] = 'active in';
  }

  // Fill in tabs when we're done
  $out['wrap']['bar']['tabs'] = array(
    '#theme' => 'item_list',
    '#items' => $tabs,
    '#type' => 'ul',
    '#attributes' => array(
      'class' => array('nav', 'nav-pills', 'nav-stacked'),
    ),
  );

  return ($out);

}

/**
 * Render active jobs pane
 */
function longform_dashboard_embed_view($opts) {
  $out = array();
  $out['wrap'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('col-md-12', 'dashboard-pane-inner'),
    ),
  );

  return $out;
}

/**
 * Provide a placeholder panel before the first AJAX request is made.
 *
 */
function longform_dashboard_placeholder_pane() {
  $out = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('placeholder'),
    ),
  );
  $out['inner'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('placeholder-content', 'col-md-10'),
    ),
  );
  $out['inner']['contents'] = array(
    '#markup' => '<h5 class="text-muted"><span class="glyphicon glyphicon-arrow-left"></span> To get started, click one of these tabs.</h5>',
  );

  return $out;
}
