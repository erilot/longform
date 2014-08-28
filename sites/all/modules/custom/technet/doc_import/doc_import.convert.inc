<?php

/**
 * File: doc_import.convert.inc
 * Functions that adapt and convert incoming code to Technet styles and formatting.
 */

 /**
 * Run conversion steps on a single topic.
 *
 * @param $topic
 * 	A loaded topic node object
 *
 * @param $author_tool
 * 	The tool used to create the HTML. Can be 'word' or 'mif2go'.
 *
 * @param $controller
 * 	The loaded import_ticket node controlling this document.
 * 	If calling from an import ticket page, this can be omitted (it will be loaded automatically).
 * 
 */
function doc_import_convert_topic($topic, $author_tool, $controller = NULL) {
    
    module_load_include ('inc', 'doc_import');
    if (!$topic || !is_object($topic) || $topic->type != 'topic') return FALSE;
    if (!$controller) {
	$controller = menu_get_object();
    }
    $section = $topic->field_topic_source_content[LANGUAGE_NONE][0]['value'];
    
    extract_body($section);
    //run remainder of frame preprocess
    if($author_tool == 'mif2go' || $author_tool == 'frame'){
      doc_import_frame_preprocess($section, $author_tool);
      extract_body($section);
    }

    //remove hard-coded style definitions	
    remove_style($section);
    
    // Correct encoding errors
    doc_import_correct_encoding_errors($section);

    // Process symbol characters
    doc_import_process_symbols($section);
    
    //Convert embedded classes to definio document classes.
    extract_body($section);    

    doc_import_transliterate_classes($section, $author_tool);

    doc_import_convert_classes($section, $author_tool);

    doc_import_remove_junk($section);
    
    doc_import_style_code($section);

    //process tables
    extract_body($section);
    doc_import_process_tables($section, $author_tool);
  
    //Process HTML lists
    extract_body($section);
    doc_import_process_lists($section, $author_tool);

    //correct image URLs
    extract_body($section);
    doc_import_final_images($section, $controller);

    //clean up printisms
    extract_body($section);
    doc_import_clean_printisms($section);
    
    // Add body anchors to anchor array
    $targets_old = json_decode($topic->field_topic_anchors[LANGUAGE_NONE][0]['value'], TRUE);
    if(!$targets_old['head'] && count($targets_old['head']) !=0) {
        $targets['head'] = $targets_old;	
    }
    else {
	$targets['head'] = $targets_old['head'];
    }
    
    $targets['body'] = _doc_import_get_content_anchors($section);
    $topic->field_topic_anchors[LANGUAGE_NONE][0]['value'] = json_encode($targets);
    //doc_import_links_preprocess($section);
    extract_body($section);
    //Process links
    //dpm($content_sectioned,'content sectioned');
    //doc_import_prepare_links($section, $i, $controller->nid);

    //take care of other processing items that should be done last, after everything else is finished
    $section = doc_import_last_train_processes($section, $topic);
    
    $topic->field_topic_processed_html[LANGUAGE_NONE][0]['value'] = $section;
    $topic->field_topic_processed_html[LANGUAGE_NONE][0]['format'] = 'full_html';
    
    node_save($topic);

    
    //DEVELOPMENT
    //REMOVE OR COMMENT OUT BEFORE CONVERTING AN ENTIRE BOOK
}

 
/**
 * Batch processes the topics in the controller's "topic_used_in" relation.
 * These functions adapt the incoming code to Technet styles and standards.
 * 
 */
function doc_import_convert(){
  
  $controller = menu_get_object();

  //get the "topic_used_in" relation endpoints. These will be the topics to convert.
  $query = relation_query('node', $controller->nid, 'is_a_topic_in_');
  $topic_relation = $query->execute();
  $topic_relation = array_shift($topic_relation);
  $topic_relation = relation_load($topic_relation->rid);
  
  $pages = $topic_relation->endpoints[LANGUAGE_NONE];
    if(!isset($pages)){
      drupal_set_message('No relation was found to work with.','error');
      return FALSE;
    }
    
    //set up batch
    $operations[] = array('_doc_import_convert_operation', array($pages));
    $operations[] = array('doc_import_rebuild_topic_links', array($controller));
    $batch = array(
      'operations' => $operations,
      'file' => drupal_get_path('module', 'doc_import') . '/doc_import.convert.inc',
      'finished' => '_doc_import_convert_finished',
      'title' => t('Processing HTML in topics...'),
      'init_message' => t('Starting processes...'),
      'progress_message' => t('Completed @current task out of @total.'),
      'error_message' => t('Error while converting topics.'),
    );
    
    batch_set($batch);
    batch_process();
  
  return FALSE;
  //retrieve the segmented content array (preprocessed)
  $content = _doc_import_get_processed_file($controller, 'pre');

  //dpm($content,'content before processing'); 
  add_log_entry('info','Beginning main HTML processing',$log);
  $author_tool = $controller->field_authoring_tool[LANGUAGE_NONE][0]['value'];
  $final_dir = "public://import-source-files/".$controller->nid."/";
	
  $total = count($content);
  watchdog('import cron','Beginning HTML conversion for [' . $total . '] sections');  
  $i = 0;
  
  add_log_entry('info','Finished main HTML processing',$log); 
  
  //dpm($content,'content array after processing');
  
  //Save two copies of the converted document: One HTML for quick review, and one JSON array for further processing.
  $new_file_uri = $final_dir . 'processed_html_' . $controller->nid . '.html';
  add_log_entry('info','Saving processed file as '.$new_file_uri, $log);
  $new_file = file_save_data(doc_import_construct_html_file($content), $new_file_uri, FILE_EXISTS_REPLACE);
  if($new_file){
    add_log_entry('info','Processed HTML saved',$log);
  }
  else{
    add_log_entry('error','Error saving processed file',$log);
    drupal_set_message('Unable to save processed HTML master file.','error');
    return FALSE;
  }
  
  //save converted content array as JSON data
  add_log_entry('info',"Converting final array to JSON object",$log);
  $processed_json_uri = $final_dir . 'json_processed_' . $controller->nid . '.json';  
  $converted_content = json_encode($content);
  add_log_entry('info','Saving processed JSON object as '.$processed_json_uri, $log);
  $processed_json = file_save_data($converted_content, $processed_json_uri, FILE_EXISTS_REPLACE);
  if($processed_json){
    add_log_entry('info','Processed JSON file saved',$log);
  }
  else{
    add_log_entry('error','Error saving processed JSON file',$log);
    drupal_set_message('Unable to save processed JSON file.','error');
    return FALSE;
  }
  ////debug check contents of files as saved
  //dpm(file_get_contents($new_file_uri),'file saved and re-read');
  //dpm(json_decode(file_get_contents($processed_json_uri),TRUE),'processed JSON file saved and re-read');
  
  $controller->field_imported_html_master[LANGUAGE_NONE][0] = (array)$new_file;
  $controller->field_imported_html_master[LANGUAGE_NONE][0]['display'] = 1;
  $controller->field_processed_json[LANGUAGE_NONE][0] = (array)$processed_json;
  $controller->field_processed_json[LANGUAGE_NONE][0]['display'] = 1;
  
  add_log_entry('info','Saving/updating node ' .$controller->nid,$log); 
  node_save($controller);
  add_log_entry('info',"Node ".$controller->nid." saved successfully",$log);


  doc_import_update_log($log, $controller->nid);
  return TRUE;
}

function _doc_import_convert_operation($pages, &$context){
  // information needed to track progression between successive calls.
    if (empty($context['sandbox'])) {
	$context['sandbox'] = array();
	$context['sandbox']['progress'] = 0;
	$context['sandbox']['current_topic'] = 0;

	// Save topic count for the termination message.
	$context['sandbox']['max'] = count($pages);
	
	// Pass ticket ID through to "finished"
	$context['results']['ticket'] = $pages[0]['entity_id'];
    }

    $controller = node_load($pages[0]['entity_id']);
    $author_tool = $controller->field_authoring_tool[LANGUAGE_NONE][0]['value'];
    // Retrieve the next group of sections
    $result = array_slice($pages, $context['sandbox']['current_topic'],1); //last parameter is $limit. Caps number of jobs that can be run at once.

    foreach ($result as $section) {
	if ($section['entity_id'] == $controller->nid) continue;
	$topic = node_load($section['entity_id']);
	$i++;
	watchdog('import cron','Beginning HTML conversion for section '.$i.' of '.$total.' ['. trim($part['title']).'].');  
	
	doc_import_convert_topic($topic, $author_tool, $controller);
	$context['results']['processed'][] = $section;
  
    }
    
    $context['results']['endpoints'] = $relation_endpoints;

    // Update our progress information.
    $context['sandbox']['progress']++;
    $context['sandbox']['current_topic']++;
    $context['message'] = 'Processing topic ' . $context['sandbox']['current_topic'] . ' of ' . $context['sandbox']['max'] . ': ' . $topic->title;

    if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
	$context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
    }
}

