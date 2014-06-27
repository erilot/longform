/**
 * file: ui-tour.js
 *
 * Creates a bootstrap tour for the technet ui.
 *
 * Requires the boostrap-tour framework to be loaded.
 *
 */
(function($){
Drupal.behaviors.docManage = {
    attach: function(context,settings){

console.log('loading');
    // Instance the tour
    var tour = new Tour({
	    name: 'ui-tour',
	    //orphan: true,
	    debug: true,
	});
    
    // Tour steps
    tour.addSteps([
	{
	    element: '#navbar-header > .navbar-left button.technet-actions',
	    title: 'Contents',
	    container: '.title-block-fixed .navbar-left',
	    placement: 'right',
	    content: 'Click here to see the contents for this document and jump to other pages.',
	},
	{
	    element: '#navbar-header > .navbar-left button.settings',
	    title: 'Options',
	    container: '.title-block-fixed .navbar-left',
	    placement: 'top',
	    content: '<p>From the options dropdown, you can:</p><ul><li>Download documents as PDFs</li><li>View documents in other languages</li></ul>',
	},
	//{
	//    element: '#navbar-header > .navbar-left button.on-this-page',
	//    title: 'On this page',
	//    container: '.title-block-fixed .navbar-left',
	//    content: '<p>This dropdown has a list of all topics contained on the current page. You can use it to quickly find a topic, instead of scrolling around.',
	//},
	//{
	//    element: '#navbar-header > .navbar-right button.platform',
	//    title: 'Other platforms',
	//    container: '.title-block-fixed .navbar-right',
	//    content: '<p>If a document has different versions for different platforms (like Android and iOS), you can switch versions here.</p>',
	//},
	//{
	//    element: '#navbar-header > .navbar-right button.doctype',
	//    title: 'Other documents for this device',
	//    container: '.title-block-fixed .navbar-right',
	//    content: '<p>If there are other documents available for the same device, you can find them and switch documents here.</p>',
	//},
	//{
	//    element: '#navbar-header > .navbar-right button.release',
	//    title: 'Other releases for this document',
	//    container: '.title-block-fixed .navbar-right',
	//    content: '<p>If this document is available for other releases, you can switch to a different one here.</p>',
	//}
    ]);
    
    console.log('finished loading');
    // Stand by tour
	console.log(context);
	if (context == document) {
	    console.log('initializing');
	    tour.init();
	    if (tour.ended()) {
		tour.restart();
	    }
	    else{
		tour.goTo(0);
		tour.start();
	    }
	}
    }
};


})(jQuery)