<?php

/**
 * File: doc_import.graveyard.inc
 * These functions are all legacy, and are not used in production. They are retained in case we need to raid them for parts.
 */

 
/**
 * clean up after importing a document
 */
function _doc_import_cleanup($pending_doc) {
  $file_directory_path = file_directory_path();
  $pending_path = $file_directory_path . '/' . variable_get('doc_import_path_pending', '');
  $finished_path = $file_directory_path . '/' . variable_get('doc_import_path_finished', '');

  // make sure the finished path exists
  $temp = explode('.', $pending_doc);
  array_pop($temp);
  $image_rel_path = implode('.', $temp); // .'_files';
  $temp = explode('/', $image_rel_path);
  $temp_path = $finished_path;
  while (!empty($temp)) {
    $temp_path .= '/' . array_shift($temp);
   //dsm($temp_path, '$temp_path');
    if (!file_prepare_directory($temp_path, TRUE)) {
      if (!file_create_path($temp_path)) {
        form_error($form, t('Cannot create the directory for cleaning up imported documents.'));
      }
    }
  }
  $source = $pending_path . '/' . $pending_doc;
  $dest = $finished_path . '/' . $pending_doc;
  file_unmanaged_copy($source, $dest, FILE_EXISTS_REPLACE); //CHANGE BACK TO _MOVE

  $image_path = $pending_path . '/' . $image_rel_path;
  $images = file_scan_directory($image_path, '/./');
 //dsm($images, '$images');
  foreach ($images as $image) {
    $source = $pending_path . '/' . $image_rel_path . '/' . $image->basename;
    $dest = $finished_path . '/' . $image_rel_path . '/' . $image->basename;
   //dsm($source, '$source');
   //dsm($dest, '$dest');
    file_unmanaged_copy($source, $dest, FILE_EXISTS_REPLACE); //CHANGE BACK TO _MOVE
  }
  if (@chmod($image_path, 0777)) {
    unlink($image_path);
  }
}

/**
 * import documents form
 */
