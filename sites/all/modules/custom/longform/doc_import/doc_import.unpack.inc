<?php
/**
 * File: doc_import.unpack.inc
 * All unarchiving/unpacking functions are in this file.
 */

 
/**
 * Extract uploaded archive and perform initial file management / grooming.
 * Creates new "raw_html_[nid].html" file and attaches it to the ticket node
 */
function doc_import_extract_archive($ticket) {
				module_load_include('inc', 'doc_import', 'doc_import.log');
    module_load_include('inc', 'doc_import', 'htmLawed.lib');
    $temp_directory = "temporary://".$ticket->nid."/";
    $final_directory = "public://longform/imports/expanded/".$ticket->nid."/";

    $heading_split_level = taxonomy_term_load($ticket->field_granularity[LANGUAGE_NONE][0]['tid'])->name;
    $archive_uri = $ticket->field_document_archive[LANGUAGE_NONE][0]['uri'];
    $archive = $ticket->field_document_archive[LANGUAGE_NONE][0]['filename'];

    if (!$archive) {
    drupal_set_message('No archive has been uploaded to the server!','error');
    return FALSE;
    }

    // Extract the zip archive
  try {
    $archiver = archiver_get_archiver($archive_uri);  
  }
  catch (Exception $e) {
    drupal_set_message('There was a problem opening the archive. The error returned was: <br/><b>' . substr($e, 0, strpos($e, 'Stack trace')) . '</b>','error');
    return FALSE;
  }

    if (!$archiver) {
    drupal_set_message('The archive uploaded is not a valid zip archive, and could not be unpacked. Remove the uploaded file from this node, and upload the correct type (must be a .zip file)','error');
    return FALSE;
    }
  
    $files = $archiver->listContents();        
    
    // if the final directory already exists, clear it out and start over.
    if(file_exists($final_directory)){
    _rmdir_recursive($final_directory);
    }
  
    //extract files to temp directory
    $archiver->extract($final_directory);
  
    sort($files, SORT_STRING);
  
    // Clean up filenames (lowercase and remove spaces)
    _doc_import_groom_filenames($final_directory);

    // if a TOC file is present, load that and use it to assemble the full HTML file from component pieces
    $html_count = 0;
  
    $build_master = FALSE;

    foreach($files as &$zipped_file){
    $zipped_file = _doc_import_groom_string($zipped_file);
    if (stripos($zipped_file, '.htm') !== FALSE && stripos($zipped_file, '__macosx') !== FALSE) $html_count++;
    if (stripos($zipped_file, 'toc.htm') !== FALSE){
      $build_master == TRUE;
      watchdog('IMPORT','TOC file [' . $zipped_file . '] discovered, loading and assembling master document',$log);
      $toc_file = $zipped_file;
      //strip off the TOC's parent path for finding components
      $temp = explode('/',$zipped_file);
      array_pop($temp);
      $toc_root = $temp[0] . '/';
      break;
    } 
  
    }
    if ($html_count > 1 && !$toc_file){
        drupal_set_message('More than one HTML file is present in the archive, but there is no TOC.html file to use as an index. Check your Mif2Go settings and make sure <b>use TOC</b> is checked.','error');
        return FALSE;
    }
    if($toc_file){
 $toc = file_get_contents($final_directory . $toc_file);
 $author_tool = doc_import_get_author_tool($toc);
 //dpm($toc,'toc file');
 //remove <nobr> tags: these are invalid and crash QP
 $toc = str_ireplace(array('<nobr>','</nobr>'),'',$toc);
    
 //record author tool line -- at present, these will all be Mif2Go exports, but could be others in the future
 preg_match_all('/<!--\s(.*)\s-->/i', $toc, $matches);
 $tool = $matches[0][0];
    
 //get filenames from toc links. In mif2go-generated files, these will be links inside <p> tags. No other text will be on the page.
 $topics_list = qp($toc, 'p a');
 if ($topics_list->size() == 0){
     drupal_set_message('The uploaded archive contains a TOC file, but the TOC does not contain links to any other files. Technet cannot reconstruct the document without a working TOC file. Check Mif2Go settings and re-upload the archive.','error');
     return FALSE;
 }
    
 $topics = array();
 $files_to_load = array();
 $previous_level = NULL;
 
 // Log all filenames for future reference.
 $all_files = scandir($final_directory . $toc_root);
 array_shift($all_files);
 array_shift($all_files);
 
 // Create side arrays of file prefixes and suffixes for indexing
 foreach($all_files as $individual) {
    $regex = '/^([a-zA-Z]{2,3})(\d{5,7}).htm$/siu';
    preg_match($regex, $individual, $splitfile);
    $filenames['prefixes'][] = $splitfile[1];
    $filenames['suffixes'][] = $splitfile[2];
 }
 
 
 // Iterate over topics. 
 foreach($topics_list as $topic_item){
     
     // Find heading elements. These have classes of head1, head2, head3, etc.
     $class = $topic_item->attr('class');
    
     // If this is a heading element, get the heading level from the class name 
     if (stripos($class,'Head') !== FALSE) {    
  preg_match('/(\d)/', $class, $matches);
  $level = $matches[1];
     }
     
     // Classes containing "title" indicate a special Frame/Mif2Go topic title.
     // Temporarily assign these level 0.
     if(stripos($class,'title') !== FALSE) {
  $level = 0;
  $previous_level = 0;
     }
     
     //separate the filename from the link anchor. Create an array of topics to iterate and build from.
     $url_pieces = explode('#',$topic_item->attr('href'));
     $topic_title = $topic_item->text();
     $url_front = array_shift($url_pieces);
     $uri = $toc_root . trim(str_replace('./', '', _doc_import_groom_string($url_front)));
     
     $html_temp = trim(str_replace('./', '', _doc_import_groom_string($url_front)));
         
     $anchor = str_replace('X','', array_pop($url_pieces));
     
     $regex = '/^([a-zA-Z]{2,3})(\d{5,7}).htm$/siu';
     preg_match($regex, $html_temp, $splitfile);
     $prefix = $splitfile[1];
     $suffix = $splitfile[2];
     
     
     if(file_exists($final_directory . $toc_root . $anchor . '.htm')) {
  $uri = $toc_root . $anchor . '.htm';
  $anchor_files['present'][$anchor] = $topic_title;
  
  $loc = array_search($anchor . '.htm',$all_files);
  if ($loc) unset($all_files[$loc]);
     }
     else {
  if(file_exists($final_directory . $uri)) {
  $loc = array_search($html_temp,$all_files);
  if ($loc !== FALSE) {
      unset($all_files[$loc]);
      $anchor_files['present'][$anchor] = '>> ' . $html_temp;
  }
  else{
      $anchor_files['present'][$anchor] = '** missing or duplicated topic ** ' . $html_temp;
      $anchor_files['not'][$anchor] = $topic_title;
      
      } 
    
  }
     }
     
     // If the new level is not correctly structured (h2 following h1, h3 following h2) adjust it now.
     if ($previous_level !== FALSE && ($level > $previous_level && $level != $previous_level && $level != ($previous_level + 1))) {
  $level = $previous_level;
  $level++;
    
     }
     
     //dpm($uri,'uri');
     // If the URI is not already in the stack, add it. 
     $check = recursive_array_search($uri,$topics);
     if(($check === FALSE)&& substr($uri,-1,1) != '/') {
  
  $topics[] = array('uri' => $uri, 'original_level' => $level, 'level' => ($level+1), 'new_level' => NULL, 'this_prefix' => $prefix, 'this_id' => $suffix);
     }
     $previous_level = $level;
 }
 
    
 if(count($anchor_files['not'])) {
     $message_string = 'The following topic files are expected by the TOC, but are not in the archive. The import was allowed to finish, but <b>the resulting documents should not be published without thorough review.</b>
     This import should be undone, and these topics should be fixed in source and re-uploaded. <ul>';
     
     foreach($anchor_files['not'] as $missing) {
  $message_string.="<li><b>$missing</b></li>";
     }
     $message_string.="</ul>";
     $messages['error'][]=$message_string;
 }
 
 
 // Deal with leftover problems:
 
 foreach($all_files as &$leftover) {
     
     // front matter is prepended to topic, for now
     if(stripos($leftover, '_front_') !== FALSE) {
  array_unshift($topics, array(
     'uri' => $toc_root . $leftover,
     'original_level' => 0,
     'level' => 1,
     'new_level' => NULL,
     'title_override' => t('Important Notices'),));
  $leftover = NULL;
     }
     
     if(stripos($leftover, 'toc') !== FALSE) {
  $leftover = NULL;
     }
     
     if(stripos($leftover, '.css') !== FALSE) {
  $leftover = NULL;
     }
     
     if(is_dir($final_directory . $toc_root . $leftover)) {
  $leftover = NULL;
     }
    
 }
 $all_files = array_filter($all_files);
 //dpm($all_files,'remaining files');
    
 // What's in those files?
 
    
 for($i = 0; $i < count($topics); $i++) {
     $temp = file_get_contents($final_directory . $topics[$i]['uri']);
     //preg_match("/<title>(.*?)</title>/siu", $temp, $match);
     
     preg_match('/<!-- Split file for ID (\d.*?) here -->/siu', $temp, $ids);
     $topics[$i]['next_id'] = $ids[1] ? $ids[1] : NULL;
 }
    
 for($i = 0; $i < count($topics); $i++) {
    
     $this_id = $topics[$i]['this_id'];
     $next_id = $topics[$i]['next_id'];
     $x = $i+1;
     $actual_next_id = $topics[$x]['this_id'];
     
     $topic_processed[] = $topics[$i];
     
     while ($next_id && $next_id != $actual_next_id) {
  $file_index = array_search($next_id, $filenames['suffixes']);
  $nextfile = $filenames['prefixes'][$file_index] . $next_id . '.htm';
  
  $temp = file_get_contents($final_directory . $toc_root . $nextfile);
    
  // Get ID of next section
  preg_match('/<!-- Split file for ID (\d.*?) here -->/siu', $temp, $ids);
  $next_id = $ids[1] ? $ids[1] : NULL;
  
  $found_topic = array(
      'uri' => $toc_root . $nextfile,
      'original_level' => $topics[$i]['level'],
      'level' => $topics[$i]['level'] + 1,
      'this_prefix' => $filenames['prefixes'][$file_index],
      'this_id' => $filenames['suffixes'][$file_index],
      'next_id' => $next_id,
      'inserted' => TRUE,
      'content' => $temp,
  );
  $inserted_topic[] = $found_topic;
  $topic_processed[] = $found_topic;     
     
  // Keep going if the next topic still doesn't match
  
     }
    
 }
 unset ($topics);
 $topics = $topic_processed;
 
 // Front matter is prepended to array
 
 watchdog('IMPORT',"[".count($topics)."] topics were found. Loading topics and assembling master.");
    
 // Compress heading levels, removing gaps
 // For some reason, looping over $topics by reference ($key => &$topic) corrupts the last element in the array. Thus the
 // hack with $new_level and the splicing foreach loop after this.
 
 foreach($topics as $key => $topic) {
     if (!$floor || $topic['level'] < $floor) {
  $floor = $topic['level'];
  //dpm($floor,'floor set');
     }
     
     if($topic['level'] == $floor) {
  $new_level[$key]['new_level'] = $topic['level'];
  //dpm($floor,'floor == level');
  //continue;
     }
     
     if($topic['level'] > $floor) {
  $offset = $topic['level'] - $floor;
  //dpm($floor . '/' . $topic['level'] . '/' . $offset,'floor/level/offset');
  switch($offset){
      case 1: // Offset is 1 (floor is 1, current is 2 == normal, do nothing)
   $floor = $topic['level'];
   $new_level[$key]['new_level'] = $topic['level'];
   //dpm($floor, 'resetting floor');
   break;
      default: // Offset is higher than 1 (floor is 1, current is 4 == bad, lower level by offset-1)
   $new_level[$key]['new_level'] = $topic['level'] - ($offset-1);
  }
     } 
 }
    
 // Merge new level array into the topics array.    
 foreach ($topics as $key=>$topic) {
     $newtopics[$key] = $topic;
     $newtopics[$key]['new_level'] = $new_level[$key]['new_level'];
 }
    
 // Begin assembling HTML master file
     
 $topics = array();
 $topics = $newtopics;
 $master = '<!-- This file was assembled by the Technet import module -->' . PHP_EOL . $tool . PHP_EOL;
    
 foreach($topics as $topic_set){
     //dpm($topic_set,'topic_set');
     $topic = _doc_import_groom_string($topic_set['uri']);
     $level = $topic_set['level'];
     $new_level = $topic_set['new_level'];
     $original_level = $topic_set['original_level'];
     
     // HTML files can appear either as .htm or .html depending on Mif2Go configuration settings. Allow both.
     if(!file_exists($final_directory . $topic) && substr($topic,-1,1) == 'm'){
       $topic = $topic . 'l';
     }
    
     // Read topic contents, or error out. 
     if(!$processed_chunk = file_get_contents($final_directory . $topic)){
  if(file_exists($final_directory . $topic)) {
      drupal_set_message("The file [$topic] is required by the TOC, but is empty. Proceeding without it.",'warning');
      $processed_chunk = '<!-- This topic is missing or empty: ' . $topic . ' -->';
  }
  else {
      drupal_set_message("The file [$topic] is required by the TOC, but it is not on disk or is in the wrong place.",'error');     
      return FALSE;
  }
     }
     
     extract_body($processed_chunk);
     $processed_chunk = doc_import_adjust_headings($processed_chunk, $level, $new_level, $original_level, $topic_set['title_override']);
    
     $master .= PHP_EOL . "<!--Begin section $topic -->" . PHP_EOL . $processed_chunk;
 }
     
    
 
     // adjust $final_directory (append $toc_root) and save assembled master file;
     $master_uri = $final_directory . $toc_root . 'technet-master.html';
     $master_save = file_save_data($master, $master_uri, FILE_EXISTS_REPLACE);
  
    }//end of build master file//


    /*** Common -- Word and Frame files are processed identically from this point on ***/
    
    //find html file and extract metadata from it. This can be a Word source file or a master doc built by us.
    if(!$master_uri){
 // This is an original (Word) doc, so the filenames have not been lowercased/de-spaced yet.
 // @TODO: This should probably be refactored into a single function, called in two places.
 foreach($files as &$check) {
     $extension = pathinfo($check, PATHINFO_EXTENSION);
     
     if(($extension == 'htm' || $extension == 'html') && substr($check,0,1) != '~') {
  $html_file = $check;
  $path = explode('/',$check);
  if(count($path) > 1) {
      array_pop($path);
      $prefix = implode('/', $path) . '/';
  }
     }
 }
  
 // Get the TOC root. This is the part of the path between the archive directory and the HTML file ("_wrap/", etc.) 
 $temp_path = explode('/',$html_file);
 array_pop($temp_path);
 $toc_root = implode('/',$temp_path) . '/';
      
 if(!$html_file){
     drupal_set_message('No HTML file was found in the uploaded archive. To proceed, edit this page and remove the existing archive. Then upload an archive containing HTML.','error');
     return FALSE;
 }

 $raw_html = file_get_contents($final_directory . $html_file);
 
 // Verify encoding is UTF-8, and if it is not, convert it. 
 $enc = mb_detect_encoding($raw_html, 'UTF-8', TRUE);
 if (!$enc) {
     $raw_html = mb_convert_encoding($raw_html, 'UTF-8', 'WINDOWS-1252');
     drupal_set_message('Converted document encoding to UTF-8', 'status');
 }

 // Define $author_tool: found in HTML <meta> tags.
 $author_tool = doc_import_get_author_tool($raw_html);
 // If no expected tool was found, abandon ship.
 if(!$author_tool){
   drupal_set_message('Could not identify the tool used to create this document. This probably means that it was saved using the wrong file format (Word files must be saved as "filtered HTML"). Document was not imported.','error');
   return FALSE;
 }
   
 // Strip extra HTML, if any has been introduced by QueryPath
 extract_body($raw_html);
 
 // Check headings if a mif2go generated doc.
 if($author_tool['tool'] == 'mif2go') {
     $raw_html = doc_import_adjust_headings($raw_html, 1, 1, 0);
 }
 
    }
    else {
 
 // This is a rebuilt master document. All file grooming has already been done.
 $prefix = $toc_root;
 $raw_html = $master;
    }
  
  
    // Do crazy table manipulation to re-class or re-order elements
    $raw_html = doc_import_unpack_fix_tables($raw_html, $author_tool);
      // Whack special characters in hrefs, names, and ids
    $raw_html = doc_import_fix_link_attributes($raw_html);

  // Encrypt special characters. This protects data from QueryPath parsers, which choke on "&#" strings.
  // This converts all "&" chars to "%%%". These MUST be decrypted prior to use as HTML. Obviously.
  protect_special_characters($raw_html);
      //dpm($raw_html,'raw html');

  // This next line can probably be removed -- it's very old code. @TODO: run tests on this!
  $raw_html = str_replace('&amp;','&',$raw_html);
  
    // Tidy HTML with htmLawed  
    $config = array(
      'deny_attribute' => 'style',
      'comment' => 1,
      'cdata' => 1,
      //'tidy' => 1,
      'balance' => 0,
      'keep_bad' => 1,
    );
    
    $raw_html = htmLawed($raw_html, $config);
  
    // Get a list of all <img> sources in the raw html
    preg_match_all('/<img.*?src="(.*?)".*?>/i', $raw_html,$matches);
    $image_sources = $matches[1];

    // Rename image src in HTML to match renamed files (lowercased + spaces converted to underscores)
  if (isset($image_sources)) {
    $bad_src = array();
      $raw_html = preg_replace_callback(
        '/<img.*?src="(.*?)".*?>/isu',
        function($matches) use ($toc_root, &$bad_src, &$image_sources){
          $full = $matches[0];
          $src = $matches[1];
          $new_src = $toc_root . _doc_import_groom_string($src);
          
          // Mif2Go can sometimes export .wmf or .psd files. Change these src= attributes to ~.png and alert the user.
          if(strpos($src,'wmf') != 0){
            if(!in_array($src, $bad_src)) $bad_src[] = $src;
            $new_src = str_replace('.wmf','.png',$new_src);        
          }
          if(strpos($src,'psd') != 0){
            if(!in_array($src, $bad_src)) $bad_src[] = $src;
            $new_src = str_replace('.psd','.png',$new_src);        
          }
          
          // Strip leading slash if it exists in filepath
          if(substr($new_src, 0, 1) == '/') {
            $new_src = substr($new_src, 1);
          }
          
          // Add the modified src to the image_sources array
          if(!in_array($new_src, $image_sources)) {
            $image_sources[] = $new_src;
          }      
          return str_replace($src, $new_src, $full);
        },
        $raw_html
      );
    
    if(count($bad_src)) {
      $warning_string = "The following source images are in illegal formats. Graphics must be either <b>png</b>, <b>jpg</b>, or <b>bmp</b> files.
      The html references have been updated to use <b>.png</b>. These will appear as broken images until replacement files are uploaded. <ul>";
  
      foreach($bad_src as $warning) {
        $warning_string.="<li>" . substr($warning, -4) . ': <b>' . $warning . '</b></li>';
      }
     
      $warning_string.="</ul>";
      $messages['error'][] = $warning_string;
    }
  }
   
  // Regenerate image sources array for further checks
  preg_match_all('/<img.*?src="(.*?)".*?>/i', $raw_html,$matches);
    $image_sources = $matches[1];

  
    // Build an array of stripped filenames (minus path and extension) for comparison
    $bad_files = array_map(function($i){
						$fragment = array_pop(explode('/',$i));
						return _doc_import_groom_string(substr($fragment, 0, (strlen($fragment)-4)));
				}, $bad_src);
    
    $missing = array();
    foreach($image_sources as $image_source){
    //@TODO: next line throws a strict warning, passing functions by reference
    $check = (array_shift(explode('.',array_pop(explode('/',$image_source)))));
    if(!file_exists($final_directory . $image_source) && !in_array($check, $bad_files)){
      $missing[] = "<b>$image_source</b> (should be in <i>$final_directory</i>)";
    }
    }
    if(count($missing)) {
    $msg_string = 'The following files are used by the document, but are not present in the archive. These will most likely need to be fixed or replaced before publication:<ul>';
    foreach($missing as $oops) {
      $msg_string.="<li>$oops</li>";
    }
    $msg_string.="</ul>";
    $messages['error'][] = $msg_string;
    }
    
    // Remove unreferenced files from the server.
    $filter_extensions = array('png','jpg','pdf','eps','wmf','ai','db','mso');
    $counter = 0;
    foreach($files as $file){
 
    $file = _doc_import_groom_string($file);
    $pieces = explode('.', $file);
    $extension = array_pop($pieces);
    if (in_array($extension, $filter_extensions)){
      if (!in_array($file,$image_sources)){
        $deleted_files[] = $file;
        file_unmanaged_delete($final_directory . $file);
        $counter++;
      }
    }
  }
    
		if(count($deleted_files)) {
		$message = '<button class="btn btn-default alert-toggle collapsed" data-toggle="collapse" data-target="#deleted-files">(' . count($deleted_files) . ') files were not used, and have been deleted. Click for details.</button><ul id="deleted-files" class="collapse">';
		foreach($deleted_files as $byebye) {
						$message.="<li>$byebye</li>";
		}
		$message.="</ul>";
		$messages['status'][] = $message;
		}
		
		// Remove other useless odds and ends that Word includes
		if(file_exists($final_directory . '/images/Thumbs.db')) file_unmanaged_delete ($final_directory . '/images/Thumbs.db');
		if(file_exists($final_directory . 'editdata.mso')) file_unmanaged_delete ($final_directory . 'edit.mso');
		
		// Make sure ampersands are properly encoded. HTMLawed can bork this.
		$raw_html = str_replace('&amp;%','&%',$raw_html);
		$raw_html = str_replace('&','%%%',$raw_html);
		
		// Save updated raw HTML file as a managed file, and add it to the node.
		// If a copy already exists, delete it and save the new one.

		$new_html_uri = $final_directory . 'raw_html_' . $ticket->nid . '.html';
		
		if(file_exists($new_html_uri)) {
						file_delete($new_html_uri);
						drupal_set_message('Existing raw html file has been removed and replaced with a new copy.','warning');
		}
		$saved_file = file_save_data($raw_html, $new_html_uri, FILE_EXISTS_REPLACE);

		if (!$saved_file) {
						drupal_set_message('Unable to save raw HTML file.','error');
						return FALSE;
		}
		
		//create field array for new file and attach it
		$ticket->field_raw_html_master[LANGUAGE_NONE][0] = (array)$saved_file;
		$ticket->field_raw_html_master[LANGUAGE_NONE][0]['display'] = 1;
		
		$author_tool_array = array('value' => $author_tool['tool'], 'format' => NULL, 'safe_value' => $author_tool['tool']);
		$ticket->field_authoring_tool[LANGUAGE_NONE][0] = $author_tool_array;
		node_save($ticket);
		
		//document conversion complete, update workflow state
		$comment = t("State changed by import module (unpack archive)");
		$new_state = workflow_execute_transition($ticket, WORKFLOW_TICKET_IMPORTING_STATE, $comment, TRUE);
		entity_get_controller('node')->resetCache((array)$ticket->nid);

		// Report messages
		if (isset($messages)) {
				foreach ($messages as $type => $message_list) {
						foreach ($message_list as $list) {
								drupal_set_message($list, $type);
						}
				}
		}
		drupal_set_message('Document state set to "Importing"');
		return TRUE;
} 