function _doc_import_convert_finished($success, $results, $operations){
  if($success){
	
				//add lock file to resources, indicating this process has been completed.
				$timestamp = time();
				global $user;
				$file = "public://longform/imports/expanded/".$results['ticket']."/converted.lck";
				file_unmanaged_save_data('Converted by ' . $user->name . ' at ' . $timestamp, $file, FILE_EXISTS_REPLACE);

				drupal_set_message('Content conversion completed successfully.');
  }
  else{
				drupal_set_message('Error while converting content.','error');
  }
}

/**
 *  Correct Frame-generated HTML
 *  
 */
function doc_import_frame_preprocess(&$doc, $tool) {

    //Deal with H4 content: convert plain p.head4 into bullet intros, and head4 ex-3 to proper H4 tags.
   
    $doc = extract_body(htmlqp($doc, 'p.Head4.ex-3')->each(function($index,$item) {
	$open = '<h4 class="original-head-3">';
	$close = '</h4>';
	swap_tags(qp($item),$open, $close);
    })
    
    ->top()->find('p.Head4')->each(function($index,$item){
	return qp($item)->removeClass('Head4')->addClass('bulleted-task-introduction original-head-4');
    })
    
    ->top()->html());


    //dpm((array)$doc,'document in frame preprocess');
  
    $doc = str_ireplace('<p class="Body">&amp;%bull;', '<p class="list-bullet-1">',$doc);
    $doc = str_ireplace('<span class="courier">&#9679; </span>', '', $doc);
    $doc = str_replace('QUG"','"',$doc);
  
    // Miscellaneous content bashing follows
    $qp = htmlqp($doc, 'body')
	->find('.Chapter-, .Chapter')->remove()																	//.Chapter-xyz classes are xref links only and are discarded
	->top()->find('h1.TitleFront')->remove()					//h1.TitleFront classes are above H1 (i.e., H0) and are discarded 
	->top()->find('.TitleIndex')->nextAll()->andSelf()->remove()			//.TitleIndex headings start the Index section, which is excluded from the HTML doc. Remove from here to end of doc.
	->top()->find('a[name*="pgfId-"], a[name*="marker-"]')->remove()		//These <a> tags are internal Frame markers and are discarded
	->top()->find('p.body-text br, h1 br, h2 br, h3 br, li br')->remove()		//Drop empty line breaks inside <p> and <H*> elements
	->top()->find('.TableAnchor, .FigAnchorIndent')->remove()

	// FigAnchor may actually be needed? preserving for regression if need be.
	//->top()->find('.FigAnchor, .AnchorTbFig')->remove()
	
	->top()->find('a[name^="X"]')->remove()							
      
    //Delete empty <map> and <b> tags
    //->top()->find('table.Box')->removeClass('Box')->addClass('table-note note')
    ->top()->find('map');
    
    foreach ($qp as $link){	
	if ($link->contents()->size() == 0){
	    $link->end()->remove();
	}
    }

    $qp->top()->find('caption>p.TableCap')->addClass('caption')->parent('caption')->contents()->unwrap();
    $qp->top()->find('.TaskIntroduction')->addClass('bulleted-task-introduction')   
	->top()->find('.ListNum')->prev('.TaskIntroduction')->addClass('numbered-task-introduction')->removeClass('bulleted-task-introduction')->removeClass('TaskIntroduction')
	->top()->find('.ListNum')->addClass('list-number-1')->removeClass('ListNum')
	->top()->find('.ListNum3')->addClass('list-number-3')->removeClass('ListNum3')
	->top()->find('.ListAlpha')->addClass('list-number-2')->removeClass('ListAlpha')
	->top()->find('span.zBullet:contains(bull)')->parent()->addClass('list-bullet-1')
	->top()->find('span.zBullet:contains(mdash)')->parent()->addClass('list-bullet-2')
	->top()->find('.list-number-1')->prev('.Body, .body-text')->addclass('numbered-task-preface')->removeClass('Body')
	->top()->find('.list-bullet-1')->prev('.Body, .body-text')->addclass('bulleted-task-preface')->removeClass('Body')
	->top()->find('span.Title')->contents()->unwrap()
	->top()->find('.zTask, .zBullet, .zTaskOne'/*, span.wingdings'*/)->remove()
	->top()->find('p.ChapterName')->wrap('<h1 class="converted"/>');
  
    //Clean out hard-coded bolding and preset anchor links in headers
    $qp->top()->find('h1, h2, h3, .TaskIntroduction')->find('b')->contents()->unwrap();
    
    //Remove "on page xxx" links [Part 1: remove <a> tags]
    $qp->top()->find('a.Chapnumpagenum')->remove();
    
    //deconvert from QP object to HTML
    $doc = extract_body($qp->top()->html());
    
    
    //Repair broken list items with embedded graphics
    $regex = ' /((<\/li>)$(\s*?<img.*?>.*?))((<\/ul>)|(<\/ol>))/ism';
    preg_match_all($regex,$doc,$out);
    for($i=0;$i < count($out[0]);$i++){
	$replace = $out[3][$i].$out[2][$i];
	$doc = str_replace($out[1][$i],$replace,$doc);
    }	
    //dpm((array)$doc,'after frame preprocess');	
    return $doc;
}

/**
 * Removes all hard-coded style definitions from document
 */
function remove_style(&$section){
    $section = extract_body(htmlqp($section)->find('table')->removeAttr('cellspacing')->removeAttr('cellpadding')->top()
	->remove('style')
	->top()->find('p, div, span, img')->removeAttr('style')->top()
	->html());
}		

/**
 * Transliterate classes from various source class names to unified Technet class names. Does not do any DOM manipulation.
 *
 */
function doc_import_transliterate_classes(&$section, $author_tool){

    $table = _doc_import_class_conversion_table();
    
    $classes = $table['classes'];
    _doc_import_bracket($classes);
    //'classes' is a keyed array of format "technet class" => array("template classes").
    foreach ($classes as $technet_class => $template_class){
	//Replace all occurrences of corresponding template classes with their Technet class.
	$section = str_ireplace($template_class, '"'.$technet_class.'"', $section);
    }

}

/**
 * Convert classes from old/source names to Technet styles
 */
function doc_import_convert_classes(&$doc, $author_tool) {
  switch ($author_tool){
    
    case 'msword':
	$doc = str_replace ('<h1>', '<h1 class="document">', $doc);
	$doc = str_replace ('<h2>', '<h2 class="document">', $doc);
	$doc = str_replace ('<h3>', '<h3 class="document">', $doc);
	$doc = str_replace ('<h4>', '<h4 class="document">', $doc);
	  
	// Remove spans
	// ** This is disabled -- removing spans this way also eliminates character styles (like Courier).
	// **  @TODO: Why was this implemented in the first place? Troubleshoot that and discover a smarter way to do it.
	$doc = extract_body(htmlqp($doc,'span')/*->contents()->unwrap()*/
	
	//stamp CSS version on top-level div
	->top()->find('div')->addClass('css-set-2')
	
	//If there are any fake bullet lists (bullets in body-text tags), convert them		  
	->top()->find('p.body-text')->each(function($index,$item){
	    $qp = qp($item);
	    $text = $qp->text();
		if(stripos($text,'&middot;') === 0 || strpos($text,'·') === 0) {
		    $qp->removeClass('body-text')->addClass('list-bullet-1');
		}
	    })
  
	// Find p.graphic elements with text and determine if they should really be something else
	->top()->find('p.graphic')->each(function($index,$item) {
	    $node = qp($item);
	    $text = $node->branch()->text();
	    
	    if(trim($text) && trim(htmlentities($text)) != '&Acirc;&nbsp;'){
		$decimal_pos = strpos($text,'.');
		
		// If the first character is numeric and a decimal is in either spots 2 or 3, it's really a list item.
		if(is_numeric(substr(trim($text),0,1)) && ($decimal_pos == 1 || $decimal_pos == 2)) {
		    $node->removeClass('graphic')->addClass('list-number-1');
		}
	    }
	})
	->top()->html());
	
	break;
    case 'mif2go':
    case 'frame':
      //Convert Frame Classes
      $qp = htmlqp($doc)
      ->top()->find('.IconNote')->closest('table')->addClass('note note-note')
      ->top()->find('.IconCaution')->closest('table')->addClass('note note-caution')
      ->top()->find('.iconWarning')->closest('table')->addClass('note note-warning')
      
      ->top()->find('h1,h2,h3,h4')->addClass('document')
      ->top()->find('.ListNum')->addClass('list-number-1')->removeClass('ListNum')
      ->top()->find('.TableHeading')->addClass('table-heading')->removeClass('TableHeading')
      ->top()->find('.ListResult')->addClass('list-result')->removeClass('ListResult')
      ->top()->find('.TaskOneStep')->addClass('list-bullet-1 task-one-step')->removeClass('TaskOneStep')
      ->top()->find('.LegalNotice')->addClass('legal-text')->removeClass('LegalNotice');
      
      $qp->top()->find('h6.ListResult');
      swap_tags($qp,'<p class="list-result">','</p>');
      
      $qp->top()->find('em.bold');
      swap_tags($qp,'<strong>','</strong>');
      
      $doc = extract_body($qp->top()->html());
    //strip any id tags out of the document (they are added by htmLawed and break Querypath)
      $doc = preg_replace('/(id|border)="\d+"/', '', $doc);		
      
      //strip out any "on page" references embedded inside links
      $doc = preg_replace('/>(\s+?|(&nbsp;)+?)on\spage.*?<\/a>\./','></a>. ', $doc);	//version at end of sentence (ends with a period)
      $doc = preg_replace('/>(\s+?|(&nbsp;)+?)on\spage.*?<\/a>/','></a> ', $doc);	//version in middle of sentence (ends with a space)
      $doc = preg_replace('/>(\s+?|(&nbsp;)+?)on\spage.*?<\/a><\/p/','></a></p', $doc);	//version at end of tag (ends with an open tag)

      break;
  }
}

