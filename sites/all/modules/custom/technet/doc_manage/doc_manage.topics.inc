<?php

 /**
  * Topic manager
  * Form function which generates a table of all topic nodes for a given controller.
  * 
  * @param (array) $form
  * @param (array) &$form_state
  * @param (object) $controller: found in the URL argument for the page.
  * 
  */
 function doc_manage_display_related_topics($form, &$form_state){
    module_load_include('inc', 'doc_manage', 'doc_manage.relation');

  if (!arg(2) || !is_numeric(arg(2))){
    return FALSE;
  }
  $nid = arg(2);
 
  if (!$nid) {
    return '<h2>No controller object specified.</h2><p>You must append a node ID for a controller node to the path.</p>';
  }
  
  $node = node_load($nid);
   if (!$node || $node->type != 'import_ticket') {
    return '<h2>No valid object specified.</h2><p>The node specified must exist and be an "online document" type.</p>';
  }
      
  //set up presentation table  
  $table = array(
    'header' => array(
      array('data' => 'NID'),
      array('data' => 'Topic Title'),
      array('data' => 'Re-run conversion'),
    ),
  );

  $result = _doc_manage_get_topics($node);
  //loop through endpoints and add table rows for each
  foreach($result as $topic){
          
    $rows[] = array(
      'data' => array(
        array('data' => l($topic->nid,'node/'.$topic->nid.'/edit'), 'class' => array('topic-nid')),
        array('data' => $topic->title, 'class' => array('topic-title')),
        array('data' => l('Re-convert this topic','admin/technet/manage/conversion/convert-single/' . $topic->nid), 'class' => array('topic-convert')),
      ),
    );
  }
  
  //add rows to $table array
  $table['rows'] = $rows;
  $table['empty'] = '';
  $table['sticky'] = TRUE;
  
  //run everything through theme layer
  drupal_set_title('Manage topics');
  $output = array();
  $output = array(
    'heading' => array(
      '#type' => 'markup',
      '#markup' => '<h2>' . count($result) . ' topics are used in <em>' . $node->title . '</em></h2>',
    ),
    'table' => array(
      '#type' => 'markup',
      '#markup' => theme_table($table),
    ),
    'attributes' => array(
	'class' => array('hello','there'),
    ),
  );
  return $output;

}

/**
 * Display a block of administration commands for current controller node
 */
function doc_manage_administer_related_topics(){
    
  $node = menu_get_object();
  if(!$node) $node = node_load(arg(2));
  if($node->type != 'import_ticket') return FALSE;
    
  return drupal_get_form('doc_manage_administer_related_topics_form');
}

/**
 * Generate topic administration form.
 */
function doc_manage_administer_related_topics_form($form, &$form_state){
  
  $orphans_all = _doc_manage_get_orphaned_topics();
  $orphans = $orphans_all['orphans'];
  $abandoned = $orphans_all['abandoned'];

  //if there is a parent node to return to, insert a return link
  $arg = arg(2);
  if (is_numeric($arg)) {
    $result = db_query('SELECT nid, type FROM {node} WHERE nid IN (:nid)', array(':nid' => $arg));
    $match = $result->fetchAssoc();
    if($match['type'] == 'import_ticket'){
      $home = $match['nid'];
    }
  }
  
  if($home){
    $form['return'] = array(
      '#type' => 'markup',
      '#markup' => l('Return to Online Document','node/' . $home, array('attributes' => array('class' => array('return-link')))),
    );
  }
  $form['global'] = array(
    '#type' => 'fieldset',
  );
  $form['global']['header'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>Global topic status</h3>',
  );
  
  if(count($orphans)>0){
    $i = 0;
    foreach($orphans as $orphan){
      $i = $i + count($orphan->endpoints[LANGUAGE_NONE]);
    }
    $form['global']['orphans']['markup'] = array(
      '#type' => 'markup',
      '#markup' => '<p>There are <b>' . count($orphans) . '</b> orphaned relations in the database, containing <b>' . $i . '</b> topics. <span class="note">Orphaned relations have no controlling Online Document.</span></p>',
    );
  }
  if(count($abandoned)>0){
    $form['global']['abandoned']['markup'] = array(
      '#type' => 'markup',
      '#markup' => '<p>There are <b>' . count($abandoned) . '</b> abandoned relations in the database. Abandoned relations have no endpoints at all.</p>',
    );
  }
  
  if(count($orphans) > 0 || count($abandoned) > 0){
    $form['global']['action'] = array(
      '#type' => 'submit',
      '#value' => 'Clean up',
      '#submit' => array('doc_manage_clean_up_orphans'),
    );
  }
  return $form;
}

/**
 * Find and remove abandoned/orphaned topics in database
 */
function doc_manage_clean_up_orphans(){
  $orphans_all = _doc_manage_get_orphaned_topics();
  
  $abandoned = $orphans_all['abandoned'];
  $orphans = $orphans_all['orphans'];
  
  if (count($abandoned) != 0){
    foreach ($abandoned as $trash){
      relation_delete($trash->rid);
    }
  }
  //set up batch to delete orphaned topics
  $delete = array();
  foreach($orphans as $orphan){
    $delete = array_merge($delete,$orphan->endpoints[LANGUAGE_NONE]);
  }
  
  $operations[] = array('_doc_manage_batch_delete_orphans', array($delete));
    $batch = array(
    'operations' => $operations,
    'file' => drupal_get_path('module', 'doc_manage') . '/doc_manage.topics.inc',
    'finished' => '_doc_manage_batch_delete_orphans_finished',
    'title' => t('Deleting orphaned topics...'),
    'init_message' => t('Starting processes...'),
    'progress_message' => t('Completed @current task out of @total.'),
    'error_message' => t('Error while deleting orphaned topics.'),
    );

    batch_set($batch);
    batch_process();
}

