<?php

/**
 * @file
 * Default theme implementation to navigate books.
 *
 * Presented under nodes that are a part of book outlines.
 *
 * Available variables:
 * - $tree: The immediate children of the current node rendered as an unordered
 *   list.
 * - $current_depth: Depth of the current node within the book outline. Provided
 *   for context.
 * - $prev_url: URL to the previous node.
 * - $prev_title: Title of the previous node.
 * - $parent_url: URL to the parent node.
 * - $parent_title: Title of the parent node. Not printed by default. Provided
 *   as an option.
 * - $next_url: URL to the next node.
 * - $next_title: Title of the next node.
 * - $has_links: Flags TRUE whenever the previous, parent or next data has a
 *   value.
 * - $book_id: The book ID of the current outline being viewed. Same as the node
 *   ID containing the entire outline. Provided for context.
 * - $book_url: The book/node URL of the current outline being viewed. Provided
 *   as an option. Not used by default.
 * - $book_title: The book/node title of the current outline being viewed.
 *   Provided as an option. Not used by default.
 *
 * @see template_preprocess_book_navigation()
 *
 * @ingroup themeable
 */
?>
<?php if ($tree || $has_links): ?>
  <div id="book-navigation-<?php print $book_id; ?>" class="book-navigation footer">
    <!--<?php print $tree; ?>-->

    <?php if ($has_links): ?>
    <div class="page-links clearfix">
      <?php if ($prev_url): ?>
      
        <div id="nav-left-container" class="side-nav-bars" data-toggle="popover" data-trigger="hover" data-placement="right" data-content="<?php print $prev_title ?>">
          <div id="nav-left" class="book-navigation" >
            <a href="<?php print $prev_url; ?>" class="page-previous" title="<?php print t('Press the left arrow key for the previous page'); ?>"><?php print $prev_title; ?></a>
          </div>
        
        <span class="arrow-prev glyphicon glyphicon-arrow-left"></span>
        </div>
        
        <div id="nav-flyin-prev" class="nav-flyin prev btn btn-default">
          <div id="nav-flyin-prev-inner" class="inner">
            <div class="arrow">&nbsp;</div>

            <a href="<?php print $prev_url; ?>" class="page-previous" title="<?php print t('Press the left arrow key for the previous page'); ?>"><h4 class="handle">Previous</h4>
<?php print $prev_title; ?></a>
          </div>
        </div>
        
        <a href="<?php print $prev_url; ?>" class="page-previous" title="<?php print t('Press the left arrow key for the previous page'); ?>"><?php print $prev_title; ?></a>

      <?php endif; ?>
      
<!--      <?php if ($parent_url): ?>
        <a href="<?php print $parent_url; ?>" class="page-up" title="<?php print t('Go to parent page'); ?>"><?php print t('up'); ?></a>
      <?php endif; ?>
-->
      <?php if ($next_url): ?>
      
        <div id="nav-right-container" class="side-nav-bars"  data-toggle="popover" data-trigger="hover" data-placement="left" data-content="<?php print $next_title ?>">
          <div id="nav-right" class="book-navigation">
            <a href="<?php print $next_url; ?>" class="page-next" title="<?php print t('Press the right arrow key for the next page'); ?>"><?php print $next_title ?></a>
          </div>
                <span class="arrow-next glyphicon glyphicon-arrow-right"></span>

        </div>
        
        <div id="nav-flyin-next" class="nav-flyin next btn btn-default">
          <div id="nav-flyin-next-inner" class="inner">
            <div class="arrow">&nbsp;</div>

            <a href="<?php print $next_url; ?>" class="page-next" title="<?php print t('Press the left arrow key for the next page'); ?>"><h4 class="handle">Next</h4><?php print $next_title; ?></a>

          </div>
        </div>

        <a href="<?php print $next_url; ?>" class="page-next" title="<?php print t('Press the right arrow key for the next page'); ?>"><?php print $next_title ?></a>
      <?php endif; ?>
    </div>
    <?php endif; ?>

  </div>
<?php endif; ?>