/**
 * Remove hard-coded table attributes
 * 
 */
function doc_import_process_tables(&$doc, $author_tool){

  // Remove hard-coded table HTML tags: Border, cellspacing, cellpadding, width. Leave Colspan & Rowspan.
  $doc = preg_replace('/(border|cellspacing|cellpadding|width|valign)="\d+"/', '', $doc);

    // Strip all newlines/other EOL characters from everything
    $doc = str_replace(array("\n","\r","\t","\r\n", "&Acirc;","&Acirc;&nbsp;"),"",$doc);
    
    //Remove border attributes, then strip any image in the top-left cell of a note table (these are icons which will be replaced by CSS)
    $tables = htmlqp($doc)->find('table')->removeAttr('border');
    
    // Account for older templates that do not have two columns in note tables
    $tables->top()->find('table.note')->each(function($index, $item) {
	$table = qp($item);
	$columns = $table->branch()->find('tr:first-child td')->size();
	if ($columns == 1) {
	    $header = $table->branch()->find('tr:first-child td:first-child p:first-child')->addClass('table-header')->remove();
	    $table->prepend('<tr><td rowspan="2" class="inserted"></td> <td>' . $header->html() . '</td></tr>');
	    return $table;
	}
	else{
	    return $table->find('tr:first-child td:nth-child(2)>p:not(.note-header)')->addClass('note-header')->html();
	}
    });
    
    // Force first <td> (icon container) to be 58px wide, then strip embedded note <img>
    $tables->top()->find('table.note tr:first-child td:first-child')->css('width', '58px')->find('img')->remove();
  
    // transfer table-heading class from <p> elements up to parent <td> elements for styling purposes
    $tables->top()->find('table:not(.note) tbody td p.table-heading')->parent('td')->addClass('in-table-heading');
    
    // Some versions of the Word template mistakenly apply the 'admin tip' style to a regular note. This detects and fixes the discrepancy.

    $tables->top()->find('table.note')->each(function($index,$item) use ($author_tool){
	$table = qp($item);
	$classes = explode(' ',$table->attr('class'));
	
	if($author_tool == 'msword' && array_search('note-admin-tip',$classes)) {
	    $type = $table->branch()->find('td:nth-child(2)>p')->text();
	    if(stripos($type,'note:') !== FALSE){
		$table->removeClass('note-admin-tip')->addClass('note-note');
	    }
	}
    
    });
  
    // Some Word template versions do not style note tables at all, leaving them without identifying classes.
    // Check ALL tables that aren't notes to make sure they aren't supposed to be.
    
    if($author_tool == 'msword'){
	
	$tables->top()->find('table:not(.note)')->each(function($index, $item){
	    $table = qp($item);
	    
	    // Look in the text value of the 2nd table td (that's where the note label is), before a colon. Squeeze out extra spaces.
	    $text = str_replace(array('  ','   ','&nbsp;'),'',array_shift(explode(':',$table->branch()->find('td:nth-child(2)>p')->text())));
	    $types = array('note-note' => 'Note',
			   'note-caution' =>'Caution',
			   'note-warning' =>'Warning',
			   'note-admin-tip' =>'Admin Tip',
			   'note-power-tip' => 'Power Tip',
			   'note-settings' => 'Settings',
			   'note-timesaver' => 'Timesaver',
			   'note-troubleshooting' => 'Troubleshooting',
			   'note-user-tip' => 'User Tip',
			   'note-web-info' => 'Web Info');
	    
	    // array_search returns the key if matched, otherwise FALSE (e.g. "Note" returns key "note-note")
	    if ($key = array_search($text, $types)) {
		$table->attr('class','')->addClass('note ' . $key);	
	    }
	});
	
    }
    
    // Catch table headings mis-applied as table captions
    if($author_tool == 'mif2go') {
	$tables->top()->find('p.table-heading')->next('table')->prev('p.table-heading')->addClass('caption')->removeClass('table-heading');
    }
    
    // Make an educated guess at table column widths by calculating the average number of characters in each columns' cells.
    // Higher character weighting will result in wider columns.
    // If not overridden by HTML or weighting, assume 2-column tables are 33/66, 3-column tables are 33/33/33, and 4-columns are 25-25-25-25.
    $tables->top()->find('table:not(.note)')->each(function($index,$item){
	
	$table = qp($item);
	$head = $table->branch()->find('table>thead')->size();
	$body = $table->branch()->find('table>tbody')->size();
	
	if ($head != 0) {
	    $colspan = $table->branch()->find('thead > tr:first-child > td')->attr('colspan');
	    $label = $table->branch()->find('thead > tr:first-child > td:first-child')->text();
	    $columns = $colspan ? $colspan : $table->branch()->find('thead > tr:first-child > td')->size();
	}
    
	else {
	    $columns = $table->branch()->find('table > tr:first-child > td')->size();
	}
	// If <tbody> is present, add it to the QP selector for columns.
	if($body) $tbody = "tbody ";
	
	// Loop through columns, scoop up text of each <td> in the column and get the strlen of the content.
    	$column = array();

	for($i = 1; $i <= $columns; $i++){
	    $temp = $table->branch()->find("$tbody" . "td:nth-child($i)");
	    foreach ($temp as $cell) {
		$column[$i][] = strlen(trim(qp($cell)->text()));
	    }
	    $count = count($column[$i]);
	    
	    if ($count == 0) {
		$mean = -1;
	    }
	    else {
		$mean = array_sum($column[$i]) / $count;		
	    }
	    $average[$i] = $mean;
	}
	
	// Convert the raw results into a weighting ($weight = array of raw values (.00 - .01); $raw = array of % [$weight *100])
	foreach($average as $col) {
	    $value = round($col / array_sum($average),2);
	    $weight[] = $value;
	    $raw[] = $value * 100;
	}
	
	// Include the raw weighting distribution as a class. Currently just informational, but could be used for styling.
	$raw_weighting = 'raw-' . implode('-',$raw);
	
	// Start building class string. Begins with the column count.
	$class = "column-" . $columns;

	// Assign the remainder of the column widths based on character counts, using and commonly-used table layouts.
	// Tables with more than 6 columns are on their own.
	switch ($columns) {
	    case 1:
		break;
	    case 2:
		if($weight[0] < .2) {
		    $class .= "-20-80";
		}
		else if($weight[0] < .4) {
		    $class .= "-33-66";
		}
		else {
		    $class .= "-50-50";
		}
		break;
	    case 3:
		if ($weight[0] < .2 && $weight[1] < .2) {
		    $class .= "-20-20-60";
		}
		else if ($weight[0] < .2 && $weight[1] > .2 && $weight[3] > .2) {
		    $class .= "-20-40-40";
		}
		else if($weight[0] > .5 && $weight[1] < .2) {
		    $class .= "-70-15-15";
		}
		else {
		    $class.="-33-33-33";
		}
		break;
	    case 4:
		if($weight[0] > .5 && $weight[1] < .2 && $weight[2] < .2 && $weight[3] < .2) {
		    $class.="-55-15-15-15";
		}
		else if ($weight[0] <= .1 && $weight[1] <= .1 && $weight[2] <= .1) {
		    $class.="-10-10-10-70";
		}
		else if ($weight[3] > .4) {
		    $class.="-20-20-20-40";
		}
		break;
	    case 5:
		if($weight[0] > .5 && $weight[1] < .2 && $weight[2] < .2 && $weight[3] < .2 && $weight[4] < .2) {
		    $class.="-52-12-12-12-12";
		}
		    break;
	    case 6:
		if($weight[0] > .5 && $weight[1] < .2 && $weight[2] < .2 && $weight[3] < .2 && $weight[4] < .2 && $weight[5] < .2) {
		    $class.="-50-10-10-10-10-10";
		}
	    default:
		break;
	}
	switch (trim(strtolower($label))) {
	    case 'regulatory notices':
		$table_type = 'regulatory-notices';
		break;
	    
	    case 'china ccc emc statement':
	    case 'taiwan bsmi emc statement':
	    case 'japan vcci emc statement':
		$table_type = 'center-all';
		break;
	    
	    default:
		$table_type = 'no-merged-header';
		break;
	    
	}
	
	$table->addClass($class)->addClass($raw_weighting)->addClass($table_type);
	if(!$colspan) $table->addClass('fixed');

	return $table;
    });
    $doc = extract_body($tables->top()->HTML());
    unset($tables);
}

