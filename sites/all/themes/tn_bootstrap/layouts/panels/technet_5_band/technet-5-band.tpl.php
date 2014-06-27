<?php

/**
 * @file
 * A 5-band layout. Top band is "header", bottom band is "footer", the three center bands are "row-1", "row-2", and "row-3".
 */
?>

<div class="panel-layout technet-5-band container <?php if (!empty($class)) { print $class; } ?>" <?php if (!empty($css_id)) { print 'id="' . $css_id . '"'; } ?>>
  <section class="technet-section technet-section-first technet-header row">
    <?php print $content['header']; ?>
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
