<?php
// $Id$
/**
 * @File: doc_import.process_book.inc
 * Functions for processing the imports with breadking headings
 */
  error_reporting(-1);
  mb_internal_encoding('UTF-8');
  
// load library functions
define('STORAGE_DIRECTORY', 'temporary://');


module_load_include('inc', 'doc_import', 'doc_import.process.lib');
module_load_include('inc', 'doc_import', 'htmLawed.lib');

//get the heirarchical select widget for use in form building
module_load_include('inc', 'hierarchical_select', 'includes/common') or die("Can't load Hierarchical Select");

function doc_import_node_form($type){
  
    $output = array();
    $types = node_type_get_types();
    
    if (isset($types[$type])){
      $node = (object) array(
        //'uid' => $user->uid,
        //'name' => (isset($user->name) ? $user->name : ''),
        'type' => $type,
        //'language' => LANGUAGE_NONE,
      );
      
    $output = drupal_get_form($type . '_node_form',$node);  
    }
    return $output;
}







