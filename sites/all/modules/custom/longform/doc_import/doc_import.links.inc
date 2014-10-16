<?php

/*
 * doc_import.links.inc
 * version 7.x-1.0
 *
 * Provides link processing and reconstruction services for doc_import.
 */

 
 

/**
* Process links and build link arrays
*/
function doc_import_prepare_links(&$section, $index, $node_id){
    //return;
    //
    ////dpm($section,'section array');
    $content = $section['content'];
    //$h_targets = $section['target']['head'];
    //if(!is_array($temp_sources)){
    //        $temp_sources = array();
    //}
    //
    //if(!is_array($temp_targets)){
    //        $temp_targets = array();
    //}
    //
    ////Add targets contained in headings
    //foreach($h_targets as $heading_target){
    //    $temp_targets[] = array(
    //        'section' => $index,
    //        'name' => $heading_target,
    //        'find' => 'head',
    //    );
    //}


    //Loop through all links in section content and assemble the link sources & targets
    $regex_href = '/<a.*?href="(.*?)".*?>(.*?)<\/a>/iu';
    preg_replace_callback(
        $regex_href,
        function($matches) use (&$outgoing, $index) {
                // Ignore external links: return unchanged and don't add it to the list
                if (strpos($matches[1], 'http://') !== FALSE || strpos($matches[1], 'mailto') !==FALSE) {
                    return $matches[0];
                }
            
            if(stripos($matches[1], '#') !== FALSE) {
                $anchor = array_pop(explode('#',$matches[1])); 

                //throw it on the $outgoing stack to deal with later
                $outgoing[] = array(
                    'section' => $index,
                    'href' => '#' . $anchor,
                    'target' => $anchor,
                    'text' => $matches[2],
                    );
                
                // Return the stripped href
                return str_replace($matches[1], '#' . $anchor, $matches[0]);
            }
        },
        $content
    );
    
    $regex_test = '/<a.*?href="(.*?.htm.*?)".*?>(.*?)<\/a>/iu';
    //preg_match_all($regex_href, $content, $matches);
    
    //dpm(count($matches[1]),'# hmts');
    
    $regex_name = '/<a.*?name="(.*?)".*?>(.*?)<\/a>/iu';
    preg_match_all($regex_name, $content, $matches);
    
    foreach($matches[1] as $match){
        $incoming[] = array(
            'section' => $index,
            'name' => $match,
            'find' => 'REPLACE-WITH-NID#' . $match,
            );
    }

    $links['sources'] = $outgoing;
    $links['targets'] = $incoming;
    doc_import_update_link_file($links, $node_id);
    
    $section['content'] = $content;
    //extract_body($qp->top()->html());
}

/*
 * Given a ticket node ID, runs through related nodes and reconstructs links and targets
 */

function doc_import_reconstruct_links($ticket){
    return;
  //if $ticket_node is not a node object, assume it is a nid and load the corresponding node
  if(!is_object($ticket)){
   $tempnode = node_load($ticket);
   $ticket = $tempnode;
  }
  
  //Get related nodes
  
    $nodes = doc_manage_get_book_pages($ticket);
    $parent = array_shift($nodes);    
    $lf = doc_import_read_link_file($parent);
    //dpm($nodes,'nodes');
    $sources = $lf['sources'];
    $targets = $lf['targets'];
    //dpm($sources,'sources');
    //dpm($targets,'targets');
    foreach ($targets as &$target){
	    $target['node'] = $nodes[$target['section']];
    }
    foreach ($sources as &$source){
      $key = recursive_array_search($source['target'], $targets); //recursive_array_search in _book.inc, from php.net
      if($key !== FALSE){
        $source['node'] = $nodes[$source['section']];
        $source['target_section'] = $targets[$key]['section'];
        $source['target_node'] = $targets[$key]['node'];
               
        if($targets[$key]['find'] == 'head'){
          $source['type'] = 'head';
        }
        else{
          if($source['target_section'] == $source['section']){
            $source['type'] = 'same';
          }
          else{
            $source['type'] = 'content';
          }
        }
      }
    }
    
    //eliminate extras that were not anywhere
    for($i = count($sources); $i >= 0; $i--){
        if(!isset($sources[$i]['node'])){
            unset($sources[$i]);
        }
    }
    //cycle through book nodes

    foreach($sources as $source) {
        $types[$source['type']][] = $source;
    }
    		
    $last_node = 0;
    foreach ($sources as &$source){
      $new_node = $source['node'];
      if ($new_node != $last_node){
        if($last_node != 0){
          node_save($node);
        }
        $node = node_load($new_node);
        $last_node = $new_node;
      }
      $content = $node->body[LANGUAGE_NONE][0]['value'];
      $find = $source['href'];
      
      //source and target are in the same section
      if($source['section'] == $source['target_section']){
        $content = str_replace($find,$source['href'],$content);
        $node->body[LANGUAGE_NONE][0]['value'] = $content;
        continue;
      }
      
      if($source['type'] == 'head'){
        //dpm('link target is in the heading of another section (source["type"] == "head")');
        $content = str_replace($find, "/node/" . $source['target_node'], $content);
        $node->body[LANGUAGE_NONE][0]['value'] = $content;
        continue;
      }
      
      //target is in the body of another section
      if($source['type'] == 'content'){
        $content = str_replace($find, "/node/" . $source['target_node'] . $source['href'], $content);
        $node->body[LANGUAGE_NONE][0]['value'] = $content;
      }
    }
    //if there are no sources, the node will not have been loaded and there will be nothing to save
    if(count($sources) != 0){
      node_save($node);
    }
  drupal_set_message('Document postprocessing completed successfully.','status');
}

/*
 * Creates or updates link array stored on disk
 */
function doc_import_update_link_file($meta, $node_id){
    return;

    $local_directory = "public://import-source-files/$node_id";
    $link_directory = $local_directory . '/log';
    if (!file_exists($link_directory)){
            mkdir($link_directory);
    }
    $sources = array();
    $targets = array();
    
    $existing = doc_import_read_link_file($node_id);
    if ($existing['sources']){
            $sources = $existing['sources'];
    }
    else{
            $existing['sources'] = array();
    }
    if($existing['targets']){
            $targets = $existing['targets'];
    }
    else{
            $existing['targets'] = array();
    }
    
    $sources = array_merge($sources,$meta['sources']);
    $targets = array_merge($targets,$meta['targets']);
    
    $meta['sources'] = $sources;
    $meta['targets'] = $targets;
    
    file_unmanaged_delete($link_directory.'/links');
    
    //write links
    $new_file_uri = $link_directory . '/links';
    $converted_content = json_encode($meta);
    //save JSON object
    $new_file = file_save_data($converted_content, $new_file_uri, FILE_EXISTS_REPLACE);
    if($new_file){
    }

}
 
/*
 * Reads the link array stored on disk
 */
function doc_import_read_link_file($node_id){
    return;
  $local_directory = "public://import-source-files/$node_id";
  $log_directory = $local_directory . '/log';
	if (!file_exists($log_directory)){
    return FALSE;
  }
  $meta = json_decode(file_get_contents($log_directory.'/links'),TRUE);
  return $meta;
}