/**
 * Recursively iterate through a directory, lowercasing filenames and replacing spaces with underscores.
 *
 * Also deletes macosx garbage files.
 *
 * @param $directory: a valid URI
 */
function _doc_import_groom_filenames($directory) {
  if(!is_dir($directory)) {
    return FALSE;
  }
  foreach(scandir($directory) as $file) {
				if ($file == '.' || $file == '..') continue;
				
				
				// Normal files can be renamed directly
				if (!is_dir($directory . $file)) {
						$file_renamed = _doc_import_groom_string($file);
						$success = rename ($directory . '/' . $file, $directory . '/' . $file_renamed);
				}
				else {				
						// Directories cannot be renamed, and must be re-created with a new name.
						// This is a limitation of Pantheon's file system.
						$file_renamed = _doc_import_groom_string($file);
						if ($file_renamed != $file) {
								$tempdir = '/temp_rename_dir';
								// Copy the directory to a temp location
								copy_recursive($directory . '/' . $file, $directory . $tempdir);
								
								// Remove the original (whoa)
								_rmdir_recursive($directory . '/' . $file);
								
								// Copy the temp directory to the renamed location
								copy_recursive($directory . $tempdir, $directory . '/' . $file_renamed);
								
								// Dispose of the temp directory
								_rmdir_recursive($directory . $tempdir);
						}
						
				}
				if ($success === FALSE) {
						drupal_set_message('Directory could not be created: ' . $directory . $prefix . $file_renamed, 'error');
				}
				_doc_import_groom_filenames($directory . $prefix . $file_renamed);
		}
}

