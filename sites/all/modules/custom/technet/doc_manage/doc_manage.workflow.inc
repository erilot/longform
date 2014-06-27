<?php

/*
 * Block displays workflow state and allows state changes to be made
 */
function shift_workflow($form, &$form_state){
    
  global $user;
  if($user->uid){
    drupal_add_js(drupal_get_path('module','book_nav'). '/topic-edit.js');
    drupal_add_css(drupal_get_path('module','book_nav'). '/workflow-manage.css');
    };
   
  $page = menu_get_object();
  $nid = $page->nid;
  $form_state['storage']['node'] = $page->nid;
    
  if ($workflow = workflow_get_workflow_type_map_by_type($page->type)) {
    $choices = workflow_field_choices($page);
    $workflow = workflow_get_workflows_by_wid($workflow->wid);
    $states = workflow_get_workflow_states_by_wid($workflow->wid);
    $current = workflow_node_current_state($page);

    //perform check on validity of current state, and adjust if needed
    foreach ($states as $temp){
      $compare[] = $temp->sid;
    }
    
    if (!in_array($current,$compare) || $current == 1){
      workflow_execute_transition($page,$compare[1],'State was invalid, and was reset automatically.',TRUE);
      entity_get_controller('node')->resetCache(array($page->nid));

      drupal_set_message('The workflow state for this node was invalid, and was reset automatically. Most likely this happened because of a forced workflow shift by rule. You will need to set the correct state yourself.','warning');
    }
    
    $min = 2; // Our current status, and our new status.
    foreach ($states as $state) {
      if ($state->sid == $current) {
        $min = $state->status == t('(creation)') ? 1 : 2;
      }
    }
    $form['#wf'] = $workflow;
    $name = check_plain($workflow->name);
    // If the current node state is not one of the choices, pick first choice.
    // We know all states in $choices are states that user has permission to
    // go to because workflow_field_choices() has already checked that.
    if (!isset($choices[$current])) {
      $array = array_keys($choices);
      $current = $array[0];
    }

    $apply_label = t('Apply');
    $headline_label = t('Workflow State');
    
    $form['page-workflow-' . $nid]['header'] = array(
      '#type' => 'markup',
      '#markup' => "<h3>$headline_label</h3>",
    );

    $group_type = $workflow->wid == 2 ? 'btn-group-horizontal' : 'btn-group-vertical';
    
    $form['page-workflow-' . $nid] = array(
	'#type' => 'fieldset',
	'#prefix' => '<div class="page-set ' . $group_type . '" data-page-target="' . $nid . '" data-current="' . $page->workflow . '">',
	'#
	suffix' => '</div>',
    );
    

    $form['page-workflow-' . $nid]['radios'] = array(
      '#type' => 'radios',
      '#options' => $choices,
      '#name' => 'options-' . $nid,
      '#parents' => array('workflow'),
      '#default_value' => $current,
      //'#prefix' => '<div class="btn-group lalala">',
      //'#suffix' => '</div>',
      '#attributes' => array(
	'class' => array('btn-group-justified', 'btn-group-lg'),
      ),
    );    

    $form['page-workflow-' . $nid]['comment'] = array(
      '#type' => 'textarea',
      '#description' => t('Optionally, include a comment describing this change.'),
      '#prefix' => '<div id="workflow-comment-' . $nid . '" class="workflow-comment panel panel-info"><div class="panel-heading"><h4 class="panel-title">'.t('Confirm State Change').'</h4></div><div class="panel-body">',
      '#suffix' => '</div></div>',
      '#cols' => 20,
      '#resizable' => FALSE,
    );
	
    $form['page-workflow-' . $nid]['page'] = array(
      '#type' => 'hidden',
      '#name' => 'page',
      '#value' => $nid,
      '#attributes' => array('id'=>'page-id-' . $nid, 'class' => array('page-id-field')),
    );
    $form['page-workflow-' . $nid]['buttons']['submit'] = array(
      '#type' => 'submit',
      '#value' => $apply_label,
      '#id' => 'workflow-submit-' . $nid,
      '#attributes' => array (
	'class' => array('btn btn-success'),
      ),
      '#ajax' => array(
        'callback' => 'doc_manage_workflow_callback',
	'file' => drupal_get_path('module','doc_manage') . '/doc_manage.workflow.inc', //** If this gets moved into an .inc, it needs to be included here
        ),
      );
    $form['feedback'] = array(
	'#type' => 'markup',
	'#markup' => '',
    );
  }
  return $form;
}

/*
 * Callback for workflow shift form
 */
