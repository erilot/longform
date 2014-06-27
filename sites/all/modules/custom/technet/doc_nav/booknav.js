(function ($)
{

// Requires jQuery 1.7+.
     if (parseFloat($('body').jquery) < 1.7) return;
     
// Older JS doesn't use "on", which breaks everything. Try a nonsense call using "on", and if it doesn't work, bail out.
try {
  $('body').on('click','.noexist',function(){});
} catch(e) {
  console.log('booknav.js did not run.');
  return;
}

try {
     $('.noexist').affix();
} catch(e) {
     console.log('bootstrap not loaded.');
     return;
}

/**
 * Collapse a document element.
 *
 * Automatically determines whether the collapsing element is a table or procedure, and does the jiujitsu.
 *
 * Options:
 * 	collapseProcedure {boolean}:
 * 		Whether to collapse procedures. Default passed in from book_nav module.
 * 		
 * 	collapseTable {boolean}:
 * 		Whether to collapse tables. Default passed in from book_nav module.
 * 		
 * 	collapseThreshold {0-1}:
 * 		Decimal value that equates to a percentage of screen height, below which a procedure will not collapse.
 * 		
 *	procedureSelector {string}:
 *		jQuery selector for procedures to collapse
 *		
 * 	tableSelector {string}:
 * 		jQuery selector for tables to collapse
 *
 */
$.fn.collapseElement = function(action, options){
     
     if (!Drupal.settings.book_nav || !action) {
	  return this;
     }
     
     var settings = $.extend({
          collapseProcedure : Drupal.settings.book_nav.procedure_collapse,
	  collapseTable : Drupal.settings.book_nav.table_collapse,
	  collapseThreshold: Drupal.settings.book_nav.procedure_collapse_threshold,
	  procedures : '.numbered-task-introduction',
	  tables : 'table:not(.note):not(.sticky-header)',
     }, options );
     
     settings.collapseThreshold = settings.collapseThreshold * $(window).height();
     
     switch (action) {
	  
	  case 'collapse':
	       i = 0;
	       var lookup = this.find(settings.procedures).add(this.find(settings.tables));
	       
	       return lookup.each(function() {
		
		    // Set the caption element. This will be used as the collapse button/trigger label.
		    if ($(this).prev('p.caption').next('table').length != 0) {
		      
			//This is a table with a caption, so the caption is the preceding paragraph element
			 var caption = $(this).prev('p.caption');
		    }
		    else {
		      if ($(this).prop('tagName') != 'TABLE') {
			
			// This is not a table, so the lookup element is already a caption.
			 var caption = $(this);
		      }
		    }
		    
		    // If no caption was found using either method, return the object unaltered
		    if (caption == null) {
		      return $(this)
		    }
		    
		    // Otherwise, set "label" as the caption text, and proceed to collapse.
		    var label = caption.text();

		    // Procedures
		    
		    if (settings.collapseProcedure && caption.next('ol').length != 0 || caption.next('ul').find('.task-one-step').length != 0) {
			 // If tagged with "no-collapse" or if smaller than threshold, set state to "in"
			 if (caption.hasClass('no-collapse') || caption.next('ol li, ul li').length > 1) {
			      caption.next('ol, ul').wrap('<div class="procedure-wrapper"/>').parent().attr('id','procedure-' + i).addClass('in');
			      caption.after('<button class="btn btn-default btn-block procedure" index="' + i + '" data-toggle="collapse" data-target="#procedure-' + i + '"><span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' + label + '</button>').remove(); 
			 }
			 else{
			      caption.next('ol, ul').wrap('<div class="procedure-wrapper"/>').parent().attr('id','procedure-' + i).addClass('collapse');
			      caption.after('<button class="btn btn-default btn-block collapsed procedure" index="' + i + '" data-toggle="collapse" data-target="#procedure-' + i + '"><span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' + label + '</button>').remove();
			 }
			 i++;
		    }
		    
		    // Tables
		    if (settings.collapseTable && caption.length != 0 && caption.next('table:not(.note)').length != 0 && !caption.hasClass('no-collapse')) {

			 var wrapper = caption.nextUntil('*:not(table:not(.note))');
			 var table = wrapper.find('table:not(.sticky-header)');
			 // For the animation to work smoothly, the table must be wrapped in a container, and the animation done on that.
			 // We'll add the "table-responsive" class to the wrapper since we're here anyway.
			 wrapper.wrap('<div class="table-wrapper table-responsive"/>').parent('.table-wrapper').attr('id','table-' + i).addClass('collapse');
			 caption.after('<button class="btn btn-default btn-block collapsed table" index="' + i + '" data-toggle="collapse" data-target="#table-' + i + '"><span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' + label + '</button>').remove();
			 i++;

		    }
	       });
	       break;
	  
	  case 'expand':
	       
	       this.find('button.procedure').add(this.find('button.table')).each(function(){
		    var elem = $(this);
		    
		    if (elem.hasClass('procedure')) {
			 var intro = '<p class="numbered-task-introduction">' + elem.text() + '</p>';
			 elem.next('.procedure-wrapper').contents().unwrap();
			 elem.replaceWith(intro);
		    }
		    
		    if (elem.hasClass('table')) {
			 var intro = '<p class="caption">' + elem.text() + '</p>';
			 elem.next('.table-wrapper').contents().unwrap();
			 elem.replaceWith(intro);
		    }
		    return elem;
	       });
	       return this;
	       
	       break;
     }
     
}

/**
 * Reverses the collapse procedure, unwrapping the content and restoring the base markup.
 *
 */
$.fn.unCollapseElement = function() {
     
}



// Utility functions

/**
  * Check whether an element is visible in the browser or not. Return true if it is, false if not.
  *
  * @param elem	The element to check.
  * 
  */
function isScrolledIntoView(elem)
{
     var docViewTop = $(window).scrollTop();
     var docViewBottom = docViewTop + $(window).height();
     var elemTop = $(elem).offset().top;
     var elemBottom = elemTop + $(elem).height();
     return ((elemBottom <= docViewBottom) && (elemTop >= docViewTop));
}

/**
 * Scroll the page back to avoid hiding headings underneath titlebars.
 *
 * @param hash  The hash to offset from. Should not include the leading '#'.
 *
 * @param speed  The velocity of the scroll action. 0 is instant.
 *
 */

// function scrollBack: 
function scrollBack(hash, speed) {
     if ($('body a[name="'+hash+'"]').length == 0) return;
     
     setTimeout(function (){
	  if (document.documentElement.clientHeight + $(document).scrollTop() >= document.body.offsetHeight)
	  {}
	  else{
	       $('html,body').stop().animate({
		    scrollTop: ($('body a[name="'+hash+'"]').offset().top) - ($('.title-block-fixed').outerHeight() + 20 )
	       }, speed);
	  }
     }, 0);
 }


/**
 * Set and manage positioning of the TOC (region first). This must be manually handled because it's been
 * pulled out of document flow and can't use relative spacing.
 *
 * @param: none
 *
 */
function updateOnResize() {
     // The width of the titlebar must be locked to prevent it from jumping out when affixed
     $('.book-ui #navbar-header').css({
	  'width': $('.title-block-main').innerWidth(),
     });
     
    // Position prev/next arrows. Absolutely positioned, so must be updated on resize.

    $('.side-nav-bars').css({'top': ($(window).height() / 2) - ($('#nav-left a').outerHeight() / 2)});
    var leftNav = $('#nav-left-container');
    var rightNav = $('#nav-right-container');
    
    if (leftNav.length != 0){
      leftNav.css({'left' : $('.technet-section-2').offset().left /*- leftNav.outerWidth()*/})
    }
    
    if (rightNav.length != 0) {
      rightNav.css({'left' : $('.technet-section-2').width() + $('.technet-section-2').offset().left - rightNav.outerWidth()});
    }
    
    
}
 
// Drupal.behaviors.docManage: These functions fire on doc ready, and every time Drupal updates content via AJAX

Drupal.behaviors.docManage = {
    attach: function(context,settings){


  // Only do this section on initial pageload, to prevent double-firing if AJAX updates the page
  if (context == document) {
     
     //Collect metadata from Drupal.settings.book_nav object
     var booknavSettings = Drupal.settings.book_nav;
     var collapseProcedure = booknavSettings.procedure_collapse;
     var collapseTable = booknavSettings.table_collapse;
     var collapseThreshold = booknavSettings.procedure_collapse_threshold;
     var clickToExpand = booknavSettings.click_to_expand;
     var clickToCollapse = booknavSettings.click_to_collapse;
     
     // Set the threshold for collapsing procedures. Lists that are taller than this value will be collapsed.
     var collapseThreshold = $(window).height() * .5; 	// Set to 50% window height

     
// ===== Content patches =====
     
     
     // Temporary fix to make sure all tables are stamped with "table" class.
     // @TODO: Build this into a QueryPath cron job so they're updated in DB instead of here.
     $('table:not(.note):not(.table)').addClass('table').removeClass('fixed');
     
     // Headings sometimes end up with embedded anchors instead of preceding anchors. Find these and switch them.
     $('h2 a').parent('h2').each(function(){
	  var head = $(this);
	  var link =  head.find('a');
	  link.clone().prependTo(head).contents().remove();
	  link.contents().unwrap();
     });
     
     //IE hack to avoid lack of 'nth-child" selectors in IE8: sets background color on tables and lists in tables
     $('.css-set-1 table.note td:nth-child(2)').addClass('note-shade-gray');
     $('#doc-content .css-set-1 table.note tr td:nth-child(2) li p').addClass('note-shade-gray');
     
     //Tables
     $('td:has(p.TableBodyHeading)').addClass('in-table-heading');
     $('td:has(p.table-heading)').addClass('in-table-heading');
     
     // Links: add a 'new page' icon to outbound links
     $('a.link-external').append(' <span class="glyphicon glyphicon-new-window"></span>');
     
// ===== UI Automation =====
    
     // If a hash is in the URL, find the target and make sure its parent table/procedure does not collapse.
     if (window.location.hash) {
	  var hash = window.location.hash.substring(1);
	  $('#doc-content a[name="' + hash + '"]').parents('table').prev('p.caption').addClass('no-collapse');
     }
    
    
     // Collapse tables and procedures

     $('#doc-content').collapseElement('collapse');

     
     if ($('html').hasClass('no-touch')) {
	  // Apply popover for collapse triggers
	  $('#doc-content').tooltip({
	       selector: 'button[data-toggle="collapse"]',
	       title: 'Click to expand or collapse',
	       trigger: 'hover',
	       placement: 'top',
	       html: true,
	       container: 'body',
	  });
	  $('.toc').tooltip({
	       selector: 'button[data-toggle="collapse"]',
	       title: 'Click to expand or collapse',
	       trigger: 'hover',
	       placement: 'right',
	       html: true,
	       container: 'body',
	  });
     }
     
// ===== TOC collapsing =====

     // Collapse everything by default.
     $('.toc-list ul:not(.in)').addClass('collapse');
     
     // Check for single-item groups and open those.
     var items = $('ul.toc-list > li');
     if (items.length == 1) {
	  var target = items.find('button').attr('data-target');
	  items.find('button').removeClass('collapsed');
	  $(target).removeClass('collapse').addClass('in');
     }  

// ===== Views Groupings ======
     
    // Convert views groupings to bootstrap collapsing elements
     var i = 0;
     $('.view-grouping-header').each(function(){
	  var label = $(this).text();
	  $(this).next('.view-grouping-content').attr('id','group-' + i).addClass('collapse');
	  $(this).after('<button class="btn btn-tb-fixed btn-block justify collapsed" index="' + i + '" data-toggle="collapse" data-target="#group-' + i + '"><span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' + label + '</button>').remove();
	  i++;
     });
   
// ===== Navigation UI =====
     
     // These are only inserted on internal book pages (not the cover page).
     if($('.book-inside').length != 0){
    
	  // === Affixed left/right nav elements
	  
	  $('#nav-left-container').prependTo('#doc-content');
	  $('#nav-right-container').appendTo('#doc-content');

	  
	  //Apply popover for last/next navs
	
	  var popovers = $('[data-toggle="popover"]');
	  var prevs = popovers.filter('[data-placement="left"]');
	  var nexts = popovers.filter('[data-placement="right"]');
	  $('[data-toggle="popover"]').popover({
	       trigger: 'hover',
	       container: '.technet-5-band',
	       html: true,
	  });

	  // === Fly-in nav links (appear at the bottom of the page when scrolled into view)
	  
	  if ($('a.page-previous').length != 0) {
	       $('#nav-flyin-prev').appendTo($('#doc-content'));
	  }
	  
	  if ($('a.page-next').length != 0) {
	       $('#nav-flyin-next').appendTo($('#doc-content'));      
	  }
    
	  // Event handlers for fly-in divs. Set the in/out class based on footer visibility.
	  $(window).scroll(function(){
	       if (isScrolledIntoView('.technet-section-last .views-table') && !$('.nav-flyin').hasClass('in')) {
		    $('.nav-flyin').toggleClass('in');
		    $('.side-nav-bars').toggleClass('out');
	       }
	       else if(!isScrolledIntoView('.technet-section-last .views-table') && $('.nav-flyin').hasClass('in')) {
		    $('.nav-flyin').toggleClass('in');
		    $('.side-nav-bars').toggleClass('out');
	       }
	  })
     }
   
       
// ===== Document UI Nav Bar =====
    
    // dynamically position dropdowns.
        
    // Move "on this page" links into nav bar. If there are none, remove the button.
//     if ($('#page-links').length == 0){
//	  $('button.on-this-page').remove();
//     }
//     else {
//	  $('#page-links').remove();
//     }
//
     // Move the TOC div into the navbar dropdown.
     $('#toc-sidebar').addClass('toc-main').removeClass('col-sm-3').appendTo('.toc-insert');
     
     
     // Dynamic window-resizing actions. Do these once on initial load, and then again on window resizes.
     updateOnResize()
     $(window).resize(function(){
	  updateOnResize();
     });
     
    // Affix the navbar.
     $('.title-block-fixed').affix({
	  offset: {
	       top: function() {
		    return (this.top = ($('.title-block-fixed').offset().top));
	       }
	  }
     });
     
     // Add content pad on affix event
     // ** Note: the 'affixed.bs.affix' event is not yet in a bootstrap release. Workaround wtih window.scroll for now.
     // When the affix event fires, resize col first
     
     $(window).scroll(function(){
	  if($('.title-block-fixed.affix').length > 0) {
	       
	       $('.technet-section-2').addClass('padded').css({'padding-top': $('.title-block-fixed').outerHeight()});
	  }
	  else {
	       $('.technet-section-2').addClass('padded').css({'padding-top': ''});
	  }
     });
     
     // Apply popover for multiple-device documents
     $('body').popover({
	  selector: 'a.device-group',
	  title: 'This document applies to:',
	  content: $('.device-list'),
	  trigger: 'hover',
	  placement: 'bottom',
	  html: true,
	  container: 'body',
     });
     
// ===== UI actions =====
     
    // Assign left and right arrow keys to previous and next page links.
    
    $(document).keydown(function (event){
      //previous (key left)
      if (event.which == 37){
        if ($('a.page-previous').length != 0){
          window.location.href = $('a.page-previous').attr('href');
        };
      };
      //next (key right)
      if (event.which == 39){
        if ($('a.page-next').length != 0){
          window.location.href = $('a.page-next').attr('href');
        };
      };
    });
     
    // @TODO: Write "in this section" component for pages with no content

     // ===== Scroll handlers =====
     
     // These handlers adjust the window scroll value to ensure headings don't get hidden behind affixed toolbars.
     
     // Reverse scroll on pageload, if a hash is present in the URL
     if (window.location.hash){
	  var hash = window.location.hash.substr(1);
	  scrollBack(hash, 0);
     }
    
     // Reverse scroll on click. Take hash from URL instead.
     $('#doc-content').on('click', 'a', function() {
	  if ($(this).parents('.dropdown-menu').length == 0 ){
	    var href = $(this).attr("href");
	    var hash = href.substr(href.indexOf("#")+1);	
	    scrollBack(hash, 200);
	  }
     });

// ===== Google Translate =====

    // If the disclaimer cookie already exists on pageload, reveal GT widget and dimsiss the disclaimer.
     if ($.cookie('google-translate-ok')) {
	  var gtWidget = $('.gt-widget');
	  gtWidget.removeClass('hidden');
	  gtWidget.text(gtWidget.text().trim());
	  $('ul.machine-options').remove();
     }
    
     // When the disclaimer is accepted, remove the disclaimer content and provide further instructions. Set cookie.
     $('body').on('click', '.disclaimer button.accept', function(){
	  var insert = '<div class="alert alert-success"><strong>Select your language:</strong> Choose your language from the list below, then click <b>close</b>.</div>'
	  $('#google_translate_element').removeClass('hidden');
	  $('.disclaimer .modal-footer .btn').addClass('accepted btn-success').removeClass('btn-default');
	  $('.disclaimer .modal-body').contents().remove();

	  $('#google_translate_element').prependTo('.disclaimer .modal-body');
	  //$('.goog-te-menu-frame').appendTo('.disclaimer .modal-body');
	  $('.disclaimer .modal-body').addClass('accepted').prepend(insert);
	  $.cookie('google-translate-ok', 1);
     });

     // When the disclaimer modal is closed, open the toolbar behind it so the user knows where to go.
     $('#disclaimer').on('hidden.bs.modal', function (e) {
	  //$('.settings-button').addClass('open');
     });

// ===== Warnings =====

     //Set warnings and log cookies for warnings that have already been closed, so they don't constantly reappear.
    if ($('.title-block-fixed .warning').length != 0) {
	  var bid = Drupal.settings.book_nav.bid;
	  var from = Drupal.settings.book_nav.thisRelease;
	  var to = Drupal.settings.book_nav.targetRelease;
	  if (!$.cookie('tn-dismissed-' + bid + '-' + from + '-' + to)){
	       $('.warning-dropdown .alert').clone().prepend('<button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>').prependTo('.region-two-33-66-second');
	  }
    }
    
    $('body').on('click', '.alert-warning button.close', function(){
	  $.cookie('tn-dismissed-' + bid + '-' + from + '-' + to, 1);
    });
    
// ===== Other page actions =====
    
     // If links were not matched during import, mark them via CSS.
     // If the user is not logged in, strip the link element. Leaves styled text behind. 
     $('a[href^="/REPLACE-WITH-NID"]').addClass('link-error');
     
     if (!$('body').hasClass('logged-in')) {
	  $('a.link-error').wrap('<span class="link-error"/>').contents().unwrap();
     }
    
     // Move all modals to the front of the <body>, to keep them from getting trapped in dropdowns
     $('.modal').prependTo('body');

     // Add vertical scroll to marked elements if required
     $('.scroll').each(function(){
	  var elem = $(this);
	  if (elem.height() < elem.contents().outerHeight) {
	       elem.css({'overflow-y':'scroll'});
	  }
     })

    /**
     * Development
     */
    
// ===== Tocify =====
    var heading = $('#doc-content .wrapper').attr('heading');
    var findFirst = Number(heading)+1;
    var findNext = findFirst+1;
    var findThis = ('h' + findFirst + ', h' + findNext);

     $('.on-this-page-dropdown-main').tocify({
	  context: '#doc-content',
	  selectors: findThis,
	  extendPage: false,
	  showEffect: 'fadeIn',
	  hideEffect: 'fadeOut',
	  scrollTo: $('.title-block-fixed').outerHeight(),
     });
     
     $('.tocify-item a').prepend('<span class="glyphicon glyphicon-arrow-right"/>');

     // Prevent 'on this page' dropdown from closing when clicking on root-level index links
     $('.btn-group-on-this-page').on({
	  'show.bs.dropdown' : function () {
	       $(this).data('closable' , true);
	  },
	  'click': function (event) {
	       var target = event.target;
	       var parent = $(target).parent('li').parent('ul');
	       if (parent.hasClass('tocify-header')) {
		    $(this).data('closable', false);
	       }
	       else {$(this).data('closable', true);}
	  },
	  'hide.bs.dropdown' : function () {
	       return $(this).data('closable')
	  }
     });
     
     // Prevent TOC dropdown from closing when clicking on toc items
     $('.btn-group-technet-actions, .btn-group-search, .keep-alive').on({
	  'show.bs.dropdown' : function() {
	       $(this).data('closable', true);
	  },
	  'click': function(event) {
	       var target = event.target;
	       if ($(target).parents('.prevent-close').length != 0 || $(target).hasClass('form-control')) {
		    $(this).data('closable', false);
	       }
	       else { $(this).data('closable', true);}
	  },
	  'hide.bs.dropdown' : function(event) {
	     return $(this).data('closable');  
	  }
     });

     // Clicks anywhere outside the title bar div should close all dropdowns.
     $('body').on('click', function(event){
	  if (!$(event.target).parents('.navbar').length) {
	       $('.btn-group').removeClass('open');
	  }
     });
     
     
     // Click panel switcher
     var choices = $('.switcher-container label');
     
     choices.each(function(){
	  $(this).on('click', function(){
	       var target = $(this).find('input[type="radio"]').attr('id');
	       $('.panels-container .settings-panel').removeClass('active');
	       $('.panels-container .settings-panel[data-display="' + target + '"]').addClass('active');
	  });
     });
 
     // Doctype by release switcher. Check modernizr for touch or no-touch and adjust trigger accordingly.
     
     if ($('html').hasClass('no-touch')) {
	  var switchTrigger = 'mouseover';
     }
     else {
	  var switchTrigger = 'click';
     }
     
     $('.doctypes-by-release-parents>li a').on(switchTrigger, function(){
	  var release = $(this);
	  var target = release.attr('href');

	  // Make sure the child div is at least as high as the parent div
	  var parentHeight = release.parents('.parents').height();
	  var childHeight = $(target).height();
	  
	  if (parentHeight > childHeight) {
	       $(target).css({'height': parentHeight});
	  }
	  
	  release.parents('ul').find('.group-release.active').removeClass('active');
	  release.parent('li').addClass('active');
	  
	  $('.children .group-child.active').removeClass('.active').stop(true,true).fadeOut(0);
	  $(target).addClass('active').stop(true,true).fadeIn(200);
	  $(target).parent().addClass('active');
     })

     
     
    }
    }
}
})(jQuery);