function doc_import_process_form($form, $form_state, $step = 1) {
  // dsm($form_state, 'loading form: $form_state');

  // validation
  if (!_doc_import_validate_settings()) {
    $form['error'] = array(
      '#title' => t('Error text'),
      '#type' => 'markup',
      '#value' => t('The module has not set properly, please contact site admin.'),
      '#prefix' => '<div class="error">',
      '#suffix' => '</div>',
    );
    return $form;
  }

  // steps
  $step = 1;
  if (!empty($form_state['storage']['step'])) {
    $step = $form_state['storage']['step'];
  }
  if (!empty($form_state['storage']['success'])) {
    $step++;
  }
  $form['step'] = array(
    '#type' => 'hidden',
    '#value' => $step,
  );

  $form['#multistep'] = TRUE;
  $form_state['#redirect'] = FALSE;

  switch ($step) {
    case 1:
      // content types
      $selected_node_types = variable_get('doc_import_content_types', array());
      // dsm($selected_node_types, '$selected_node_types');
      $node_type_options = node_type_get_names();
      $selected_node_type_options = array();
      foreach ($selected_node_types as $node_type) {
        if (isset($node_type_options[$node_type])) {
          $selected_node_type_options[$node_type] = $node_type_options[$node_type];
        }
      }
      $form['content_type'] = array(
        '#title' => t('Select content type the document will be imported'),
        '#type' => 'select',
        '#options' => $selected_node_type_options,
        '#required' => TRUE,
      );

      // pending documents
      $file_directory_path = file_directory_path();
      $pending_path = $file_directory_path . '/' . variable_get('doc_import_path_pending', '');
      $pending_doc_options = _doc_import_get_pending_docs($pending_path);
      // dsm($pending_doc_options, '$pending_doc_options');
      $form['pending_doc'] = array(
        '#title' => t('Pending documents'),
        '#type' => 'radios',
        '#options' => $pending_doc_options,
        '#required' => TRUE,
      );
      if (empty($pending_doc_options)) {
        $form['pending_doc']['#description'] = t('You have no documents to import.');
      }

      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Import',
      );

      break;

    case 2:
      if (!empty($form_state['storage']['success'])) {
        $form_state['storage']['success'] = FALSE;

        $form['pending_doc'] = array(
          '#type' => 'hidden',
          '#value' => $form_state['storage']['pending_doc'],
        );

        $file_directory_path = file_directory_path();
        $pending_path = $file_directory_path . '/' . variable_get('doc_import_path_pending', '');
        $pending_path = trim($pending_path . '/' . $form_state['storage']['pending_doc']);
        // dsm($pending_path, '$pending_path');
        $realpath = realpath($pending_path);
        // dsm($realpath, '$realpath');

        // using file_get_contents() to read the whole file at once
        $doc = '';
        $doc = file_get_contents($pending_path);
        // dsm($doc, '$doc');

        // set title
        $temp = explode('/', $pending_path);
        $title = $temp[count($temp) - 1];
        $temp = explode('.', $title);
        unset($temp[count($temp) - 1]);
        $title = implode('.', $temp);
        $form['doc_title'] = array(
          '#title' => t('Title'),
          '#type' => 'textfield',
          '#size' => 100,
          '#maxlength' => 100,
          '#required' => TRUE,
          '#default_value' => $title,
        );

        // extract the doc id
        // $pattern = '/<title[^>]>(.*)<\/title>/si';
        // preg_match($pattern, $doc, $matches);
        // dsm($matches, '$matches');

        // extract body part
        $pattern = '/<body[^>]*>(.*)<\/body>/si';
        preg_match($pattern, $doc, $matches);
        // dsm($matches, '$matches');
        $doc = trim($matches[1]);

        // handle special chars
        _doc_import_handle_special_chars($doc);
        // dsm($doc, '$doc');

        // process images
        $doc = _doc_import_handle_images_filtered($doc, $pending_path);

        // $form['doc_content'] = array(
        // '#title' => t('Document content'),
        // '#type' => 'markup',
        // '#value' => $doc,
        // '#prefix' => '<div class="import_doc_content" style="border: 1px solid #000000; padding: 10px;">',
        // '#suffix' => '</div>',
        // );
        $form['doc_content'] = array(
          '#title' => t('Document content'),
          '#type' => 'textarea',
          '#value' => $doc,
          '#rows' => 20,
        );

        // open the file line by line
        // $lines = file($pending_path, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        // dsm($lines, '$lines');
      }
      else {
        $submitted_values = $form_state['clicked_button']['#post'];
        // dsm($submitted_values, '$submitted_values');

        $form['pending_doc'] = array(
          '#type' => 'hidden',
          '#value' => $submitted_values['pending_doc'],
        );
        $form['doc_title'] = array(
          '#title' => t('Title'),
          '#type' => 'textfield',
          '#size' => 100,
          '#maxlength' => 100,
          '#required' => TRUE,
          '#default_value' => $submitted_values['doc_title'],
        );
        $form['doc_content'] = array(
          '#title' => t('Document content'),
          '#type' => 'textarea',
          '#value' => $submitted_values['doc_content'],
          '#rows' => 20,
        );
      }

      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Save',
      );

      $form_state['#redirect'] = TRUE;

      break;

    case 3:
      if (!empty($form_state['storage']['node_id'])) {
        $link = '<a href="' . url('node/' . $form_state['storage']['node_id']) . '" target="_blank">' . t('Click here to view') . '</a>';
        // $link = l('Click here to view', 'node/'. $form_state['storage']['node_id'], array('attributes' => array('target' => '_blank')));
        $form['status'] = array(
          '#type' => 'markup',
          '#value' => t('You have successfully imported a new document! !click_link.', array('!click_link' => $link)),
          '#prefix' => '<div class="form-item">',
          '#suffix' => '</div>',
        );

        $form['submit'] = array(
          '#type' => 'submit',
          '#value' => 'Import another document',
        );
      }
      break;

    default:
      break;
  }

  // dsm($form, '$form');
  // dsm($form_state, '$form_state');
  return $form;
}

/**
 * import documents form submit
 */
