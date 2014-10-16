<?php

/**
 * File: doc_import.log.inc
 * Log file CRUD functions
 */

 
/**
 * Updates the saved log file for this document
 */
function doc_import_update_log($log, $node_id){
  $local_directory = "public://import-source-files/$node_id";
  $log_directory = $local_directory . '/log';
  if (!file_exists($log_directory)){
    mkdir($log_directory);
  }
  
  //if the file doesn't exist, insert a header row
  if(!file_exists($log_directory . '/import')){
    $log_string="timestamp|type|message|elapsed time".PHP_EOL;
  }
  
  $logfile = fopen($log_directory . '/import','ab');
  
  foreach($log as $line){
    $line_string = '';
    foreach($line as $entry){
      $line_string .= "$entry|";
    }
    $log_string .= $line_string . PHP_EOL;
  }
  fwrite($logfile,$log_string);
  fclose($logfile);
 }

/**
 * Prints a log summary to the message area.
 */
function doc_import_show_log($log, $nid = NULL){
  foreach($log as $entry){
    $line = $entry['timestamp'] . "\t[" . $entry['category'] . "]\t" . $entry['message'];
    $summary[] = $line;
    $summary_text .= $line . PHP_EOL;
  }
}

/**
 * Appends a new entry onto the $log array
 */
function add_log_entry($category = NULL, $message, &$log = array()){
  
  $new_entry_timestamp = date('Y-M-d h:m:s', time());
  if(count($log) > 1){
    $last_entry = end($log);
    $elapsed = $new_entry_timestamp - $last_entry['timestamp'];
  }
  $log[] = array('timestamp' => $new_entry_timestamp, 'category' => $category, 'message' => $message, 'elapsed' => $elapsed);
}

/**
 * Returns the active log file as an array
 */
function doc_import_read_log($node_id){
  $local_directory = "public://import-source-files/$node_id";
  $log_directory = $local_directory . '/log';
  if (!file_exists($log_directory)){
    return FALSE;
  }
  
  //read file into an array. csv_in_array is a contrib library function found on php.org
  $log = csv_in_array($log_directory.'/import','|',"\"",TRUE);
  return $log;
}