/**
 * List Parsing Engine
 * Processes <p class="list-[bullet,number]-x>" constructs into semantically correct HTML lists
 */
function doc_import_process_lists(&$doc, $author_tool){
    module_load_include('inc', 'doc_import', 'htmLawed.lib');
    
    // Clean up incoming HTML
    $config = array(
      'deny_attribute' => 'style',
      'comment' => 1,
      'cdata' => 1,
      //'tidy' => 1,
      'balance' => 0,
      'keep_bad' => 1,
    );
    $doc = htmLawed($doc, $config);
      
    // Remove hard-coded numbering in Word docs. Regex matches after <p class="*-number-*">, and removes the first <b>*</b>
    // or the first number, period, any number of "&nbsp", and an optional space.
    // examples:
    // <p class="list-number-1"><b>1.</b>...</p>      <--removes [<b>1.</b>]
    // <p class="list-number-3">aa.&nbsp;&nbsp;&nbsp; Start...</p>  <--removes [aa.&nbsp;&nbsp;&nbsp; ](including the trailing space)
      
    switch ($author_tool) {
	case 'msword':
	    $doc = str_replace('&#9679;<span>&nbsp;&nbsp;&nbsp; </span>', '', $doc);
	    $doc = str_replace('&Oslash;<span>&nbsp; </span>', '', $doc);
	    
	    $regex = '/(<p\s*class\s*=\s*\".*?[-a-zA-Z0-9_]+number[-a-zA-Z0-9_]+\".*?>)((<b>.*?<\/b>)|([0-9a-z]*\.?(&nbsp;)*\s))/siu';
	    preg_match_all($regex,$doc,$out);
	    for($i=0;$i < count($out[0]);$i++){
		$doc = str_replace($out[0][$i],$out[1][$i],$doc);
	    }
	
	    $regex = '/(<p\s*class\s*=\s*\"[-a-zA-Z0-9_]+?bullet[-a-zA-Z0-9_]+?\".*?>)(.+?(&nbsp;)+\s*)/siu';
	    preg_match_all($regex,$doc,$out);
	    for($i=0;$i < count($out[0]);$i++){
		$doc = str_replace($out[0][$i],$out[1][$i],$doc);
	    }
	    
	    $regex = '/&raquo;(&nbsp;)+/siu';
	    $doc = preg_replace($regex, '', $doc);

	    $bold_lists = htmlqp($doc,'p.bulleted-heading');
	    foreach($bold_lists as $bold_list){
		$content = $bold_list->contents()->html();
		if($position = strpos($content,'&emsp;') != 0){
		    $content = '<b>' . $content;
		    str_replace('&emsp;','</b>&emsp;',$content);
		}
	    }
	    
	    
	    break;
	case 'mif2go':
	    $doc = str_replace(array(
			'<span class="wingdings">&#10146; </span>',
			'<span class="wingdings">&diams; </span>',),
			'',
			$doc);
    
    }
    //dpm((array)htmLawed($doc, $config),'after regex');
    $lists = htmlqp($doc);

    //remove empty lines. @TODO: Break this into a helper function?
    $lists->find('p')->each(function($index,$item){
	//dpm(qp($item)->text(),'text');
	$line = qp($item);
	
	//ignore elements that contain images. They may have no text on purpose.
	if($line->branch()->children('img')->size() == 0) {

	    $text = trim($line->branch()->text());
	    if(!($text) || (strlen($text < 3) && htmlentities($text) == '&Acirc;&nbsp;')){
		$line->addClass('kill');
	    }
	}
	else {
	    // If they do contain images, make sure they shouldn't really be a 'graphic' style
	    $nodes = $line->branch()->children()->size();
	    $text = $line->branch()->text();
	    if($nodes == 1 && $line->hasClass('body-text') && !trim($text)) {
		$line->removeClass('body-text')->addClass('graphic');
	    }
	}
    });
    
    // If a preface immediately precedes an introduction AFTER another list, the first list will wrap the following <p.~preface> element.
    // This results in bad html (<ul><li></li><p></p></ul) and querypath madness. In this (rare) instance, convert the preface to body-text.
    $lists->top()->find('p.numbered-task-preface, p.bulleted-task-preface')->each(function($index,$item){
	if (qp($item)->next()->hasClass('numbered-task-introduction') || qp($item)->next()->hasClass('bulleted-task-introduction')) {
	    $line = qp($item);
	    $line->removeClass('numbered-task-preface')->removeClass('bulleted-task-preface')->addClass('body-text was-preface');
	    return $line;
	}
    });
    
    $lists->top()->find('.kill')->remove();

    // Mark root-level <p> tags with a 'base' class to make future identification a little easier
    $lists->top()->find('div>p.body-text')->addClass('base');
    $lists->top()->find('div>p.numbered-task-introduction, div>p.bulleted-task-introduction')->addClass('stop');
    
    // Drop zTableNum spans from Frame docs
    if($author_tool == 'mif2go'){
	$lists->top()->find('span.zTableNum')->contents()->unwrap();
    }
		  
    // flag existing task intros with a 'root' class to make them easier to find.
    $list_introductions = '
	p.bulleted-task-introduction,
	p.bulleted-task-preface,
	p.task-introduction,
	p.numbered-task-introduction,
	p.numbered-task-preface
	p.task-major-intro';
    
    $lists->top()->find($list_introductions)->addClass('root');

    // $break_list is a comma-separated list of CSS selectors that are NEVER inside a list (OL or UL).
    // These are used to mark the end of lists while building. 
    $break_list = '
	h1, h2, h3, h4, h5,
	p.body-heading,
	div>p.bulleted-task-introduction,
	div>p.numbered-task-introduction,
	div>p.task-introduction,
	div>p.bulleted-task-preface,
	div>p.numbered-task-preface,
	div>p.task-major-intro,
	p.body-text.base,
	div>p.note-heading,
	div>table.note,
	.stop';

    $level=3;
    while($level > 0){
	
	// Add additional level- and structure-specific CSS selectors that signal the end of a list. These strings are appended to $break_list.
	switch ($level){
	    case 1: // Level 1 lists (e.g., ul)
		$ul_stop = ', .list-bullet-1, p.numbered-task-introduction, p.numbered-task-preface, p.bulleted-task-introduction, p.bulleted-task-preface, p.task-introduction';
		$ol_stop = ', .list-number-1, p.numbered-task-introduction, p.numbered-task-preface, p.bulleted-task-introduction, p.bulleted-task-preface, p.task-introduction';
		break;
	    
	    case 2: // Level 2 lists (e.g., ul ul)
		$ul_stop = ', .list-bullet-1, .list-bullet-2, .list-continue-1, .list-number-1, p.root';
		$ol_stop = ', .list-number-1, .list-number-2, .list-continue-1, .list-bullet-1, p.root';
		break;
	    
	    case 3: // Level 3 lists (e.g., ul ul ul)
		$ul_stop = ', .list-bullet-1, .list-bullet-2, .list-bullet-3, .list-number-2, .list-number-1, .list-continue-1, .list-continue-2, p.root';
		$ol_stop = ', .list-number-1, .list-number-2, .list-number-3, .list-bullet-2, .list-bullet-1, .list-continue-1, .list-continue-2, p.root';
		break;
	}		

	//Find list-bullet elements and loop through them, wrapping each in <li> tags.
	$lists->top()->find('p.list-bullet-'.$level);
	$step = 1;
	  
	foreach($lists as $li){
	    $pass_class = 'step-' . $step;
	    $li->nextUntil($break_list . $ul_stop)->andSelf()->wrapAll('<li class="bullet-'.$level.' level-'.$level.' ' . $pass_class . '"/>');
	    $target = $li->branch()->top()->find('li'.'.'.$pass_class);
	    $li->top()->find('.'.$pass_class)->children();
	    if($li->size() > 1){
		$li->last()->prependTo($target)->remove();
	    }
	    $li->top()->find('.'.$pass_class)->removeClass($pass_class)->removeClass("level-$level")->addClass("level-$level-processed");
	    $step++;
	}
	//Find list-number elements and loop through them	
	$lists->top()->find('p.list-number-'.$level);
	$step = 1;
	foreach($lists as $li){
	    $pass_class = 'step-' . $step;
	    $li->nextUntil($break_list .  $ol_stop)->andSelf()->wrapAll('<li class="number-'.$level.' level-'.$level.' ' . $pass_class . '"/>');
	    $target = $li->branch()->top()->find('li'.'.'.$pass_class);
	    $li->top()->find('.'.$pass_class)->children();
	    if($li->size() > 1){
		$li->last()->prependTo($target)->remove();
	    }
	    $li->top()->find('.'.$pass_class)->removeClass($pass_class)->removeClass("level-$level")->addClass("level-$level-processed");
	    $step++;
	}
	$level--;
    }
  
    // Fix mismatched headings/bullets. Assume the bullet is correct.
    $lists->top()->find('li[class^="bullet"]')->prev('p.numbered-task-introduction, p.numbered-task-preface')->removeClass('numbered-task-introduction, numbered-task-preface')->addClass('bulleted-task-introduction');
    $lists->top()->find('li[class^="number"]')->prev('p.bulleted-task-introduction, p.bulleted-task-preface, p.task-introduction')->removeClass('bulleted-task-introduction, bulleted-task-preface, task-introduction')->addClass('numbered-task-introduction');
   
    // Find orphaned lists (lists with no introduction) and insert temporary introductions before each one.
    // This allows them to be processed normally into HTML lists.
  
    $topic = $lists->top()->find('div')->html();
    $lists->top()->find('li[class^="bullet"]')->each(function($index, $item){
	$qp = qp($item);
	$check = $qp->branch()->prev();

	$topic = $qp->branch()->closest('div')->attr('id');
	
	while($check->hasClass('note')) {
	    $check->prev();
	}
	
	$classes = array_shift(explode(' ',$qp->attr('class')));
	$this_level = substr($classes,strlen($classes)-1);
	$last_level = substr(array_shift(explode(' ',$check->attr('class'))),-1);
	
	if($check->size() == 0 || ($check->hasClass('body-text') || $check->hasClass('table-text') || $check->hasClass('table-body') || $check->hasClass('temp') || $last_level < $this_level || $check->hasClass('list-continue') || $check->hasClass('document') || $check->hasClass('placeholder')) &&
	    !$check->hasClass('bulleted-task-introduction') && !$check->attr('class*=["bulleted-task-"]')){
	   if($check->size() != 0 && $check->branch()->parent('div')->hasClass('document-content')){$root = 'root';} else $root = '';
	   $tempstring = '<p class="bulleted-task-introduction '.$root.' temp">***Inserted [BULLET] Intro '.$index.'***</p>';
	   $qp->before($tempstring);
	}
    });
    
    $lists->top()->find('li[class^="number"]')->each(function($index, $item){
	$qp = qp($item);
	$check = $qp->branch()->prev();

	$classes = array_shift(explode(' ',$qp->attr('class')));
	$this_level = substr($classes,strlen($classes)-1);
	$last_level = substr(array_shift(explode(' ',$check->attr('class'))),-1);
		
	if($check->size() == 0 || ($check->hasClass('body-text') || $check->hasClass('table-text') || $check->hasClass('table-body') || $check->hasClass('temp') || $last_level < $this_level || $check->hasClass('list-continue') || $check->hasClass('document') || $check->hasClass('placeholder')) &&
	    !$check->hasClass('numbered-task-introduction') && !$check->attr('class*=["numbered-task-"]')){
	   if($check->size != 0 && $check->branch()->parent('div')->hasClass('document-content')){$root = 'root';} else $root = '';
	   $tempstring = '<p class="numbered-task-introduction ' . $root . ' temp">+++Inserted [NUMBERED] Intro '.$index.'+++</p>';
	   $qp->before($tempstring);
	}
    });
  
    //Wrap list items in ul/ol tags. Works in reverse order, wrapping level 3 lists, then level 2, then level 1.		
  
    $stop = ', div>p.root';	
    $level = 3;
    while($level > 0){
    
    switch ($level){
	case 1:
		
	    //bullets
	    $lists->top()->find('li.bullet-1')->prev('p.bulleted-task-introduction, p.task-introduction, p.bulleted-task-preface');
	    foreach ($lists as $list){
	      $list->nextUntil($break_list . $stop)->wrapAll('<ul class="bulleted-list-level-1" />');
	    }
	    
	    //remove nested at same level. @todo: change above code so this doesn't happen
	    $lists->top()->find('ul.bulleted-list-level-1 ul.bulleted-list-level-1')->contents()->unwrap(); 
    
	    //numbers
	    $lists->top()->find('li.number-1')->prev('p.numbered-task-introduction, p.numbered-task-preface, p.task-major-intro, table.note');	
	    foreach($lists as $list){

		$list->nextUntil($break_list . $stop)->wrapAll('<ol class="numbered-list-level-1 the-start" type="1" start="1"/>');
	    }
	    
	    //remove nested at same level. @todo: change above code so this doesn't happen
	    $lists->top()->find('ol.numbered-list-level-1 ol.numbered-list-level-1')->contents()->unwrap(); 
	    break;
      
	case 2:
	    //bullets
	    $lists->top()->find('li.bullet-2')->prev('p.bulleted-task-introduction, p.bulleted-task-preface, p.task-introduction')->next();
	    foreach ($lists as $list){
		$items = $list->branch()->nextUntil($break_list . ', li.bullet-1, li.number-1')->size();
		if($items == 0){
		    $list->wrap('<ul class="bulleted-list-level-2 singleton" />');						
		}
		else{
		    $list->prev()->nextUntil($break_list . ', li.bullet-1, li.number-1, p.list-continue-1')->wrapAll('<ul class="bulleted-list-level-2 items-'.($items+1).'" />');
		}
	    }
    
	    //remove nested at same level. @todo: change above code so this doesn't happen
	    $lists->top()->find('ul.bulleted-list-level-2 ul.bulleted-list-level-2')->contents()->unwrap();
	    
	    //numbers
	    $lists->top()->find('li.number-2')->prev('p.numbered-task-introduction, p.numbered-task-preface, p.task-major-intro')->next();
    
	    foreach ($lists as $list){
		$items = $list->branch()->nextUntil($break_list . ', li.bullet-1, li.number-1')->size();
		if($items == 0){
		    $list->wrap('<ol class="numbered-list-level-2 singleton" type="a"/>');
		}
		else{
		    $list->prev()->nextUntil($break_list . ', li.number-1, li.bullet-1, p.list-continue-1')->wrapAll('<ol class="numbered-list-level-2 items-'.($items+1).'" type="a" />');
		}
	    }
	     
	    //remove nested at same level. @todo: change above code so this doesn't happen
	    $lists->top()->find('ol.numbered-list-level-2 ol.numbered-list-level-2')->contents()->unwrap();
	    break;
      
	case 3:
	    //bullets
	    $lists->top()->find('li.bullet-3');
	    foreach ($lists as $list){
		$items = $list->branch()->nextUntil($break_list . ', li.bullet-1, li.bullet-2, li.number-1, li.number-2')->size();
		if($items == 0){
		    $list->wrap('<ul class="bulleted-list-level-3 singleton" />');						
		}
		else{
		    $list->prev()->nextUntil($break_list . ', li.bullet-1, li.bullet-2, li.number-1, li.number-2, p.list-continue-1, p.list-continue-2')->wrapAll('<ul class="bulleted-list-level-3 items-'.($items+1).'" />');
		}
	    }
	    
	    //remove nested at same level. @todo: change above query so this doesn't happen
	    $lists->top()->find('ul.bulleted-list-level-3 ul.bulleted-list-level-3')->contents()->unwrap();
	    
	    //numbers
	    $lists->top()->find('li.number-3');
	    foreach ($lists as $list){
		$items = $list->branch()->nextUntil($break_list . ', li.bullet-1, li.bullet-2, li.number-1, li.number-2')->size();
    
		if($items == 0){
		    $list->wrap('<ol class="numbered-list-level-3 singleton" type="1"/>');
		}
		else{
		    $list->prev()->nextUntil($break_list . ', li.bullet-1, li.bullet-2, li.number-1, li.number-2, p.list-continue-1, p.list-continue-2')->wrapAll('<ol class="numbered-list-level-3 items-'.($items+1).'" type="1"/>');
		}
	    }
	    
	    //remove nested at same level. @todo: change above query so this doesn't happen
	    $lists->top()->find('ol.numbered-list-level-3 ol.numbered-list-level-3')->contents()->unwrap(); 
    
	    break;	
	}		
    $level--;
  }
  
  // Add typecasting for ordered lists based on nesting level
  $lists->top()->find('div>ol, div>ul ol, div>ul ul ol')->attr('type','1')
    ->top()->find('div>ol ol, div>ul ol ol, div>ul ul ol ol')->attr('type','a')
    ->top()->find('div>ol ol ol, div>ul ol ol ol, div> ul ul ol ol ol')->attr('type','1');
    
  //remove temp intros
  $lists->top()->find('p.temp')->remove();
  //$lists->top()->find('p.base')->removeClass('base');
  
  //Take care of one step tasks
  $lists->top()->find('p.task-one-step')->parent('li.bullet-1')->addClass('task-one-step');
  
  $doc = extract_body($lists->top()->HTML());
  
}