function doc_import_process_form_submit($form, &$form_state) {
  // dsm($form, 'doc_import_process_form_submit: $form');
  // dsm($form_state, 'doc_import_process_form_submit: $form_state');

  global $user;

  // init storage
  if (empty($form_state['storage'])) {
    $form_state['storage'] = array();
  }
  $form_state['storage']['success'] = FALSE;

  $values = $form_state['clicked_button']['#input'];
  // dsm($values, '$values');

  $step = $values['step'];
  $form_state['storage']['step'] = $step;

  switch ($step) {
    case 1:
      $file_directory_path = file_directory_path();
      $pending_path = $file_directory_path . '/' . variable_get('doc_import_path_pending', '');
      $pending_path .= '/' . $values['pending_doc'];
      // dsm($pending_path, '$pending_path');
      $realpath = realpath($pending_path);
      // dsm($realpath, '$realpath');
      if (file_exists($realpath)) {
        $form_state['storage']['content_type'] = $values['content_type'];
        $form_state['storage']['pending_doc'] = $values['pending_doc'];
        $form_state['storage']['success'] = TRUE;
      }
      break;

    case 2:
      $pending_doc = $values['pending_doc'];

      $content_type = $form_state['storage']['content_type'];
      $title = $values['doc_title'];
      $content = $values['doc_content'];
      $images = array();
      _doc_import_prepare_content($content, $images);
      // dsm($images, '$images');
      if (!empty($images)) {
        _doc_import_copy_images($images, $pending_doc);
      }
      _doc_import_substitute_image_path($content);

      $edit = array();
      $edit['type'] = $content_type;
      // set node type defaults
      $node_type_default = variable_get('node_options_' . $edit['type'], array('status', 'promote'));
      $edit['uid'] = $user->uid;
      $edit['name'] = $user->name;
      $edit['promote'] = in_array('promote', $node_type_default);
      $edit['comment'] = variable_get('comment_' . $edit['type'], 2);
      $edit['revision'] = in_array('revision', $node_type_default);
      $edit['format'] = FILTER_FORMAT_DEFAULT;
      $edit['status'] = 1;
      $edit['title'] = $title;
      $edit['body'] = $content;
      node_validate($edit);
      if ($errors = form_get_errors()) {
        form_set_error($form, t('Found errors:') . implode("\n", $errors));
      }
      else {
        $node = node_submit($edit);
        node_save($node);
        if (FALSE || $node->nid) {
          _doc_import_cleanup($pending_doc);
          watchdog('content', '@type: import %title through Doc Import module.', 
						array('@type' => $node->type, '%title' => $node->title), WATCHDOG_NOTICE, l(t('view'), "node/$node->nid"));
          $form_state['storage']['node_id'] = $node->nid;
          $form_state['storage']['success'] = TRUE;
        }
      }
      break;

    case 3:
      drupal_goto('admin/doc_import/process');
      break;

    default:
      break;
  }

  // dsm($form_state, 'doc_import_process_form_submit after processing: $form_state');
}

/**
 *	Rebuild MSWord Xrefs. supplied $content is a QueryPath object.
 */
