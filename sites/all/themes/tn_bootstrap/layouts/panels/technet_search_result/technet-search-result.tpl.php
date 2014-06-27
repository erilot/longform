<?php

/**
 * @file
 * A 5-band layout, with a split 75/25 header.
 *
 * Top band includes "header-first" (75%) and "header-second" (25%).
 * Bottom band is "footer", the three center bands are "row-1", "row-2", and "row-3".
 *
 * The "container" class is omitted since it's already included at the page layout level, and this
 * template is only ever used as a widget.
 * 
 */
?>

<div class="panel-layout technet-search-result 2col-head <?php if (!empty($class)) { print $class; } ?>" <?php if (!empty($css_id)) { print 'id="' . $css_id . '"'; } ?>>
  <section class="technet-section technet-section-first technet-header row">
    <div class="technet-col-first search-result-title col-md-12">
      <?php print $content['header-first']; ?> 
    </div>
    <div class="technet-col-last search-result-extra hidden-xs hidden-sm hidden-md hidden-lg col-md-3">
      <?php print $content['header-last']; ?>
    </div>
  </section>
  <section class="technet-section search-result-info row">
    <div class="col-sm-12">
      <?php print $content['row-1']; ?>
    </div>
  </section>
  <section class="technet-section search-result-snippet row">
    <div class="col-sm-12">
      <?php print $content['row-2']; ?>
    </div>
  </section>
</div>
