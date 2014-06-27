<?php

/**
 * @file
 * A 5-band layout, with a split 75/25 header.
 *
 * Top band includes "header-first" (75%) and "header-second" (25%).
 * Bottom band is "footer", the three center bands are "row-1", "row-2", and "row-3".
 * 
 */
?>

<div class="panel-layout technet-5-band 2col-head container <?php if (!empty($class)) { print $class; } ?>" <?php if (!empty($css_id)) { print 'id="' . $css_id . '"'; } ?>>
  <section class="technet-section technet-section-first technet-header row">
    <div class="technet-col-first col-md-9">
      <?php print $content['header-first']; ?> 
    </div>
    <div class="technet-col-last col-md-3">
      <?php print $content['header-last']; ?>
    </div>
  </section>
  <section class="technet-section technet-section-2 technet-row row">
    <?php print $content['row-1']; ?>
  </section>
  <section class="technet-section technet-section-3 technet-row row">
    <?php print $content['row-2']; ?>
  </section>
  <section class="technet-section technet-section-4 technet-row row">
    <?php print $content['row-3']; ?>
  </section>  
  <section class="technet-section technet-section-last technet-footer row">
    <?php print $content['footer']; ?>
  </section>
</div>
