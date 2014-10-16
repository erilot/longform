<?php
/**
 * @file
 * longform_product_navigation.pages_default.inc
 */

/**
 * Implements hook_default_page_manager_pages().
 */
function longform_product_navigation_default_page_manager_pages() {
  $page = new stdClass();
  $page->disabled = FALSE; /* Edit this to true to make a default page disabled initially */
  $page->api_version = 1;
  $page->name = 'vocaulary_switcher';
  $page->task = 'page';
  $page->admin_title = 'Vocabulary Switcher';
  $page->admin_description = '';
  $page->path = 'vocab-switcher/%vid';
  $page->access = array();
  $page->menu = array();
  $page->arguments = array(
    'vid' => array(
      'id' => 1,
      'identifier' => 'Taxonomy vocabulary: ID',
      'name' => 'entity_id:taxonomy_vocabulary',
      'settings' => array(),
    ),
  );
  $page->conf = array(
    'admin_paths' => FALSE,
  );
  $page->default_handlers = array();
  $handler = new stdClass();
  $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
  $handler->api_version = 1;
  $handler->name = 'page_vocaulary_switcher_panel_context';
  $handler->task = 'page';
  $handler->subtask = 'vocaulary_switcher';
  $handler->handler = 'panel_context';
  $handler->weight = 0;
  $handler->conf = array(
    'title' => 'Panel',
    'no_blocks' => 0,
    'pipeline' => 'standard',
    'body_classes_to_remove' => '',
    'body_classes_to_add' => '',
    'css_id' => '',
    'css' => '',
    'contexts' => array(),
    'relationships' => array(),
    'access' => array(
      'logic' => 'and',
      'plugins' => array(),
    ),
  );
  $display = new panels_display();
  $display->layout = 'longform_product_page';
  $display->layout_settings = array();
  $display->panel_settings = array(
    'style_settings' => array(
      'default' => NULL,
      'header' => NULL,
      'row-1-first' => NULL,
      'row-1-last' => NULL,
      'row-2-first' => NULL,
      'row-2-last' => NULL,
      'footer' => NULL,
    ),
  );
  $display->cache = array();
  $display->title = '%vid:name (switcher)';
  $display->uuid = '6f9478aa-2f7d-49b7-9fcb-4d50f1eece79';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-060a1b4b-6abc-46b8-a874-43aef48a8db6';
    $pane->panel = 'header';
    $pane->type = 'block';
    $pane->subtype = 'book_nav-vocabulary_navigator';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 1,
      'override_title_text' => '%vid:name',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '060a1b4b-6abc-46b8-a874-43aef48a8db6';
    $display->content['new-060a1b4b-6abc-46b8-a874-43aef48a8db6'] = $pane;
    $display->panels['header'][0] = 'new-060a1b4b-6abc-46b8-a874-43aef48a8db6';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = '0';
  $handler->conf['display'] = $display;
  $page->default_handlers[$handler->name] = $handler;
  $pages['vocaulary_switcher'] = $page;

  return $pages;

}
