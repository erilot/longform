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

<div class="panel-layout technet-33-66-stacked container <?php if (!empty($class)) { print $class; } ?>" <?php if (!empty($css_id)) { print 'id="' . $css_id . '"'; } ?>>
  <section class="technet-section technet-section-first technet-header row">
    <div class="technet-header-first col-xs-12">
      <?php print $content['header']; ?> 
    </div>
  </section>
  <section class="technet-section technet-section-2 technet-row row">
    <div class="technet-col-first col-md-4 well">
      <div class="row">
	<div class="technet-col-first-inner inner-a technet-col-inner col-md-6">
	  <?php print $content['row-1-33-a']; ?>
	</div>
	<div class="technet-col-first-inner inner-b technet-col-inner col-md-6">
	  <?php print $content['row-1-33-b']; ?>
	</div>
      </div>
    </div>
    <div class="technet-col-last col-md-8">
      <div class="technet-col-last-inner technet-col-inner col-xs-12">
	<?php print $content['row-1-last']; ?>
      </div>	
    </div>
  </section>
  <section class="technet-section technet-section-last technet-footer row">
    <?php print $content['footer']; ?>
  </section>
</div>
