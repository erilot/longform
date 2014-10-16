<?php

/**
 * Document Preview & Edit
 * Generates an jquery-switchable preview of the document post-processing. Allows editing and updating prior to node save operations.
 */
function doc_manage_preview_form_create($form, &$form_state){
  
  // Temporarily disabled.

  $form['preview'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="panel panel-danger">
      <div class="panel-heading">
	       <h2 class="panel-title">Preview temporarily disabled</h2>
      </div>
      <div class="panel-body">
	       <p>Document preview is temporarily disabled. It should be functional again shortly.</p>
      </div></div>',
  );
  return $form;

  
  module_load_include ('inc', 'doc_import', 'doc_import.build');
  drupal_add_css(drupal_get_path('theme', 'tn_bootstrap') . '/css/document.styles.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

  $ticket = menu_get_object();
  if(!file_exists("public://import-source-files/" . $ticket->nid . "/converted.lck") || $ticket->field_publish_ticket_relation[LANGUAGE_NONE]) return $form;
  
  $path = drupal_get_path('module','doc_manage');
  drupal_add_js($path. '/preview-tabs.js');
  drupal_add_css($path . '/preview-tabs.css');
  
  $node = menu_get_object();
  $split_level = array_pop(explode(' ',taxonomy_term_load($node->field_granularity[LANGUAGE_NONE][0]['tid'])->name));
  
  $content = doc_import_construct_document_array($node, $split_level);
  //derive list index and content sections
  $items= array();
  $i = 0;
  foreach($content as $chunk){
    $items[$i] = array('data' => $chunk['title'], 'class' => array('level-' . $chunk['level'],'tab'), 'tab' => array('tab' . $i));
    $body[$i]['title-html'] = '<h' . $chunk['level'] . ' class="title-preview">' . $chunk['title'] . '</h' . $chunk['level'] . '>';
    $body[$i]['title-plain'] = html_entity_decode($chunk['title'],ENT_NOQUOTES,'UTF-8'); 
    $body[$i]['contents'] = extract_body(htmlqp($chunk['content'])->find('div')->addClass('preview-content')->top()->html());
    $body[$i]['level'] = $chunk['level'];
    $i++;
  }
  $list = array(
    'items' => $items,
    'type' => 'ul',
    'attributes' => array(
      'class' => array('preview-tabs'),
    ),
  );
    
  $options['qp'] =  array(
                      'convert_from_encoding' => 'utf-8',
                      'convert_to_encoding' => 'utf-8',
                      );
  //get themed list, but strip off outer <div> so the jquery business works properly
  $themed_list = theme_item_list($list);
  $encoding = mb_detect_encoding($themed_list);
  //$index = htmlqp(iconv($encoding, 'UTF-8', $themed_list), NULL, $options['qp'])->find('.item-list')->contents()->unwrap()->html();
  $index = $themed_list;
  $preview_html = '<div id="preview-nav">' . PHP_EOL . $index . PHP_EOL . implode(PHP_EOL,$body) . PHP_EOL . '</div>';
  
  // Show this content by default (add "in" class to #preview-nav).
  // Change $in to NULL and $mode to 'collapsed' to hide instead.
  // $comment will be displayed next to the section title.
  
  $in = " in";
  $mode = "";
  $comment = "";
  
  //build form
  $form['preview'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id="doc-content" class="preview-wrapper ticket-block"> 
	  <button type="button" id="related-header" class="btn btn-header justify' . $mode . '" data-toggle="collapse" data-target="#preview-nav">
	      <span class="glyphicon glyphicon-chevron-right"></span><span class="glyphicon glyphicon-chevron-up"></span>
		Document Preview
		<span class="comment">' . $comment . '</span>
	  </button>
	  <div id="preview-nav" class="' . $in . '">',
      '#suffix' => '</div></div>',
  );
    $form['preview']['content'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id="preview-content-wrapper" class="collapsible collapsed">',
      '#suffix' => '</div>',
    );

  $form['preview']['content']['index'] = array(
    '#type' => 'markup',
    '#markup' => $index,
  );
  $i = 0;
  foreach($body as $section){
    $form['preview']['content']['section'][$i] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id "preview-section-' . $i . '" class="preview-section">',
      '#suffix' => '</div>',
    );
    $form['preview']['content']['section'][$i]['level'] = array(
      '#type' => 'hidden',
      '#value' =>  $body[$i]['level'],
      '#attributes' => array('id'=>'section-'.$i.'-level','class'=>'preview-section-level'),
    );
  
    $form['preview']['content']['section'][$i]['contents'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id="section-' . $i . '-content" class="preview-block preview-contents" level="'.$body[$i]['level'].'">',
      '#suffix' => '</div>',
    );
    $form['preview']['content']['section'][$i]['contents']['content-display'] = array(
      '#type' => 'markup',
      '#markup' => $section['contents'],
    );

    $i++;
  }
      $form['preview']['content']['edit'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div class="preview-edit">',
      '#suffix' => '</div>',
    );
      
    $form['preview']['content']['edit']['section'] = array(
      '#type' => 'hidden',
      '#value' => -1,
      '#attributes' => array('id'=>'section-id'),
    );
    $form['preview']['content']['edit']['topic'] = array(
      '#type' => 'hidden',
      '#value' => -1,
      '#attributes' => array('id'=>'section-topic'),
    );
    $form['preview']['content']['edit']['level'] = array(
      '#type' => 'hidden',
      '#value' => -1,
      '#attributes' => array('id'=>'section-level'),
    );
    $form['preview']['content']['edit']['nid'] = array(
      '#type' => 'hidden',
      '#value' => menu_get_object()->nid,
      '#attributes' => array('id'=>'base-nid'),
    );
    
    $form['preview']['content']['edit']['title-form'] = array(
      '#type' => 'textfield', 
      '#title' => t('Title'), 
      '#default_value' => '',
      '#size' => 96, 
      '#maxlength' => 128, 
      '#required' => TRUE,
      '#attributes' => array('class' => array('title-form')),
      '#id' => array('edit-title-form'),
      '#name' => array('title-form'),
    );
    
    $form['preview']['content']['edit']['content-form'] = array(
      '#type' => 'text_format', 
      '#title' => t('Content'), 
      '#default_value' => NULL,
      '#format' => NULL,
      '#required' => false,
      '#attributes' => array('class' => array('content-form')),
    );

    $form['preview']['content']['edit']['content-update'] = array(
      '#type' => 'submit',
      '#value' => 'Update',
      '#name' => 'op-update',
      '#attributes' => array('section' => -1,'class' => array('preview-button')),
      '#ajax' => array(
        'callback' => 'doc_manage_update_content',
        'wrapper' => "doc-content",
      ),
    );
    $form['preview']['content']['edit']['content-cancel'] = array(
      '#type' => 'markup',
      '#markup' => '<input type="button" id="cancel-content" name "op" value="Cancel" class="preview-button content-cancel">',
    );  
  return $form;
}
  