/**
 * final image processing:
 * Updates image URLs to final storage location. Requires $content (string), and $node (object).
 */

function doc_import_final_images(&$content, &$node) {

    $found_images = htmlqp($content, 'img');

    //base_uri is the uri for the html_master field without the filename (i.e. public://path/to/file/example.html -> $public://path/to/file)
    $base_prefix = 'public://longform/imports/expanded/' . $node->nid .'/';
    //dpm($_SERVER,'server array');
    global $base_url;
    global $base_path;
    
    foreach ($found_images as $img){  	
						$href = $img->attr('src');
						//dpm($href,'initial image source');
						//update the URL, stripping out the site's base_url (http://mysite.com). Creates a relative src path.
											
						if($_SERVER['HTTP_HOST'] == 'localhost'){
								$new_href = $base_url . '/sites/default/files/longform/imports/expanded/' . $node->nid . '/' . $href;
								//dpm($new_href,'new href');
						}
						else{
								$new_href = file_create_url($base_prefix . $href);
						}
						
						// NOTE: Lazyloading has been moved to an outgoing hook. It is no longer done here.
						
						// Update image source.
						$img->attr('src', $new_href);
    }
  
    //Update contents
    $content = extract_body($found_images->top()->html());
}

/**
 * Removes legacy print strings like "on page xyz"
 */