/**
 * Utility function that grooms a string.
 *
 * Operations performed are:
 *  - Lowercase string
 *  - Replace spaces and html-coded spaces (%20) with underscores
 *
 * @param $string: the string to manipulate.
 *
 * @return: the manipulated string.
 */
function _doc_import_groom_string ($string) {
    
    // These functions are broken out individually for clarity
    
    // Replace spaces, ampersands, and protected/converted ampersands to underscores
    $string = str_replace(array(' ', '%20', '%%%amp;', '&amp;', '&'), '_', $string);
    
    // urlencode whatever is left to catch other illegal characters
    //$string = urlencode($string);
    
    // lowercase result
    $string = strtolower($string);
    
    return $string;
}

/**
 * Scan an HTML snippet for meta tags with the ID stamp of the application used to create it.
 *
 * @param $string (string)
 *  The content to parse.
 *
 * @return
 *  An information array expected by doc_import, or FALSE if no tool was identified.
 */
function doc_import_get_author_tool ($string) {
    $author_tool = FALSE;
    
    //Word 14 Filtered Docs
    if (strpos($string,'<meta name=Generator content="Microsoft Word 14 (filtered)">') !== FALSE){
 $author_tool = array(
     'tool' => 'msword',
     'version' => 14,
     'conversion_type' => 'filtered',
 );
    }
    
    //Word 15 Filtered Docs
    if (strpos($string,'<meta name=Generator content="Microsoft Word 15 (filtered)">') !== FALSE){
 $author_tool = array(
     'tool' => 'msword',
     'version' => 15,
     'conversion_type' => 'filtered',
 );
    }
    
    //Frame 10 HTML export filter docs
    if (strpos($string,'<META NAME="GENERATOR" CONTENT="Adobe FrameMaker 10.0/HTML Export Filter">') !== FALSE){
      $author_tool = array(
     'tool' => 'frame',
     'version' => 10,
     'conversion_type' => 'HTML_Export_Filter',
      );
    }
    
    //Frame 11 HTML export filter docs
    if (strpos($string,'<META NAME="GENERATOR" CONTENT="Adobe FrameMaker 11.0/HTML Export Filter">') !== FALSE){
      $author_tool = array(
     'tool' => 'frame',
     'version' => 11,
     'conversion_type' => 'HTML_Export_Filter',
      );
    }
    
    //Mif2Go exported docs
    if (strpos($string, 'generated by DCL filter dwhtm') !== FALSE){
      preg_match('/dwhtm,.*-->/', $string, $version); //get version and filter id, included after "dwhtm"
      $author_tool = array(
     'tool' => 'mif2go',
     'version' => trim(str_replace('-->','',$version[0])), //lop off the closing comment tag
     'conversion_type' => 'DCL filter dwhtm',
 );
    }
    return $author_tool;
}