function _doc_import_repair_word_xrefs(&$content) {

  //build an array of targets and their contained text: used by MSWord as the anchor name
  
  $targets = $content->find('a[name]');
  
  $xref=array();
  
  //loop through _Ref links and assemble an array of targets to reconstruct.
  foreach($targets as $target){
    $name = $target->attr('name');
    $text = $target->text();
    //if $text is empty, the link has been applied before the name. Get the text of the parent instead.
    if (empty($text)){
      $text = $target->parent()->text();
    }
    //dpm(array($name,$text),'target');
    
    //trim the text if it's part of a chapter
    $text = trim(array_pop(explode(":",$text)));

    //Add the resulting key and text to the $xref array, unless $text = "table".
    if($text != "Table"){
      $xref = array_merge($xref, array($name => $text));
    }
  }
  
  //Process word xrefs, marked by <span class="CrossRef">
  if($content->top()->find('span.CrossRef')->length != 0){
    $content->top()->find('p')->has('span.CrossRef')->each(function($index, $item){
      qp($item, 'span.CrossRef')->each(function($index, $item2){
	
	//strip CrossRef spans from items containing bullets or non-breaking spaces. Unwrap spans wrapping periods.
	$text = qp($item2)->html();
	if (strpos($text,'·') !== FALSE || strpos($text,'○') !== FALSE || strpos($text,'nbsp') !== FALSE){
	  qp($item2)->remove();
	}
	else if (qp($item2)->text() == '.'){
	  qp($item2)->contents()->unwrap();
	};
	// return filtered objects
	return qp($item2);
      });
      
      //mark as processed
      $exref = qp($item, 'span.CrossRef')->wrapAll('<span class="xref-processed"/>');//->contents()->unwrap();
    });
  
    $content->top()->find('span.CrossRef')->contents()->unwrap();
  }
  
//  $content->top()->find('span.xref-processed')->each(function($index, $item) use ($xref){
//	  $qp = qp($item);
//	  dpm((array)qp($item)->html(),'content to process');
//	  if (array_search($qp->text(),$xref)){
//		  //$link = '<a href="#'.array_search($qp->text(),$xref).' class="xref"></a>';
//		  $link = '<a href="#" class="link-error"/>';
//		  $qp->wrapAll($link);
//	  }
//	  
//	  //dpm(qp($item)->html(),'processed');
//  });

  $crossrefs = $content->top()->find('span.CrossRef');
  foreach($crossrefs as $crossref){
    $text = $crossref->branch()->text();
    //dpm($xref,'xref before');
    foreach($xref as $key => &$ref){

      if($text == $ref){
	$ref = 'TRUE';
	//dpm('v==MATCHED==v');
	//dpm($text,'text');
	//dpm($ref, 'ref');
	//dpm($key,'key');
	
	$crossref->wrap('<a href="#'.$key.'" class="created-xref"/>');
	//dpm($crossref->branch()->parent()->html(),'crossref');
      }
    }
  }
  //dpm($xref,'$xref');
  return ($content);
}

/*
 * Provide relative URL given a fully-defined URL
 *
 */

function return_relative_path($url){
dpm($url,'$url supplied');
  $absolute = FALSE;
	if (!$absolute){

		$url = str_replace('public://','/sites/default/files/',$url);
	}
	//$url = drupal_realpath($url);
  if (!$absolute && strpos($url, $GLOBALS['base_url']) === 0) {
    $url = /*base_path() . */ltrim(str_replace($GLOBALS['base_url'], '', $url), '/');
  }
	dpm($url,'processed URL');
	return $url;
 }

 
/**
 * Checks a string and returns TRUE if UTF-8 characters are present, FALSE if theya re not
 */

function check_utf8($str) { 
    $len = strlen($str); 
    for($i = 0; $i < $len; $i++){ 
        $c = ord($str[$i]); 
        if ($c > 128) { 
            if (($c > 247)) return false; 
            elseif ($c > 239) $bytes = 4; 
            elseif ($c > 223) $bytes = 3; 
            elseif ($c > 191) $bytes = 2; 
            else return false; 
            if (($i + $bytes) > $len) return false; 
            while ($bytes > 1) { 
                $i++; 
                $b = ord($str[$i]); 
                if ($b < 128 || $b > 191) return false; 
                $bytes--; 
            } 
        } 
    } 
    return true; 
} // end of check_utf8

/**
 * Multibyte string replace function
 */

function mb_replace($search, $replace, $subject, &$count=0) {
    if (!is_array($search) && is_array($replace)) {
        return false;
    }
    if (is_array($subject)) {
        // call mb_replace for each single string in $subject
        foreach ($subject as &$string) {
            $string = &mb_replace($search, $replace, $string, $c);
            $count += $c;
        }
    } elseif (is_array($search)) {
        if (!is_array($replace)) {
            foreach ($search as &$string) {
                $subject = mb_replace($string, $replace, $subject, $c);
                $count += $c;
            }
        } else {
            $n = max(count($search), count($replace));
            while ($n--) {
                $subject = mb_replace(current($search), current($replace), $subject, $c);
                $count += $c;
                next($search);
                next($replace);
            }
        }
    } else {
        $parts = mb_split(preg_quote($search), $subject);
        $count = count($parts)-1;
        $subject = implode($replace, $parts);
    }
    return $subject;
}

/**
 * Gets the HTML translation table and adds terms to it
 */