function doc_import_clean_printisms(&$doc){

  //Remove "on page xyz" inside links
  $regex = '/(<a.*?class="HeadingPage"><span class="xref">.*?<\/span>).*?(<\/a>)/uim';
  preg_replace($regex, '', $doc);
  
  //Remove "on page xyz"
  $regex = '/on page.*?<span class="zPageNum">.*?<\/span>/uim';
  preg_replace($regex,'',$doc);
  
  $regex = '/Appendix (0-9A-z):\s?/uim';
  preg_replace($regex,'',$doc);

  $regex = '/Chapter\s(0-9A-z):\s?/uim';
  preg_replace($regex,'',$doc);
}

/**
* Check links for problems that can be fixed early in the import process, before content is reassembled
* @TODO: This curently does nothing. Write it!
*/
function doc_import_links_preprocess(&$section){
    
    //check to see if there is an explicit .htm reference left over from mif2go compilation. If so, remove it.
    //This check is very restrictive to avoid breaking non-xref links. Xrefs that make it through will be trimmed later.
    //$section = preg_replace('/"[a-z]{2}[0-9]+?\.html*?#/','"#',$section);

}

/**
 * Strip out useless or abandoned elements (such as TOC links)
 * 
 */
function doc_import_remove_junk(&$content){

    //remove all <br/> tags
    
    // This is disabled: some line breaks are intentional (e.g. in code snippets).
    // @TODO: Refine needs and replace with smarter solution
    
    //$content = str_replace(array('<br>','<br/>','<br />','&#13;'),'',$content);
    
    // Remove EOL characters
    $content = str_replace(array('\r\n','^M', PHP_EOL),' ',$content);
    
    //remove empty anchors
    $content = str_replace(array('<p class="FigAnchor">&nbsp;</p>', '<p class="body-text">&nbsp;</p>'), '', $content);

    // Remove wingdings/copyright symbol
    //$content = str_replace('<span class="wingdings">&copy; </span>', '', $content);
    
    //link elements containing any of the elements in $remove will be destroyed
    $remove = array('_Toc',
								    );
    $qp = htmlqp($content);	
    foreach($remove as $rm){
	$find = "a[name^=\"$rm\"]";
	$qp->find($find)->each(function($index, $item){
	    $link = qp($item);
		if (qp($item)->contents()->size() == 0){
		    qp($item)->remove();
		}
		else{
		    qp($item)->contents()->unwrap();
		}
	});
    }
    
    $qp->top()->find('br')->remove()
	->top()->find('h4, h5')->each(function($index,$item){
	    $head = qp($item);
	    if (htmlentities($head->text()) == '&nbsp' || htmlentities($head->text()) == '&Acirc;&nbsp;') {
		$head->addClass('kill');
	    }
	})
	->top()->find('h4.kill, h5.kill')->remove();

    // Drop body after table lines
    $qp->top()->find('.body-after-table')->contents()->unwrap();
    
    $content = trim(extract_body($qp->top()->html()));
    
}

/**
 * Clearinghouse for special symbol characters that aren't HTML approved.
 *
 * Add elements to the $symbols array to have them str_replaced()
 * in the form ($find => $replace)
 *
 * @param $section
 * 	The raw HTML to be processed. Returned by reference.
 */
function doc_import_process_symbols(&$section) {

  $symbols = array(
    '#61692;' => '#10003;',	// Check mark
    '#61691;' => '#10007;',	// Ballot "x"
    );
  
  foreach($symbols as $find => $replace) {
    $section = str_replace($find, $replace, $section);
  }
}

/**
 * Operations reserved for the end of the conversion process.
 *
 * These are performed after everything else is done, but before saving conversions.
 *
 * @param $section
 * 	The HTML content. Passed by reference.
 *
 * @param $topic
 * 	The loaded topic object containing the HTML.
 * 	
 */
function doc_import_last_train_processes(&$section, $topic){
    //add the topic ID and original heading level to the container DIV
    $section = extract_body(htmlqp($section,'div')->attr('topic',$topic->nid)->attr('level',$topic->field_topic_source_heading[LANGUAGE_NONE][0]['value'])->top()->html());

    // Get header links
    $head_links = array();
    $head_links = json_decode($topic->field_topic_anchors[LANGUAGE_NONE][0]['value'], TRUE);
    
    // Prepare links, remove old hrefs from links
    $section = extract_body(htmlqp($section,'a')->each(function($index, $item) use ($section){

	// Check for wrapped anchors and unwrap them: copy link text outside of link.
	$link = qp($item);
	$name = $link->attr('name');
	if($name) {
	    $contents = $link->contents();
	    if($contents->size() != 0){
		qp($item)->text('')->parent()->prepend($link);
	    }	
	}
	    
        //if an .htm file snuck through preprocessing, snip it here
	$link = qp($item);
	$link_href = $link->attr('href');

	if($link_href) {
	    
	    // Add code to pop external links into a new tab/window and return
	    if (stripos($link_href, 'http') !== FALSE) {
		return $link->attr('target','_blank')->addClass('link-external');
	    }
	    // Add a class to mailto: links and return
	    else if (stripos($link_href,'mailto') !== FALSE) {
		return $link->addClass('link-mailto');
	    }
	    
	    // Drop file prefixes
	    if(strpos($link_href,'#') !== FALSE){
		$trimmed = array_pop(preg_split('/#/',$link_href));
		$link_href = '#' . $trimmed;
	    }
	    else{
		$trimmed = $link_href;
	    }
	    
	    // out-of-section xrefs are tagged, and branded with a "link-error" class which will be removed when the link is reconnected.
	    if(stripos($section, "name=\"$trimmed\"") === FALSE) {
		$link->attr('href', "REPLACE-WITH-NID" . $link_href)->attr('ref', $trimmed)->addClass('xref other-section')->addClass('link-error');
	    }
	    else{
		
		// Internal links just get a tag letting us know they've been processed.
		$link->addClass('xref same-section')->attr('ref', $trimmed);
	    }	    
	    
	    
	    return $link;//->attr('href', $link_href);
	}
	if($link_name) {
	    return $link;
	}
    })->top()->html());
    
    // Remove placeholders
    $section = extract_body(htmlqp($section, 'p.placeholder')->remove()->top()->html());
    
    //remove any remaining html bracketing inserted by querypath
    while(strpos($section, '</body>') !== FALSE){
      extract_body($section);
    }
    
    // Pretty up HTML.
    // NOTE: Do all QP work BEFORE this operation, and all text maniplation (str_replace, etc) AFTER it.
    // HTMLawed will modify/strip empty spaces and other modifications that may undo Technet cleanup work.
    
    module_load_include('inc', 'doc_import', 'htmLawed.lib');
    $config = array(
	'deny_attribute' => 0,
	'tidy' => 1,
	'balance' => 0,
	'keep_bad' => 1,
    );
    
    // Allow these custom attributes through the filter.
    // format is "element=attr, attr, attr..." separate multiple elements with semicolons.
    // Example: 'div=custom1, custom2, custom3; span=custom4; table=custom5, custom6'
    
    $spec = 'div=level, topic, heading, id; img=data-src';

    $section = htmLawed($section, $config, $spec);

    //cleanup character conversion
    $section = str_replace('&amp;%', '&%', $section);
    $section = str_replace('quo;;', 'quo;', $section); 	//Hack to fix double-decoded &rsquo and &lsquo characters
  
    // Fix lost spaces in/around bold tags
    $section = str_replace("</b><b>"," ",$section);
    $section = str_replace("</b>",'</b> ', $section);
    $section = str_replace('  ',' ',$section);
    return $section;
}