/**
 * Adjusts document headings, resulting in an ordered progression of Head1>Head2>Head3, etc.
 * Accounts for non-numbered heading styles like "TitleTOCYes".
 *
 * @param $topic (string)
 *  The content to check and adjust
 *  
 * @param $level (integer)
 *  The heading level of the topic.
 *
 * @param $new_level (integer)
 *  If altered, the new level value.
 *
 * @param $original_level (integer)
 *  If altered, the original level value.
 */
function doc_import_adjust_headings($topic, $level, $new_level = NULL, $original_level = NULL, $title_override = NULL) {
    // strip extra HTML from content (everything outside of <body> tags)
    //$topic = extract_body($topic);

    // Strip linebreaks: replace with spaces, then replace any resulting doublepsaces with single spaces.
    $topic = str_replace('  ', ' ', str_replace(PHP_EOL,' ',$topic));
    // Unify variations on heading classes. De-gerund "heading" into "head".
    $topic = str_ireplace(array('Heading1','Heading2','Heading3','Heading4','Heading5','Heading6'), array('Head1','Head2','Head3','Head4','Head5','Head6'),$topic);

    // Some classes include "title" -- these are topic titles, and will be converted to head1.
    $convert_to_head1 = array ('TitleChapter', 'TitleIntro', 'ChapterName', 'TitleTOCYes', 'TitleCover');
    //$topic = str_ireplace($convert_to_head1, 'Head1 title-ex', $topic);
    
    // If the adjusted head level is found in the HTML, move internal headings down a level to avoid conflicts.
    // Record original heading level as a class "ex-#"; for example, if a Head1 moves to a Head2, add a class "ex-1".
    if (stripos($topic, 'head' . $new_level) !== FALSE) {
    for($i = 3; $i >= $new_level; $i--) {
      $find = 'head' . $i;
      $replace = 'head' . ($i + 1) . ' document subtitle ex-' . $i;
      $topic = str_ireplace($find, $replace, $topic);
    }  
    }
    
    // Look for any of the converted title headings. If any are found, the other headings will be bumped down.
    foreach($convert_to_head1 as $convert) {
    if(stripos($topic, $convert) !== FALSE) {
      $head_conversion = TRUE;
      break;
    }
    }
    
    if($original_level != 0 || $head_conversion != TRUE) {
    $find = 'head' . $original_level;
    $replace = 'head' . $new_level . ' ex-'.$original_level;
    $topic = str_ireplace($find, $replace, $topic);
    }
    
    // If any converted title headings were found, bump the rest down a level.
    if($head_conversion) {
        $topic = str_ireplace($convert_to_head1, 'head1 ex-title', $topic);    
    }
    $topic = str_ireplace(array('<nobr>','</nobr>'),'',$topic);

    // Replace <p> tags of class Head# with corresponding <H#> tags.
    // Note: This is an extremely fragile regex that depends on well-formed HTML.
    $find = "/<p (class=\"head(\d).*?)>(.*?)<\/p>/iu";
    $replace = $title_override ? "<h$2 $1>$title_override</h$2>" : "<h$2 $1>$3</h$2>";
    $topic = preg_replace($find, $replace, $topic);
    return $topic;
}