function get_html_translation_table_CP1252() {
    $trans = get_html_translation_table(HTML_ENTITIES);
    $trans[chr(130)] = '&sbquo;';    // Single Low-9 Quotation Mark
    $trans[chr(131)] = '&fnof;';    // Latin Small Letter F With Hook
    $trans[chr(132)] = '&bdquo;';    // Double Low-9 Quotation Mark
    $trans[chr(133)] = '&hellip;';    // Horizontal Ellipsis
    $trans[chr(134)] = '&dagger;';    // Dagger
    $trans[chr(135)] = '&Dagger;';    // Double Dagger
    $trans[chr(136)] = '&circ;';    // Modifier Letter Circumflex Accent
    $trans[chr(137)] = '&permil;';    // Per Mille Sign
    $trans[chr(138)] = '&Scaron;';    // Latin Capital Letter S With Caron
    $trans[chr(139)] = '&lsaquo;';    // Single Left-Pointing Angle Quotation Mark
    $trans[chr(140)] = '&OElig;    ';    // Latin Capital Ligature OE
    $trans[chr(145)] = '&lsquo;';    // Left Single Quotation Mark
    $trans[chr(146)] = '&rsquo;';    // Right Single Quotation Mark
    $trans[chr(147)] = '&ldquo;';    // Left Double Quotation Mark
    $trans[chr(148)] = '&rdquo;';    // Right Double Quotation Mark
    $trans[chr(149)] = '&bull;';    // Bullet
    $trans[chr(150)] = '&ndash;';    // En Dash
    $trans[chr(151)] = '&mdash;';    // Em Dash
    $trans[chr(152)] = '&tilde;';    // Small Tilde
    $trans[chr(153)] = '&trade;';    // Trade Mark Sign
    $trans[chr(154)] = '&scaron;';    // Latin Small Letter S With Caron
    $trans[chr(155)] = '&rsaquo;';    // Single Right-Pointing Angle Quotation Mark
    $trans[chr(156)] = '&oelig;';    // Latin Small Ligature OE
    $trans[chr(159)] = '&Yuml;';    // Latin Capital Letter Y With Diaeresis
    ksort($trans);
    return $trans;
}        

/* Takes a QueryPath object set and combines their contents into a single string 
 */
function combine_qp_elements($object){
    echo ('elements found:' . $object->size(). '<br/>');
		if($object->size() != 0){
				foreach($object as $item){
          //$item->find('p.placeholder')->remove();
					$return = $return . $item->HTML();
				}
		}
    else{
      echo 'Nothing here!';
    }
    replace_special_characters($return);
	echo 'Combined elements: ' . $return . '<br/>';
  	return $return;
  
}

/**
 * Helper function to dynamically get the tid from the term_name
 *
 * @param $term_name Term name
 * @param $vocabulary_name Name of the vocabulary to search the term in
 *
 * @return Term id of the found term or else FALSE
 */
function _get_term_from_name($term_name, $vocabulary_name) {
  if ($vocabulary = taxonomy_vocabulary_machine_name_load($vocabulary_name)) {
    $tree = taxonomy_get_tree($vocabulary->vid);
    foreach ($tree as $term) {
      if ($term->name == $term_name) {
        return $term->tid;
      }
    }
  }

  return FALSE;
}

/**
 * Save topics and create relation for controller->topics
 */
function doc_import_save_topics($content_array, $controller = NULL){
  dpm('doc_import_save_topics called');
    if (!$controller) $controller = menu_get_object();
      
    //add controller to relation array
    $relation_endpoints[] = array('entity_type' => 'node', 'entity_id' => $controller->nid);
  
    $batch = _doc_import_save_topics_batch($content_array, $controller);    
    batch_set($batch);
    
    //prepare each new node, attach fields, and save. Update relation builder array to create relation after loop.
    foreach($content_array as $section){
    }

    //if this content has been converted already, delete the old topics and relation before proceeding
    if(isset($controller->field_topic_used_in_)){
	$rid = $controller->field_topic_used_in_[LANGUAGE_NONE][0]['rid'];
	drupal_set_message('This has already been converted. To re-convert, first delete ' .
			   l('Relation ' . $rid, 'relation/' . $rid) . ' and all of the topics it references.', 'warning');
	return 'converted';
    }
  
    foreach($relation_endpoints as $chunk){
	$node = node_load($chunk['entity_id']);
	$node->field_topic_used_in_[LANGUAGE_NONE][0]['value'] = $new_id;
	node_save($node);
    }
  
    return FALSE;
}