/**
 * Returns an array of classes, with 'from' and 'to' endpoints
 */
function _doc_import_class_conversion_table(){
  
  // The master conversion table for Technet. This will eventually be moved to the database and a UI provided for adding/editing data.
  
  //define conversion table array
  $classes = array();
  $tags = array();
  
  //headings: These are heading-swapped if Frame docs (p.head1 => H1)
  $tags['normal']['h1'] = array('classes'=>array('head1','heading1'), 'tags' => 'h1');
  $tags['normal']['h2'] = array('classes'=>array('head2','heading2'), 'tags' => 'h2');
  $tags['normal']['h3'] = array('classes'=>array('head3','heading3'), 'tags' => 'h3');
  $tags['normal']['h4'] = array('classes'=>array('head4','heading4'), 'tags' => 'h4');
  $tags['normal']['h5'] = array('classes'=>array('head5','heading5'), 'tags' => 'h5');

  //headings offset by part numbers: These are the same as the normal set, but offset for H6 = Part (essentially H0)
  $tags['hpart']['h1'] = array('classes'=>array('head1','heading1'), 'tags' => 'h6');
  $tags['hpart']['h2'] = array('classes'=>array('head2','heading2'), 'tags' => 'h1');
  $tags['hpart']['h3'] = array('classes'=>array('head3','heading3'), 'tags' => 'h2');
  $tags['hpart']['h4'] = array('classes'=>array('head4','heading4'), 'tags' => 'h3');
  $tags['hpart']['h5'] = array('classes'=>array('head5','heading5'), 'tags' => 'h4');
 
 // Classes: these are straight conversions using str_ireplace().
 // Technet classes are on the left, source classes on the right. All instances of source classes for a
 // given technet class will be merged (for example, "Body", "BodyText", and "MsoBodyText" will all become "body-text").
 
  $classes['body-text'] = 			array('Body',
						      'BodyText',
						      'MsoBodyText',
						      'MsoNormal',
						      'Default');
  $classes['body-text keep-next'] =		array ('BodyTextKeepNext');
  $classes['body-after-table'] = 		array('BodyAfterTable'); 
  $classes['caption no-collapse'] =		array('CaptionNoCollapse',
						      'NoCollapseCaption',
						      'CaptionNoCollapseTable');
  $classes['caption'] = 			array('MsoCaption',
						      'CaptionTable',
						      'CaptionFigure',
						      'CaptionTableIndent',
						      'Caption2');
  $classes['list-bullet-1'] = 			array('MsoListBullet',
						      'ListBullet',
						      'Bullet',
						      'TableBullet',
						      'TableListBullet');
  $classes['list-bullet-1 keep-next'] = 	array('ListBulletKeepNext');
  $classes['list-bullet-2'] = 			array('MsoListBullet2',
						      'ListBullet2',
						      'Bullet1',
						      'TableBullet1',
						      'TableListBullet2');
  $classes['list-bullet-3'] = 			array('MsoListBullet3',
						      'ListBullet3',
						      'Bullet2',
						      'TableBullet2',
						      'TableListBullet3');
  $classes['list-bullet-1 task-one-step'] = 	array('TaskOneStep');
  
  $classes['bulleted-heading'] = 		array('BulletedHeading');
  $classes['body-heading'] = 			array('BodyHeading');
  $classes['task-major-intro'] = 		array('TaskMajorIntro');
  $classes['task-major-list'] =			array('TaskMajorList');
  
  $classes['bulleted-task-introduction'] = 	array('BulletedTaskIntroduction');
  $classes['numbered-task-introduction no-collapse'] = 	array('NoCollapseTask');
  $classes['numbered-task-introduction'] = 	array('NumberedTaskIntroduction',
						      'ListTaskIntroduction',
						      'TaskIntroduction');
  
  $classes['no-translate'] =		 	array('No_translate',
						      'no_translate');
  
  $classes['list-number-1'] = 			array('ListNum',
						      'MsoListNumber',
						      'ListNumber',);
  $classes['list-number-1 keep-next'] = 	array('ListNumberKeepNext');
  
  $classes['list-number-2'] = 			array('MsoListNumber2',
						      'ListAlpha');
  $classes['list-number-2 keep-next'] =		array('ListAlphaKeepNext');
  

  $classes['list-number-3'] = 			array('MsoListNumber3',
						      'ListNum3',
						      'ListAlpha3');
  $classes['list-number-3 keep-next'] =		array('ListNum32KeepNext',
						      'ListAlpha3KeepNext');
  
  $classes['list-continue-1'] = 		array('MsoListContinue',
						      'ListContinue');
  $classes['list-continue keep-next'] =		array('ListContinueKeepNext');
  
  $classes['list-continue-2'] = 		array('MsoListContinue2',
						      'ListContinue2');
  $classes['list-continue-2 keep-next'] =	array('ListContinue2KeepNext');
  
  $classes['list-continue-3'] = 		array('MsoListContinue3',
						      'ListContinue3');
  $classes['list-continue-3 keep-next'] =	array('ListContinue3KeepNext');
 
  $classes['graphic'] = 			array('Graphic',
						      'GraphicIndented',
						      'GraphicIndented2');
  
  $classes['graphic no-thumbnail'] = 		array('GraphicNoThumbnail');
  $classes['graphic thumbnail'] = 		array('GraphicThumbnail');
  $classes['graphic-callout'] = 		array('GraphicsCallout');
  $classes['hyperlink'] = 			array('Hyperlinks');
  $classes['hyperlink-underline'] = 		array('HyperlinkUnderline');
  
  $classes['legal-text'] = 			array('LegalText',
						      'LegalNotice');
  
  $classes['legal-heading'] =			array('LegalHeadings');
  $classes['footnote-text'] = 			array('MsoFootnoteText');
  
  $classes['code'] = 				array('Code',
						      'CodeWide',
						      'Courier',
						      'CourierTable',
						      'CourierChar');

  $classes['code code-inline'] = 		array('CodeInline');
  
  $classes['rn-issue'] =			array('RNIssue');
  $classes['rn-workaround'] =			array('RNWorkaround');
  $classes['keep-with-next'] = 			array('KeepNext');
  
  //tables
  $classes['table-invisible'] = 		array('TBTable',
						      'InvisibleTable',
						      'Borderless');
  
  $classes['table-standard'] = 			array('TBShadeTable',
						      'TBShadeTable11',
						      'TBShadeTable21',
						      'TBBandedTable',
						      'NoColumnSeparators');
  
  $classes['table-simple'] = 			array('MsoTableGrid');
  $classes['table-parameter'] = 		array('ParameterTable');
  $classes['table-parameter-reference'] = 	array('ParameterReferenceTable');
  $classes['table-feature-summary'] = 		array('FeatureSummaryTable');
  $classes['table-sidebar'] = 			array('Sidebar');
  $classes['sidebar-heading'] = 		array('SidebarHeading');
  $classes['table-body'] = 			array('TableBody',
						      'TableText',
						      'TableNum');
  $classes['table-note-text'] = 		array('TableNoteText');
  $classes['table-courier'] = 			array('TableCourier');
  $classes['table-hyperlink'] = 		array('TableHyperlink');
 
  //$classes['courier'] = 			array('Courier',);
  //$classes['courier-table'] = 			array('CourierTable',);
    
  // note tables

  $classes['note'] =			array('TNCTable',
					      'TNCplainHelp');
  
  $classes['note note-admin-tip'] = 	array('AdminTip',
					      'AdminTip1',
					      'AdminTip2');
  
  $classes['note note-caution'] = 	array('Caution',
					      'Caution1',);
  
  $classes['note note-note'] = 		array('RegularNote',
					      'RegularNote1',);
  
  $classes['note note-power-tip'] = 	array('PowerTip',
					      'UserTip2');
  
  $classes['note note-settings'] = 	array('Setting',
					      'Setting1',);
  
  $classes['note note-timesaver'] = 	array('Timesaver',
					      'Timesaver1',);
  
  $classes['note note-user-tip'] = 	array('UserTip',
					      'UserTip1',);
  
  
  $classes['note note-troubleshooting'] = array('Troubleshooting',
						'Troubleshooting1',);
  
  $classes['note note-warning'] = 	array('Warning',
					      'Warning1',);
  
  $classes['note note-web-info'] = 	array('WebInfo',
					      'WebInfo1',);    

    $classes['note-heading'] = 		array('NoteHead',
					      'NoteHeading');

  $dom['note-note'] = array(
			    'find' => 'IconNote',
			    'operator' => 'closest',
			    'element' => 'table',
			    'class' => 'note note-note',
			  );
  $dom['note-caution'] = array(
			    'find' => 'IconCaution',
			    'operator' => 'closest',
			    'element' => 'table',
			    'class' => 'note note-caution',
			  );
  $dom['note-warning'] = array(
			    'find' => 'IconWarning',
			    'operator' => 'closest',
			    'element' => 'table',
			    'class' => 'note note-warning',
			  );
  
  //classes that should be removed wholesale are listed here. NOTE: The entire DOM element will be deleted.
  $remove = array('BodyAfterTable');
  
  ksort($tags);
  ksort($classes);
  ksort($dom);
  
  return array('tags' => $tags, 'classes' => $classes, 'dom' => $dom, 'remove' => $remove);

}

