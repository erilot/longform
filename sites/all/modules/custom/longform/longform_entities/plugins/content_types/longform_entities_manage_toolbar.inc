<?php

$plugin = array(
  'single' => TRUE,  // Just do this one, it is needed.
  'title' => t('Manager toolbar for Longform entities'), 
  'description' => t('Provide a context-aware toolbar that lets admins manage their content'), 
  'category' => t('Longform'), 
  'edit form' => 'longform_entities_entity_manager_toolbar_edit_form',
  'render callback' => 'longform_entities_entity_manager_toolbar_render',
//  'admin info' => 'longform_entities_entity_manager_toolbar_admin_info', // A function that will return the information displayed on the admin screen (optional).
  'defaults' => array( // Array of defaults for the settings form.
    'button_label' => t('replace_this'), // Replace this with something actually useful, if there is something
  ),
  'all contexts' => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function longform_entities_entity_manager_toolbar_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets saved
 * Notice, the magic that automatically does that for you.
 */
function longform_entities_entity_manager_toolbar_edit_form_submit(&$form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

/**
 * Run-time rendering of the body of the block (content type)
 */
function longform_entities_entity_manager_toolbar_render($subtype, $conf, $args, $contexts) {

  global $user;
  $user_is_admin = user_access('administer tickets', $user);
  $context = reset($contexts);
  $entity = $context->data;
//  dpm($entity,'entity in toolbar_render');
  $type = get_class($entity);

  $this_state = longform_get_current_workflow_state($entity);
  dpm($this_state->state,'current state');
  module_load_include('inc', 'longform_entities_toolbar');
  module_load_include('inc', 'longform_entities_toolbar', 'longform_entities_toolbar.relation');

  // @TODO: Make this access check dependent on group membership, not just a plain logged-in check
  if (!user_is_logged_in()) {
    return;
  }

  switch ($type) {
    case 'LongformTicket':
      $wrap = entity_metadata_wrapper('longform_ticket', $entity);
      break;
    case 'LongformSet':
      $wrap = entity_metadata_wrapper('longform_set', $entity);
      break;
    default:
      // Attempt to load a generic entity with a 'type' property.
      $wrap = entity_metadata_wrapper($entity->type, $entity);
  }

  $render = array(
    '#type' => 'container',
    '#prefix' => '<nav class="navbar navbar-default" role="navigation" id="doc-manage-toolbar">',
    '#suffix' => '</nav>',
    '#attributes' => array(
      'class' => 'container-fluid',
    ),
  );

  $render['opts'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('collapse', 'navbar-collapse'),
      'id' => 'doc-manage-toolbar',
    ),
  );


  $items[] = _longform_entities_toolbar_button_jump($entity); // "jump to" button
//dpm('--jump to done');
  $items[] = _longform_entities_toolbar_button_edit($entity);  // "edit" button
//  $items[] = _longform_entities_toolbar_button_flag_html($entity); // "html flag" button
//dpm('--flag done');
  $items[] = _longform_entities_toolbar_button_approve($entity);  // "approve" form
//dpm('--approve done');

  $render['opts']['list'] = array(
    '#theme' => 'item_list',
    '#items' => $items,
    '#attributes' => array(
      'class' => array('nav', 'navbar-nav'),
    ),
  );

  $render['opts']['status'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('btn-toolbar', 'navbar-btn', 'navbar-right'),
    ),
  );

  $render['opts']['status']['pdf_status'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('btn-group'),
    ),
  );

  $render['opts']['status']['pdf_status']['content']['label'] = array(
    '#type' => 'button',
    '#value' => t('PDF available'),
    '#attributes' => array(
      'class' => array('btn', 'btn-default', 'disabled'),
    ),
  );

  // PDF Status
  $pdf_ok = (isset($wrap->field_manual_pdf) && count($wrap->field_manual_pdf) > 0) || (isset($wrap->field_auto_pdf) && count($wrap->field_auto_pdf) > 0);

  $render['opts']['status']['pdf_status']['content']['value'] = array(
    '#type' => 'button',
    '#value' => $pdf_ok ? t('Yes') : t('No'),
    '#attributes' => array(
      'class' => array(
        'btn',
        'disabled',
        $pdf_ok ? 'btn-success' : 'btn-danger'
      ),
    ),
  );

  // Publish status

  // Get status of page
  //$wf_state = workflow_entity_current_state($ticket);
  $publish_mode = $wf_state == WORKFLOW_PAGE_PUBLISHED_STATE || $wf_state == WORKFLOW_TICKET_PUBLISHED_STATE ? 'btn-success' : 'btn-danger';
  $publish_label = $wf_state == WORKFLOW_PAGE_PUBLISHED_STATE || $wf_state == WORKFLOW_TICKET_PUBLISHED_STATE ? 'Published' : 'Unpublished';

  $render['opts']['status']['publish_status'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('btn-group'),
    ),
  );

  $render['opts']['status']['publish_status']['content']['label'] = array(
    '#type' => 'button',
    '#value' => t('Publication status'),
    '#attributes' => array(
      'class' => array('btn', 'btn-default', 'disabled'),
    ),
  );

  $render['opts']['status']['publish_status']['content']['value'] = array(
    '#type' => 'button',
    '#value' => $publish_label,
    '#attributes' => array(
      'class' => array('btn', 'disabled', $publish_mode),
    ),
  );


  $block = new stdClass();
  $block->title = '';
  $block->content = $render;
  return $block;
}


  function _longform_entities_toolbar_button_jump($entity) {

    $type = $entity->type;
     // Go to Book/Return to ticket icon
    switch ($type) {

      case 'longform_ticket':
        $state = longform_get_current_workflow_state($entity);
        $set = longform_relation_get_other_endpoint_set_ticket($entity);
        if (count($set) == 1) {
          $set = reset($set);

          $up = array(
            'data' => l('<span class="glyphicon glyphicon-arrow-up"></span> ' . t('Up to set'),
              'set/' . $set->sid,
              array(
                'html' => TRUE,
                'attributes' => array(),
              )
            ),
          );

        }
        $button = $up;
        break;

      case 'longform_set' :

      case 'import_ticket':

        // For import tickets, this icon links to the cover page of the document. It's only available after postprocessing.
        $endpoints = longform_entities_relation_query('ticket_contains_book_pages', $entity);
        if (!empty($endpoints)) {
          array_shift($endpoints);
          $cover = reset($endpoints);
          $stages = _longform_entities_toolbar_get_all_accordion_states($entity);
          $tooltip = array(
            'data-toggle' => 'tooltip',
            'data-trigger' => 'hover',
            'data-placement' => 'top',
            'title' => !isset($stages['Import']) ? t('Go to the document') : t('Document pages haven\'t been created yet'),
          );

          $button = array(
              'data' => l('<span class="glyphicon glyphicon-arrow-right"></span>Cover page', (isset($cover['entity_id']) ? 'entity/' . $cover['entity_id'] : ''), array(
                'html' => TRUE,
                'attributes' => array(
                  'class' => array($stages['Import'] ? 'disabled' : 'enabled'),
                )
              )),
            ) + $tooltip;
        }
        else {
          return FALSE;
        }
        break;

      case 'book':

        // For books, this icon sends users back to the import ticket.
        $ticket = longform_entities_toolbar_relation_get_ticket($entity);
        $button = l(
          '<span class="glyphicon glyphicon-arrow-left"></span>' . t('Ticket'), 'entity/' . $ticket, array(
          'html' => TRUE,
          'attributes' => array(
            'class' => array(),
            'data-toggle' => 'tooltip',
            'data-trigger' => 'hover',
            'data-placement' => 'top',
            'title' => t('Return to ticket'),
          )
        ));
        break;
    }

    return $button;
  }

  function _longform_entities_toolbar_button_edit($entity) {
    try {
      $entity = entity_metadata_wrapper($entity->type, $entity);
    }
    catch (Exception $e) {
      drupal_set_message('<b>Error while generating edit button: ' . $e->getMessage(),'error');
      return FALSE;
    }
    switch ($entity->type->value()) {
      case 'longform_ticket':
        $edit_url = 'admin/longform/ticket/manage/' . $entity->tid->value();
        $label = 'ticket';
        break;

      case 'longform_set':
        $edit_url = 'admin/longform/set/manage/' . $entity->sid->value();
        $label = 'set';
        break;

      default:
        drupal_set_message('Unexpected entity type in _longform_entities_toolbar_button_edit().','error');
    }

    $button = l('<span class="glyphicon glyphicon-edit"></span> ' . t('Edit this @label', array('@label' => $label)), $edit_url, array(
      'html' => TRUE,
      'attributes' => array(
        'class' => array(),
        'data-toggle' => 'tooltip',
        'data-trigger' => 'hover',
        'data-placement' => 'top',
        'title' => t('Edit this ' . $label),
      )
    ));

    return $button;
  }

  function _longform_entities_toolbar_button_flag_html($entity) {
    switch ($entity->type) {

      case 'book':

        // HTML Help Flag
        $help_flag = flag_get_flag('html_problem_alert');
        $help_status_class = $help_flag->is_flagged($entity->nid) ? ' text-danger' : '';

        $flag = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('dropdown-menu', 'prevent-close'),
          ),
        );
        $flag['flag']['alert'] = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array(),
          ),
        );
        $flag['flag']['alert']['message'] = array(
          '#markup' => 'If there is an HTML problem on this page, click this button to set an alert.<br/><br/>',
        );
        $flag['flag']['help'] = array(
          '#markup' => flag_create_link('html_problem_alert', $entity->nid),
        );

        $button = array(
          'data' => '<a href="#" class="dropdown-toggle' . $help_status_class . '" data-toggle="dropdown"><span class="glyphicon glyphicon-flag"></span>Flag HTML</a>' . render($flag),
          'class' => array('flag-set', 'keep-alive'),
          'data-toggle' => 'tooltip',
          'data-placement' => 'top',
          'data-trigger' => 'hover',
          'title' => t('Flag an HTML problem on this page'),
        );
        break;
    }

    return isset($button) ? $button : FALSE;

  }

  function _longform_entities_toolbar_button_approve($entity) {
    // Approval button

    $approved = FALSE;
    switch ($entity->type) {
      case 'book':
        $wf_choices = workflow_field_choices($entity);
        $wf_state = workflow_entity_current_state($entity);
        $wf_label = $wf_choices[$wf_state];
        $states = _longform_entities_toolbar_get_import_state($entity);

        $published_state = $entity->type == 'import_ticket' ? WORKFLOW_TICKET_PUBLISHED_STATE : WORKFLOW_PAGE_PUBLISHED_STATE;

        // Get highest state the current user can move to
        $state = workflow_get_workflow_entity_by_nid($entity->nid);
        if (array_key_exists(36, $wf_choices)) {
          array_pop($wf_choices);
        }

        end($wf_choices);
        $highest_workflow_shift = key($wf_choices);

        $label = array(
          WORKFLOW_PAGE_APPROVED_STATE => t('Approve'),
          WORKFLOW_PAGE_SUBMITTED_STATE => t('Submit'),
          WORKFLOW_PAGE_PUBLISHED_STATE => t('Publish'),
        );

        $approve = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('dropdown-menu', 'prevent-close'),
          ),
        );
        $approve['approve']['alert'] = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array(),
          ),
        );
        if ($state->sid != WORKFLOW_PAGE_APPROVED_STATE && $state->sid != WORKFLOW_PAGE_PUBLISHED_STATE) {
          $approve['approve']['alert']['message'] = array(
            '#markup' => '<p>To ' . strtolower($label[$highest_workflow_shift]) . ' this content, click
	    <button class="btn btn-sm disabled"><span class="glyphicon glyphicon-ok"></span></button>.
	    To approve and jump to the next unapproved page, click
	    <button class="btn btn-sm disabled"><span class="glyphicon glyphicon-ok"></span>& <span class="glyphicon glyphicon-arrow-right"></span></button>.
	    </p><br/>', //' . $label[$highest_workflow_shift] . '
          );
          $approve['approve']['form'] = drupal_get_form('longform_entities_toolbar_approve_content_form', $entity, $label[$highest_workflow_shift], $highest_workflow_shift);
        }
        else {
          $approved = TRUE;
          $approve['approve']['alert']['message'] = array(
            '#markup' => 'This content has been ' . strtolower($wf_choices[$highest_workflow_shift]) . ', and the workflow state can no longer be changed.',
          );
        }
        $button = array(
          'data' => '<a href="#" class="dropdown-toggle' . ($approved ? ' text-success' : '') . '" data-toggle="dropdown"><span class="glyphicon glyphicon-ok"></span>' . ($approved ? t('Approved') : t('Approve')) . '</a>' . render($approve),
          'class' => array('flag-set', 'keep-alive'),
          'data-toggle' => 'tooltip',
          'data-placement' => 'top',
          'data-trigger' => 'hover',
          'title' => $approved ? t('This content has already been approved') : t('Approve this content for publication'),
        );
        break;
    }

    return isset($button) ? $button : FALSE;
  }

  function longform_entities_toolbar_approve_content_form($form, &$form_state, $entity, $label, $target) {
    $next = longform_entities_toolbar_get_next_unapproved_page($entity);
    $form = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(''),
      ),
    );

    $form['comment'] = array(
      '#type' => 'textarea',
      '#title' => t('Enter a comment (optional)'),
    );
    $form['nid'] = array(
      '#type' => 'hidden',
      '#name' => 'nid',
      '#value' => $entity->nid,
    );
    $form['target'] = array(
      '#type' => 'hidden',
      '#name' => 'target',
      '#value' => $target,
    );
    $form['next'] = array(
      '#type' => 'hidden',
      '#name' => 'target',
      '#value' => $next,
    );
    $form['approve_container'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('btn-group', 'pull-right'),
      ),
    );
    $form['approve_container']['approve'] = array(
      '#type' => 'submit',
      '#value' => '<span class="glyphicon glyphicon-ok"></span>'/*<br/>Approve'*/,
      '#attributes' => array(
        'class' => array('btn-success'),
        'data-toggle' => 'tooltip',
        'data-placement' => 'bottom auto',
        'data-trigger' => 'hover',
        'title' => 'Approve and stay on this page',
      ),
      '#submit' => array('longform_entities_toolbar_approve_content_submit'),
    );
    if ($next) {
      $form['approve_container']['approve_next'] = array(
        '#type' => 'submit',
        '#value' => '<span class="glyphicon glyphicon-ok"></span> & <span class="glyphicon glyphicon-arrow-right"></span>'
        /*<br/>Approve & next'*/,
        '#attributes' => array(
          'class' => array('btn-success'),
          'data-toggle' => 'tooltip',
          'data-placement' => 'bottom auto',
          'data-trigger' => 'hover',
          'title' => 'Approve and jump to the next unapproved page',
        ),
        '#submit' => array('longform_entities_toolbar_approve_content_submit_next'),
      );
    }
    else {
      $form['approve_container']['approve_ticket'] = array(
        '#type' => 'submit',
        '#value' => '<span class="glyphicon glyphicon-ok"></span> & <span class="glyphicon glyphicon-arrow-left"></span>'
        /*<br/>Approve & return'*/,
        '#attributes' => array(
          'class' => array('btn-success'),
          'data-toggle' => 'tooltip',
          'data-placement' => 'bottom auto',
          'data-trigger' => 'hover',
          'title' => 'Approve and jump back to the import ticket (this is the last unapproved page)',
        ),
        '#submit' => array('longform_entities_toolbar_approve_content_submit_return'),
      );

    }

    return $form;
  }

  function longform_entities_toolbar_approve_content_submit_return($form, &$form_state, $opts) {
    $opts['return'] = TRUE;
    longform_entities_toolbar_approve_content_submit($form, $form_state, $opts);
  }

  function longform_entities_toolbar_approve_content_submit_next($form, &$form_state, $opts) {
    $opts['next'] = TRUE;
    longform_entities_toolbar_approve_content_submit($form, $form_state, $opts);
  }

  function longform_entities_toolbar_approve_content_submit($form, &$form_state, $opts) {

    $target = $form_state['values']['target'];
    $label = $form_state['values']['label'];
    $comment = $form_state['values']['comment'];
    $entity = entity_load($form_state['values']['nid']);
    $next = $form_state['values']['next'];

    workflow_transition($entity, WORKFLOW_PAGE_APPROVED_STATE, TRUE);

    drupal_set_message('Workflow state updated.');

    // Approving a page approves all included topics.

    longform_entities_toolbar_approve_topics($entity);

    if (!$opts) {
      return $form;
    }
    else {
      if ($opts['next']) {
        drupal_goto('entity/' . $next);
      }
      else {
        if ($opts['return']) {
          drupal_goto('entity/' . longform_entities_toolbar_get_ticket_from_page($entity));
        }
      }
    }

    //development
    $commands[] = ajax_command_replace('#doc-manage-toolbar', longform_entities_toolbar_render_manage_toolbar($entity));

    return array('#type' => 'ajax', '#commands' => $commands);
  }