function doc_manage_update_content($form, &$form_state){
  

    //retrieve topic node information in the format expected by the form constructor.
    $topic = node_load($form_state['input']['topic']);
    $chunk = _doc_import_get_topic_chunk($topic);
    
    
    
    // titles must be re-inserted into topic contents because of the way previews and doc arrays are built.
    // This is extremely inelegant. Find a less dumb way to do this (here, and everywhere)
    $level= $chunk['level'];
    $title_html = "<h$level class=\"document subtitle\" level=\"$level\">" . protect_special_characters($form_state['input']['title-form']) . "</h$level>";
    
    $topic->field_topic_processed_html[LANGUAGE_NONE][0]['value'] = $form_state['input']['content-form']['value'];
    $topic->title = $form_state['input']['title-form'];
    $section = $form_state['input']['section'];
    node_save($topic);

    //rebuild updated preview form section.
    // @TODO: Update title tab via AJAX as well
    $preview_html = trim(decode_special_characters(extract_body(htmlqp($form_state['input']['content-form']['value'],'div')
                                                               ->prepend($title_html)->top()->html())));
    
    $form['preview']['section'][$section] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id "preview-section-' . $section . '" class="preview-section">',
      '#suffix' => '</div>',
    );
    $form['preview']['section'][$section]['level'] = array(
      '#type' => 'hidden',
      '#value' =>  $body[$section]['level'],
      '#attributes' => array('id'=>'section-'.$section.'-level','class'=>'preview-section-level'),
    );
  
    $form['preview']['section'][$section]['contents'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id="section-' . $section . '-content" class="preview-block preview-contents" level="'.$body[$section]['level'].'">',
      '#suffix' => '</div>',
    );
    $form['preview']['section'][$section]['contents']['content-display'] = array(
      '#type' => 'markup',
      '#markup' => $preview_html,
    );
    return $form['preview'];
}