/**
* Helper function that wraps conversion table class names in quotation marks
*/
function _doc_import_bracket(&$term){

    foreach($term as $key=>&$value){
	//$value = '"' . $value . '"';
	foreach ($value as &$class){
	   $class = '"' . $class . '"';
	}
    }
}

/**
 * Iterates through all topics in the controller/uses_topic relation and reconstructs links.
 * Returns TRUE if no errors were found, and a descriptive error array if any links could not be reconnected.
 * 
 */

 function doc_import_rebuild_topic_links($controller, &$context){
    module_load_include ('inc', 'doc_manage', 'doc_manage.topics');
    module_load_include ('inc', 'doc_import');
    
    if (empty($context['sandbox'])) {
	$context['sandbox'] = array();
	$context['sandbox']['progress'] = 0;
	$context['sandbox']['current_topic'] = 0;

	// Save topic count for the termination message.
	$context['sandbox']['max'] = 1;
    }
    
    // First, get the array of head & body links from topic nodes
    $anchors = _doc_manage_get_topic_fields ($controller, "field_topic_anchors");
    foreach ($anchors as $key => $anchor) {
	$anchors_split['head'][$key] = $anchor['head'];
	$anchors_split['body'][$key] = $anchor['body'];
    }

    //dpm($anchors_split['head'],'anchors split head');
    //dpm($anchors_split['body'],'anchors split body');

    $nodes = _doc_manage_get_topic_nids($controller);
    // Sweep nodes and try to match xrefs
    $errors = array();
    global $base_path;
    foreach($nodes as $node) {
	$link_topic = node_load($node);
        $nid = $link_topic->nid;
	$title = $link_topic->title;
	$body = $link_topic->field_topic_processed_html[LANGUAGE_NONE][0]['value'];
	
	$body = extract_body(htmlqp($body, 'a.xref.other-section')->each(function($index,$item) use ($anchors_split, &$errors, $nid, $title, $base_path) {
	    $link = qp($item);
	    $href = $link->attr('href');
	    $name = array_pop(explode('#',$href));
	    
	    // Check head links -- these go in with no #anchor. Add original anchor as "Ref" attribute.
	    $in = recursive_array_search($name,$anchors_split['head']);

	    if ($in !== FALSE) {
		$link->attr('href', $base_path . 'node/' .$in)->removeClass('link-error')->addClass('matched link-topic head')->attr('ref', $name)->attr('topic', $in);
		return $link;
	    }
	    
	    // Check body links -- these go in with #anchor. Add original anchor as "ref" attribute as well.
	    $in = recursive_array_search($name,$anchors_split['body']);
	    if ($in !== FALSE) {
		$link->attr('href', $base_path . 'node/' . $in . '#' . $name)->removeClass('link-error')->addClass('matched link-topic body')->attr('ref', $name)->attr('topic', $in);
		return $link;
	    }
	    
	    // href was not found in the $anchors array, so add it to the error array for reporting
	    $errors[$href] = array('topic_nid' => $nid, 'topic_title' => $title, 'link_text' => $link->text(), 'link_href' => $href);
		
	})->addClass('topic-link-processed')->top()->html());
	
	// Strip off any QP cruft before the initial <div class="document-content' string and save the topic.
	$link_topic->field_topic_processed_html[LANGUAGE_NONE][0]['value'] = substr($body, strpos($body, '<div class="document-content'));
	node_save($link_topic);
    }

    drupal_set_message('Cross-reference links rebuilt.','status');
    
    $context['results']['endpoints'] = $relation_endpoints;
  
    // Update our progress information.
    $context['sandbox']['progress']++;
    $context['sandbox']['current_topic']++;
    $context['message'] = 'Processing links...';
  
    // Inform the batch engine that we are not finished,
    // and provide an estimation of the completion level we reached.
    if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
      $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
    }

    $sandbox['results']['link-errors'] = $errors;
 }
 
/**
 * Convert code styles into bootstrap code styles
 *
 * Converts inline code snippets into <code> elements; block-level code <p> tags are aggregated and wrapped in <pre> tags
 *
 * @param $topic
 * 	An HTML string to style. Returned by reference.
 * 	
 */
 function doc_import_style_code(&$topic) {
   $qp = htmlqp($topic);
   
   // Find embedded code snippets in list elements, and transfer the code style to the parent (li) element instead
   //$list_items = $qp->find('p[class^="list-"] > span.code')->each(function($index,$item) {
   //   $span = qp($item);
   //   $span->contents()->unwrap()->parent('p')->removeClass('code')->addClass('code');
   //})->size();
   //
   // Drop spans with no class (these break 'nextUntil')
   $qp->top()->find('p[class^="list-"] > span')->each(function($index,$item){
      $span = qp($item);
      if($span->attr('class') == '') {
	$span->contents()->unwrap();
      }
    });
   
   
   // Inline code (span.code, except those contained by list items, which were already processed).
   $qp->top()->find('span.code')->each(function($index,$item){
    $contents = qp($item)->contents();
    return qp($item)->replaceWith('<code class="code">' . $contents->html() . '</code>');
   });
   
//   // Contiguous code blocks (p tags with "code" class will be aggregated and wrapped with <pre> tags).
//   $qp->top()->find('p.code')->each(function($index, $item) {
//      $line = qp($item);
//      $prev = $line->branch()->prev();
//      if ($line->attr('[class*="list-"]') != '') {
//	$in_list = TRUE;
//      }
//	dpm($line->branch()->html(),'line');
//      
//      if($prev->hasClass('placeholder') || ($prev->size() == 0 && !$line->hasClass('code-block-processed'))) {
//	$group = $line->add($line->nextUntil(':not(.code)')->addClass('code-block-processed'));
//      }
//      else if (!$prev->hasClass('code') && !$line->hasClass('code-block-processed')) {
//
//	$group = $line->prev()->nextUntil(':not(.code)')->addClass('code-block-processed');
//      }
//      else  {
//	return $line;
//      }
//      
//      $class = $in_list ? "code code-block list-continue-1" : "code code-block";
//      
//      return $line->wrapAll('<pre class="' . $class . '"/>');
//    });
		    
   
   $topic = extract_body($qp->top()->html());
    
}
 
/**
 * Returns an array of anchors present in the content's body.
 * @param $content: string of HTML.
 * 
 */
function _doc_import_get_content_anchors($content) {
    $anchors = array();
    $qp = htmlqp($content, 'a')->each(function($index, $item) use (&$anchors) {
	$anchor = qp($item)->attr('name');
	if ($anchor) {
	    $anchors[] = $anchor;	    
	}
    });
    return $anchors;
}


/**
 * Repair characters corrupted by encoding mismatches.
 * 
 * Add elements to the $errors array to have them str_replaced()
 * in the form ($find => $replace)
 *
 * @param $section
 * 	The raw HTML to be processed. Returned by reference.
 */
function doc_import_correct_encoding_errors(&$section) {

  $repair = array(
    '&iexcl;&macr;' => '&rsquo;',		// Right single-quote
    '&uml;C' => '&ndash;',			// En dash
    '&iexcl;&ordf;' => '&mdash',		// Em dash
    '&iexcl;&deg;' => '&ldquo;',		// Left double-quote
    '&iexcl;&plusmn;' => '&rdquo;',		// Right double-quote
    '&iexcl;&ntilde;' => '',			// Probably a bullet -- kill it
    );
  
  foreach($repair as $find => $replace) {
    $section = str_replace($find, $replace, $section);
  }
  
}

