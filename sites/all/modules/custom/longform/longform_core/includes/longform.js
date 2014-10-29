(function ($)
{
  
  /**
   * Library function: checks whether an element is visible in the browser.
   */
  function isScrolledIntoView(elem)
 {
     var docViewTop = $(window).scrollTop();
     var docViewBottom = docViewTop + $(window).height();
 
     var elemTop = $(elem).offset().top;
     
     var elemBottom = elemTop + $(elem).height();
     return ((elemBottom <= docViewBottom) && (elemTop >= docViewTop));
}

  Drupal.behaviors.technet = {
  attach: function(context,settings){

  
  // Older JS doesn't use "on", which breaks everything. Try a nonsense call using "on", and if it doesn't work, bail out.
  try {
    $('body').on('click','.noexist',function(){});
  } catch(e) {
    return;
  }

// ===== TOC collapsing =====

     // Collapse everything by default.
     $('.toc-list ul').addClass('collapse');
     
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
	  $(this).after('<button class="btn btn-tb-fixed justify collapsed" index="' + i + '" data-toggle="collapse" data-target="#group-' + i + '"><span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' + label + '</button>').remove();
	  i++;
     });

    // Click handler for finder view headers
    $('body').on('click','.finder .view-grouping-header', function(){
	$(this).toggleClass('open').next('.view-grouping-content').slideToggle(200);
    });
    
    // Click handler for single-depth finder view headers
    $('body').on('click','.views-table>caption', function(){
	$(this).toggleClass('open').next('tbody').fadeToggle(250);
    });
    
    // Portal block headers: append a list-element count to H2 and collapse div 
    $('.portal .finder h2.block-title+div.block-content').not('.processed').addClass('processed').each(function(){
      var Heading = $(this).prev('h2.block-title');
      var numBullets = Heading.next('div.block-content').find('li').size();
      Heading.text(Heading.text()+" (" + numBullets + ")");
    });

    // Click handler for portal block headers    
    $('.portal .finder h2.block-title').each(function(){
	var label = $(this).text();
	$(this).next('.block-content').attr('id','block-group-' + i).addClass('collapse enable-hover');
	$(this).after('<button class="btn btn-default justify collapsed" index="' + i + '" data-toggle="collapse" data-target="#block-group-' + i + '"><span class="glyphicon glyphicon-chevron-down"></span><span class="glyphicon glyphicon-chevron-right"></span>' + label + '</button>').remove();
	i++;
      });
    
  
    // Hover handler for timestamp. Activated on any "slugline" class in a list element.
    var sluglines = $('.slugline').parents('li');
        
    sluglines.each(function(){
      $(this).on('hover', function(){
	$(this).find('.slugline').toggleClass('show');
      });
    });
 
    // Add "doc-content" to topic region
    $('.node-type-topic .region-two-66-33-first').attr('id','doc-content');
  
    // Sticky sidebars
    
    if ($('.region-two-33-66-first .region-inner').offset() && $('.title-block-fixed').length) {
      // Popup sidebar title
      var titleBottom = $('.title-block-fixed').offset().top;
	$(window).scroll(function(){ // scroll event
     
	  var windowTop = $(window).scrollTop(); 
	  var stickyBlock = $('.sticky-sidebar .region-two-33-66-first .region-inner');
  
	  // Reveal peekaboo title in sidebar once main title scrolls off the page
	  if (titleBottom < windowTop) {
	    stickyBlock.find('#sidebar-title').slideDown(200).addClass('visible');
    
	  }   
	  else {
	    stickyBlock.find('#sidebar-title').slideUp(200).removeClass('visible');	  
	  }    
	});
	
      $('.sticky-sidebar .region-two-33-66-first').append('<div class="spacer">&nbsp;</div>');
      $('.sticky-sidebar .region-two-33-66-first .region-inner').affix({'offset': $('.region-two-33-66-first').offset().top});
    
    // Stick finder sidebar blocks
    $('.finders .region-two-33-66-first').append('<div class="spacer">&nbsp;</div>');
    $('.finders .region-two-33-66-first .region-inner').affix({'offset': $('.region-two-33-66-first').offset().top});
     }
     
// ===== Dropdown menu width control ======

    function sizeDropdownsOnResize() {
  
	var containerWidth = $('.main-container').width();
	$('.dropdown-menu').each(function(){
	  var drop = $(this);
	  
	  var contentOffset = $('.main-container').offset().left;
	  var thisOffset = drop.parents().offset().left;
	  
	  var oset = thisOffset /*- contentOffset - 3*/;
	  if (drop.hasClass('full-width')) {
	    var drawerWidth = 1;
	    if (drop.parent().hasClass('btn-group')) {
	      oset = -(contentOffset - thisOffset + 31); // 31 = bootstrap gutter (15 * 2) + 1 (border width)
	    }
	    else {
	      oset = (contentOffset - thisOffset + 31); // 31 = bootstrap gutter (15 * 2) + 1 (border width)
	    }
	    drop.css({
	      'left' : oset,
	      'width' : containerWidth * drawerWidth
        });
	  }
	  else if(drop.hasClass('half-width')) {
	    var drawerWidth = .5;
	    oset = 0;
	    drop.css({
	      'left' : -oset,
	      'width' : containerWidth * drawerWidth
	    });
	  }
	});
    }
    
    // Dynamic window-resizing actions. Do these once on initial load, and then again on window resizes.
     sizeDropdownsOnResize()
     $(window).resize(function(){
	  sizeDropdownsOnResize();
     });

      // ===== Taxonomy switcher JS =====

      if ($('html').hasClass('no-touch')) {
          var switchTrigger = 'mouseover';
      }
      else {
          var switchTrigger = 'click';
      }

      $('.build-container').prepend('<ul class="build-container-list nav nav-pills"></ul>');
      // General switcher
      $('.taxonomy-switcher>li a').on(switchTrigger, function(event){

          var term = $(this);
          var target = term.attr('data-selector');
          var thisCol = term.parents('.switcher-column');
          var thisColNum = thisCol.attr('data-column');

          var higherCols = thisCol.parents('.chooser-wrapper').find('.switcher-column').filter(function(index) {return $(this).attr('data-column') > thisColNum;});
          // Make sure the child div is at least as high as the parent div
          var parentHeight = term.parents('.switcher-column').height();
          var childHeight = $(target).height();

          if (parentHeight > childHeight) {
              $(target).css({'height': parentHeight});
          }

          //term.parents('ul').find('li.active').removeClass('active');
          //if (!term.hasClass('invalid')) {
          //    term.parent('li').addClass('active');
          //}
          // Hide others
          higherCols.removeClass('reveal').find('.reveal').removeClass('reveal').stop(true,true).fadeOut(0);

          // Reveal selected
          $(target).addClass('reveal').stop(true,true).fadeIn(200);
          $(target).parent().addClass('reveal');
      });

      $('.taxonomy-switcher>li a.invalid').on('click', function(event) {
          event.preventDefault();
      });

      $('.taxonomy-switcher>li a.valid').on('click', function(event) {
          tid = $(this).attr('data-tid');
          console.log('tid: ' + tid);
          dataStore = $('#data-store');
          if (dataStore.attr('value') == '') {
              dataStore.attr('value', tid);
          }
          else {
              tidArray = dataStore.attr('value').split(',');
              tidArray.push(tid);
              dataStore.attr('value', tidArray.join(','))
          }
          $('#add-ticket-submit').removeClass('disabled');

          $(this).parents('li').removeClass('active').addClass('bg-success').addClass('disabled');
          $('.build-container-list')
              .append('<li class="list-item bg-success"><a href="#" class="click-remove" data-tid="'
              + $(this).attr('data-tid') + '">'
              + $(this).attr('data-term')
              + ' <span class="glyphicon glyphicon-remove-circle"></span></a></li>');

          $('.build-container-list').find('li:last-child > a').on('click', function(event){
              killTerm = $(this).attr('data-tid');
              killTarget = $('.taxonomy-switcher [data-tid="' + $(this).attr('data-tid') + '"]').parents('li');
              killTarget.removeClass('bg-success').removeClass('disabled');
              tidArray = dataStore.attr('value').split(',').filter(function(element){
                  return element != killTerm;
              });
              if (tidArray.length == 0) {
                  $('#add-ticket-submit').addClass('disabled');
              }
              dataStore.attr('value', tidArray.join(','));
              $(this).remove();
          })
      });

      $('.build-container-list .click-remove').on('click', function(event) {
          $(this).addClass('danger').removeClass('success');
      })

      $('.ticket-add-cancel').on('click', function(event) {
          modalContent = $(this).parents('.modal-content');
          modalContent.find('.bg-success.disabled').removeClass('bg-success').removeClass('disabled');
          modalContent.find('.data-store').attr('value', null);
          modalContent.find('.build-container-list').contents().remove();
          $('#add-ticket-submit:not(.disabled)').addClass('disabled');
      })

  } //--attach behaviors
  }
})(jQuery);