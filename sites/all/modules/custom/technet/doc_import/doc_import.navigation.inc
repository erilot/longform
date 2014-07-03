<?php

/*
 * doc_import.navigation.inc
 * version 7.x-1.0
 *
 * Provides DOM manipulation for document import. Adds nav elements such as "in this section" links, and collapsing tables and procedures.
 * 
 */


function doc_import_collapse_procedures($section){
}

function doc_import_collapse_tables($section){
}

function doc_import_add_page_links(&$content, $heading){
    module_load_include('inc','doc_import');
    module_load_include('inc', 'doc_import', 'doc_import.split');
    dpm($content,'content');
    dpm($heading,'heading');
    $in_this_section = t("In this section");
    $on_this_page = t("On this page");
    $next_heading_level = $heading + 1;
    $time = time();
    $content = extract_body($content->top()->html());
    //Add anchor links to headings, if there are any
    
    // Quit if there are no subheadings on this page.
    if (stripos($content, '</h'.$next_heading_level . '>') === FALSE) {
        return $content;
    }
    
    //// Quit if the links have already been inserted.
    //if (stripos($content,'class="link-list"') !== FALSE) {
    //    dpm('links already inserted');
    //    return;
    //}
        
    //if an old document, add the wrapper
    if (stripos($content,'<div class="wrapper"') === FALSE) {
        $content = '<div class="wrapper" heading ="' . $heading . '">' . trim($content) . '</div>';
    }

    $link_list_insert_prefix = '<div id="page-links" class="page-links"><h2>' . $on_this_page . '</h2>' . PHP_EOL . '<div id="link-list" class="link-list">' . PHP_EOL . '<ul>' . PHP_EOL;
    $link_list_insert_suffix = '</ul>' . PHP_EOL . '</div>' . PHP_EOL . '</div><!--end link insert-->';
    
    //$content->top()->find('div.wrapper')->prepend('<div id="page-links" class="page-links"><div id="link-list" class="link-list"><ul></ul></div></div>')->end();
          
    // Collect all the next-level headings in $headings
    $headings = array();
    $regex = "/<h$next_heading_level.*?>(.*?)<\/h$next_heading_level>/ui";    
    preg_match_all($regex,$content, $headings);

    $index = 0;
    foreach($headings[0] as $head){
        $head_text = htmlspecialchars(html_entity_decode(_doc_import_get_heading_text($head), ENT_COMPAT | 'ENT_HTML401'), ENT_COMPAT | 'ENT_HTML401');
        $href = "link-heading-$index";
        $source_link = "<li><a href=\"#$href\" class=\"page-xref\">$head_text</a></li>" . PHP_EOL;
        $anchor_link = "<a name=\"$href\" class=\"page-xref-anchor\"></a>";
        
        //prepend the anchor to the heading
        $content = str_ireplace($head, $anchor_link . $head, $content);

        //Build the list links
        $list .= $source_link;
        $index++;
    }
    

    $link_list = $link_list_insert_prefix . $list . $link_list_insert_suffix;
    $prefix = 'heading="' . $heading . '">';
    $content = str_ireplace($prefix, $prefix . PHP_EOL . $link_list . PHP_EOL, $content);
    $section = unprotect_special_characters($content);
    return $section;
}

function doc_import_lazy_loadify_images($section){
    $node = menu_get_object();
    $pass['placeholder'] = drupal_get_path('module','lazyloader') . '/image_placeholder.gif';	  
        
    $temp = qp($section, 'img')->each(function($index, $item) use ($pass){
        $image = qp($item);
        $href = $image->attr('src');
        $image->attr('data-source', $href)
                ->attr('storedval', $href)
                ->attr('src',$pass['placeholder'])   
                ->wrap('<span class="image-container"/>');
    });
    
    $section = $temp->top()->html();
    //}
    //else{
    //    drupal_set_message('Images have already been lazy-loadified. Skipping modification.','warning');
    //}
}

/**
 * The following two functions are adapted from Advanced Book Blocks.
 * They provide a book's table of contents in a nested array ready for theme_item_list.
 */

 function doc_import_get_book_children($mlid) {
  $nid_array = array();
  $result = db_query("SELECT mlid, weight, link_title FROM {menu_links} WHERE plid = :plid ORDER by weight, link_title", array(':plid' => $mlid));
  foreach ($result as $child) {
    if (!empty($child->mlid)) {
      $mlid_array[] = $child->mlid;
    }
  }
  if (!empty($mlid_array )) {
    foreach ($mlid_array as $child_mlid) {
      # We need the join with the node table here in order to let the i18n
      # module's rewriting kick in.
      $query = db_select('node', 'n');
      $query->join('book', 'b', 'b.nid = n.nid');
      $child_nid = $query
        ->fields('n', array('nid'))
        ->condition('b.mlid', $child_mlid)
        ->addTag('node_access')
        ->execute()
	    ->fetchField();
      if (!empty($child_nid)) {
        $nid_array[] = $child_nid;
      }
    }
  }
  return $nid_array;
}

function doc_import_recursive_book_array_builder($nid_array, &$level = 0) {
        $level++;

    // If $nid_array is not an array, assume it is a single node ID representing the top-level book node.
    // Get the book mlid of that nid, and grab the first-level children to use as the seed array.
    
    if(gettype($nid_array) != 'array') {
        $nid = $nid_array;
        $query = db_select('node', 'n');
        $query->join('book', 'b', 'b.nid = n.nid');
        $result = $query
            ->fields('n', array('title', 'status'))
            ->fields('b', array('mlid','bid','nid'))
            ->condition('n.nid', $nid)
            ->addTag('node_access')
            ->execute();
        foreach ($result as $book) {
            $book_title  = $book->title;
            $status      = $book->status;
            $mlid        = $book->mlid;
            $bid         = $book->bid;
        }
        
        $nid_array = doc_import_get_book_children($mlid);
    }
    
    foreach ($nid_array as $nid) {
        
        $query = db_select('node', 'n');
        $query->join('book', 'b', 'b.nid = n.nid');
        $result = $query
            ->fields('n', array('title', 'status'))
            ->fields('b', array('mlid','bid','nid'))
            ->condition('n.nid', $nid)
            ->addTag('node_access')
            ->execute();
        foreach ($result as $book) {
            $book_title  = $book->title;
            $status      = $book->status;
            $mlid        = $book->mlid;
            $bid         = $book->bid;
        }
        $this['data'] = l($book_title, 'node/'. $nid, array('options' => array('html' => TRUE)));
        $this['class'] = array();
        $this['class'][] = "l$level";
        $children = array();
        $children = doc_import_get_book_children($mlid);
        if (!empty($children)) {
            $this['class'][] = 'with-children';
            $subarray = doc_import_recursive_book_array_builder($children, $level);
            if (!empty($subarray)) {
                $this['children'] =  $subarray;
            }
        }
        
        $tree[] = $this;
    }
    
    if($level == 1){
        // The top level of the list will get the list attributes, so return a $wrapper array containing the component $tree.
        $wrapper['items'] = $tree;
        //$wrapper['title'] = NULL;
        $wrapper['type'] = 'ul';
        $wrapper['attributes']['class'] = array('toc','toc-list');

        $level--;
        return ($wrapper);

    }
    else {
        
        // Deeper levels just return the base $tree while it's being constructed.
        $level--;
        return $tree;
    }

}