function _doc_manage_batch_delete_nodes($relation, &$context){
  // information needed to track progression between successive calls.
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_topic'] = 0;

    // Save topic count for the termination message.
    $context['sandbox']['max'] = count($relation);
  }

  $result = array_slice($relation, $context['sandbox']['current_topic'], 1); //last parameter is $limit. Caps number of jobs that can be run at once.
  foreach ($result as $section) {
    node_delete($section['entity_id']);
    // Store some results for post-processing in the 'finished' callback.
    $context['results']['endpoints'][] = $section['entity_id'];  
  }

  // Update our progress information.
  $context['sandbox']['progress']++;
  $context['sandbox']['current_topic']++;
  $context['message'] = 'Deleting item ' . $context['sandbox']['current_topic'] . ' of ' . $context['sandbox']['max'] . ': ' . $relation[$context['sandbox']['current_topic']]['entity_id'];

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }

}

function _doc_manage_batch_delete_orphans_finished($success, $results, $operations){
  if($success){
    drupal_set_message('Orphaned topics deleted.');
  }
  else{
    drupal_set_message('Error while deleting topics.','error');
  }
}


/**
 * Generate form for single-topic conversion
 */

 function doc_manage_convert_once($form, &$form_state) {

  $arg = array_pop(arg());
  if(is_numeric($arg)) {$node = $arg;}
  
  $header = array(
    '#type' => 'markup',
    '#markup' => '<h1>Process one topic</h1>
    <p><b>Note:</b> <i>This process will erase any manual edits. The topic content will be replaced with a clean conversion of the raw HTML.</i></p>',
  );
  
  $textfield = array(
    '#type' => 'textfield',
    '#title' => 'Enter the Node ID of the topic to process',
    '#size' => 10,
    '#default_value' => $node,
    '#required' => TRUE,
  );
  
  $submit = array(
    '#type' => 'submit',
    '#value' => 'Process',
  );
  
  $form['header'] = $header;
  $form['textfield'] = $textfield;
  $form['submit'] = $submit;
  
  return $form;
 }
 
function doc_manage_convert_once_submit($form, &$form_state) {
    
    module_load_include('inc', 'doc_import', 'doc_import.convert');
  $topic = node_load($form_state['input']['textfield']);
  if(!$topic || $topic->type != "topic") {
    return;
  }
  else{
    $controller = relation_get_related_entity('node',$topic->nid);
    $tool = $controller->field_authoring_tool[LANGUAGE_NONE][0]['value'];
    
    if($topic && $controller && $tool) {
      doc_import_convert_topic($topic, $tool, $controller);
      drupal_set_message('"' . $topic->title . '" has been converted.','status');
      if(module_exists('devel')) {
	dpm((array)$topic->field_topic_source_content[LANGUAGE_NONE][0]['value'],'topic raw');
        dpm((array)$topic->field_topic_processed_html[LANGUAGE_NONE][0]['value'],'topic postprocess');
      }
    }
    else {
      drupal_set_message('Topic is missing required information. Conversion cannot be run.','error');
    }
  }
  return '<h1>submitted</h1>';
}

/**
 * Utility function that returns a keyed array of topic nodes given a controller node object.
 * Return: array (nid => title)
 */
function _doc_manage_get_topic_nids($node, $type = NULL){
    module_load_include('inc', 'doc_manage', 'doc_manage.relation');
    
  if(gettype($node) == 'array' && gettype($node[0]) == 'object') {
    $node = reset($node);
  }
  
  // If no type is supplied, assume the "uses topic" relation
  if (!$type) {
    switch ($node->type){
      case 'book':
        $type = 'topic_used_on_page';
        break;
      case 'import_ticket':
        $type = 'uses_topic';
        break;
    }
  }

  //get endpoints and split off controller object ([0]).
  $endpoints = doc_manage_relation_query($type, $node);

  $parent = array_shift($endpoints);
  foreach($endpoints as $ep){
    $nids[] = $ep['entity_id'];
  }
    
  $query = db_query("SELECT nid FROM {node} WHERE nid IN (:nids)", array(':nids' => $nids));
  return $query->fetchCol();

}

/**
 * Given a controller node object and the name of a field, returns an array of field values keyed by nid.
 * @param: $controller = controller node object
 * @param: $field_name = machine name of the field to find
 * 
 */
function _doc_manage_get_topic_fields ($controller, $field_name) {
    
    $entity_type = 'node';
    $bundle = 'topic';
    $nids = _doc_manage_get_topic_nids($controller);

   if (!$nids) return FALSE;
   
   // Load all nodes and get the topic_anchors field values into an array.

   $field_values = db_select('field_revision_' . $field_name, 'f')
      ->fields('f', array('entity_id', $field_name . '_value'))
      ->condition('entity_type', $entity_type)
      ->condition('bundle', $bundle)
      ->condition('entity_id', $nids, 'IN')
      ->condition('deleted', 0)
      ->execute()
      ->fetchAllKeyed();

   foreach($field_values as &$anchor) {
      $anchor = json_decode($anchor, TRUE);
   }
   return $field_values;
}