/**
 * Perform surgery on tables prior to splitting and prior to HTMLawed cleanup.
 * Currently, fixes the following issues:
 *  -- flags "FeatureSummaryTable" table <tr>s containing a top border for conversion to <thead><tr>s
 *  -- finds and fixes bad Frame/Mif2Go table HTML
 *
 *  @param $html
 *   The master HTML string to work on. Represents the entire document.
 *
 *  @param @tool
 *   The author tool used to create the document (Word or Mif2Go).
 */
function doc_import_unpack_fix_tables($html, $tool) {
    switch ($tool['tool']) {
  case 'msword':

    // Flag "FeatureSummaryTable" table <tr>s containing a top border for conversion to <thead><tr>s

    $html = preg_replace_callback(
    '/<td\s(?:.*?)\s?(?:width="?\d+"?)\s?(colspan="?\d+"?)?\s?(rowspan="?\d+"?)?.*?>(.*?)<\/td>/siu',
    function ($matches){
      $class = '';
      if (stripos($matches[0], 'border-top:solid') !== FALSE) {
        $class .= "border-top ";
      }
      if (stripos($matches[0], 'border-bottom:solid') !== FALSE) {
        $class .= "border-bottom ";
      }
     
      if ($class) {
        return '<td class="' . trim($class) . '" '. $matches[1] . ' ' . $matches[2] . '>' . $matches[3] . '</td>';
      }
      else return $matches[0];
    },
    $html);

     break;
 
 case 'mif2go':
     
     // Scan all tables and pull out illegal tags, re-inserting them before the table element instead.
     // These can be captions, h1, h2, or h3 elements erroneously placed there by Frame or Mif2Go.
     $regex = '/<table.*?>.*?<\/table>/sui';
     $html = preg_replace_callback(
  $regex,
  function($item){
      $backup = $item;
     //dpm((array)$backup,'backup');
      foreach(array('caption') as $check) {
   $regex_inner = "/(.*?)(<table.*?>.*?)(<$check.*?>(.*?)<\/$check>)(.*?<\/table>)/siu";
   $temp = preg_replace_callback(
       $regex_inner,
       function($inner) use ($check) {
    switch($check) {
        case 'caption':
     // Captions are inserted last, immediately before the table.
     $reshuffled = $inner[1] . $inner[4] . PHP_EOL . $inner[2] . $inner[5];
     break;
        
        default:
     // Everything else unwinds in reverse order, so h1 ends up first and h3 last.
     $reshuffled = $inner[3] . $inner[1] . PHP_EOL . $inner[2] . $inner[5]; 
     break;
    }
    return $reshuffled;
       },
       $item);
   if($temp) {
       $item = reset($temp);
   }
      };
      if(!$item) {
   return reset($backup);
      }
      else {
   return is_array($item) ? reset($item) : $item;
      }
  },  
  $html);
  
     break;
    }
    return $html;
}

