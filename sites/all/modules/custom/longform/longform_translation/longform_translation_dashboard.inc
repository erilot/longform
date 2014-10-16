<?php

/**
 * Translation Manager Home page
 */

	
 /**
  * Render translation manager home page.
  *
  * Bypasses the theme layer 'cause I am too busy to deal with that now, but
  * that's where this should be in real life
  * 
  * @TODO: refactor most of this into theme
  *
  */
	
function longform_translation_dashboard($title) {
		//$opts['title'] = $title;
		$opts['intro'] = '<b>Welcome to the Technet Translation Manager Dashboard!</b><br/>You can track upcoming, in-progress, and finished work here. If there\'s another view you\'d like to see here, please <a href="mailto:eric.lotze@polycom.com">let me know</a>!';
		$opts['panes'] = array(
				'tab-job-heading' => array('type' => 'title', 'title' => t('Active Translations')),
				'tab-active-jobs' => array(
						'type' => 'view',
						'view' => 'translation_status_active',
						'display' => 'page_1',
						'title' => t('Active'),
						'intro'=>'<p>XLIFFs have been created for these documents, but translated content has not been received.</p>'
				),
				'tab-held-jobs' => array(
						'type' => 'view',
						'view' => 'translation_status_held_for_eco',
						'display' => 'page_1',
						'title' => t('Pending ECO'),
						'intro' => array('#markup' => '<p>These documents have been translated, and are waiting for ECO approval before publishing.</p>'),
				),
				'tab-published-jobs' => array(
						'type' => 'view',
						'view' => 'translation_status_published_',
						'display' => 'page_1',
						'title' => t('Published'),
						'intro' => array('#markup' => '<p>These translations have been published and are available to the public.</p>'),
				),
				'tab-divider' => array('type' => 'divider'),
				'tab-queued-heading' => array('type' => 'title', 'title' => t('Pipeline')),
					'tab-ready-jobs' => array(
						'type' => 'view',
						'view' => 'translation_status_queued',
						'display' => 'translation_queue_ready',
						'title' =>'<b>' . t('Ready to Translate') . '</b>',
						'intro' => array('#markup' => '<p>These documents have been put in the translation queue and published. <b>They are ready for translation</b>.</p>'),
				),
				'tab-queued-jobs' => array(
						'type' => 'view',
						'view' => 'translation_status_queued',
						'display' => 'translation_queue_not_ready',
						'title' => t('Queued'),
						'intro' => array('#markup' => '<p>These documents have been put in the translation queue, but aren\'t ready for translation yet (they need to be published in English first).</p>'),
				),
		);
		
		longform_dashboard_add_js($title, $opts);
		return longform_dashboard_pane_create($opts);

}