function doc_manage_workflow_callback($form, &$form_state){

  $node = node_load($form_state['storage']['node']);
  $choices = workflow_field_choices($node);
  $old_state = $node->workflow;
  $old_label = $choices[$old_state];
  $target_state = $form_state['values']['workflow'];
  $target_label = $choices[$target_state];
  $node_title = $node->title;
  $element = $form['workflow']['footer'];
  $comment = $form_state['values']['comment'];
  
  if($target_state != $old_state){
    $new_state = workflow_execute_transition($node, $target_state, $comment, TRUE);
    entity_get_controller('node')->resetCache(array($node->nid));

    $form['feedback']['#markup'] = "<div class='alert alert-success alert-edit'><em>" . $node_title . "</em>: workflow state has been changed from <em>$old_label</em> to <em>$target_label</em>.</div>";
    
  }
    $commands = array();
    $commands[] = ajax_command_replace('#shift-workflow', drupal_render($form));

  return array('#type' => 'ajax', '#commands' => $commands);
}

 /*
  * Find the available workflow states and current state of a given node ID.
  * Return a list array.
  */
 function get_node_workflow($node){
  $workflow = workflow_get_workflow_type_map_by_type($node->type);
  $choices = workflow_field_choices($node);
  $workflow = workflow_get_workflows_by_wid($workflow->wid);
  $states = workflow_get_workflow_states_by_wid($workflow->wid);
  $current = workflow_node_current_state($node);
  
  $in_review = 13;
  $submitted = 16;
  $approved = 14;
  $published = 36;

  if($current == $published) {
    $elements[] = array(
      'data' => $choices[$current],
      'class' => array('state-published', 'btn',  'btn-justify', 'btn-sm', 'btn-success'),
    );
  }
  else{
      $class = array();
      $class[] = "states";
      switch ($current){
        case $in_review:
          $class[] = "alert alert-info";
          break;
        case $submitted:
          $class[] = "alert alert-warning";
          break;
        case $approved:
          $class[] = "alert alert-success";
          break;
      }

      if($sid != $published){
        $elements[] = array(
          'data' => $choices[$current],
          'class' => $class,
        );
      }
    }
  
  $list = array(
    'items' => $elements,
    'type' => 'ul',
    'attributes' => array(
      'class' => array('inline','node-states'),
    ),
  );
  return theme_item_list($list);
 }

 /**
 * Overrides theme_radios().
 *
 * Wraps radio groups in bootstrap <div>s
 */
function tn_bootstrap_radios($variables) {
  $element = $variables['element'];
  $attributes = array();
  if (isset($element['#id'])) {
    $attributes['id'] = $element['#id'];
  }
  $attributes['class'] = 'form-radios';
  if (!empty($element['#attributes']['class'])) {
    $attributes['class'] .= ' ' . implode(' ', $element['#attributes']['class']);
  }
  if (isset($element['#attributes']['title'])) {
    $attributes['title'] = $element['#attributes']['title'];
  }
  
    $attributes['data-toggle'] = 'buttons';
    
  return '<div' . drupal_attributes($attributes) . '>' . (!empty($element['#children']) ? $element['#children'] : '') . '</div>';
}

/**
 * Implements hook_preprocess_button().
 */
function tn_bootstrap_preprocess_button(&$vars) {
  if(!in_array('btn',$vars['element']['#attributes']['class'])) {
    $vars['element']['#attributes']['class'][] = 'btn';
  }

  if(!in_array('theme-override', $vars['element']['#attributes']['class'])) {
    if (isset($vars['element']['#value'])) {
      if ($class = _bootstrap_colorize_button($vars['element']['#value'])) {
	$vars['element']['#attributes']['class'][] = $class;
      }
    }
  }
}

/**
 * Given a ticket ID, return workflow state counts for component entities.
 *
 * This is used to determine which overall stage the user is in, and to trigger automated actions.
 *
 * @param $ticket
 * 	The ticket object to use.
 * @param $entity_type
 * 	The entity type to use. Currently supported: 'book' and 'topic'. Defaults to 'book'.
 * 	
 * @return (array)
 * 	Totals are returned in a keyed array by workflow state, in the format "[state id] => count".
 * 	
 */
function doc_manage_workflow_entity_counts($ticket, $entity_type = 'book'){
  
  module_load_include('inc','doc_manage', 'doc_manage.relation');
  
  $page_ids = doc_manage_get_book_pages($ticket);
  
  $pages = node_load_multiple($page_ids);
  $counts['total'] = count($page_ids);
  
  foreach($pages as $page) {
    $state = workflow_node_current_state($page);
    $counts[$state] = $counts[$state] + 1;
  }

  return $counts;  
}

/**
 * Return an array of node workflow states.
 *
 * @var $nodes
 * 	An array of NIDs.
 *
 * @var $type
 * 	(optional) filter the results by a particular bundle type (i.e., 'book', 'import_ticket')
 *
 * @return (array)
 * 	Associative array of state IDs, keyed by NID.
 */
function doc_manage_get_node_workflow_states($nodes, $type = NULL) {
  $pages = node_load_multiple($nodes);
  foreach ($pages as $page) {
    if ($type && $page->type != $type) continue;
    $out[$page->nid] = workflow_node_current_state($page);
  }
  unset ($pages);
  return $out;
}

/**
 * Return an array of unapproved book page NIDs.
 *
 * @var $nodes
 * 	An array of NIDs.
 *
 * @return (array)
 * 	associative array of state IDs that are not approved or published, keyed by NID.
 */
function doc_manage_get_unapproved_pages($nodes) {
  return array_filter(doc_manage_get_node_workflow_states($nodes, 'book'), function($item) {return ($item != WORKFLOW_PAGE_APPROVED_STATE && $item != WORKFLOW_PAGE_PUBLISHED_STATE);});  
}

/**
 * Return the NID of the next unapproved page in a book.
 *
 * @var $node
 * 	The current book page node.
 *
 * @return (int)
 * 	The NID of the next unapproved book page.
 */
function doc_manage_get_next_unapproved_page($node) {
  $unapproved = doc_manage_get_unapproved_pages(doc_manage_get_book_pages_from_book_page($node));
  $unapproved_pages_after_current = array_slice($unapproved, array_search($node->nid, array_keys($unapproved)) + 1, NULL, TRUE);
  $next = current(array_keys($unapproved_pages_after_current));
  return $next != $node->nid ? $next : FALSE;
}