/**
 * Find link names and anchors, and flatten non-alphanumeric characters into underscores.
 *
 * @param $content
 *  String to groom.
 *
 * @return
 *  Adjusted string.
 */
function doc_import_fix_link_attributes($content) {
    //$content = str_replace(PHP_EOL, ' ', $content);
    $regex = '/<a\s.*?(?:href|name)="?(?!(?:http|mailto)).*?"?>/siu';
    
    $content = preg_replace_callback(
 $regex,
 function($link) {
     //dpm($link[0],'link found');
 if(stripos($link[0],'_Toc') !== FALSE || stripos($link[0], 'http:') !== FALSE || stripos($link[0], 'mailto:') !== FALSE) return $link[0];
  //First, replace UTF-8 characters.
 $link[0] = str_replace(
  //left ' quote right ' quote left " quote right " quote en-dash  em-dash  ellipse  hyphen  bullet  TM  white bullet em-space check mark heavy check mark
  array("\xe2\x80\x98",  "\xe2\x80\x99", "\xe2\x80\x9c", "\xe2\x80\x9d", "\xe2\x80\x93", "\xe2\x80\x94", "\xe2\x80\xa6", "\xe2\x80\x90", "\xe2\x80\xa2", "\xe2\x84\xa2", "\xe2\x97\xa6", "\xe2\x80\x83", "\xe2\x9c\x93", "\xe2\x9c\x94"),
  '_',  
  $link[0]);
 
 $link[0] = str_replace(array('&',';','®','/',',','’'),'-',$link[0]);
 //dpm($link[0],'link repaired');
 return $link[0];
 },
 $content);
    return $content;
    
}