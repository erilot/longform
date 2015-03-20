/**
 * file: free_view_dynamic_load.js
 *
 * Manages ajax requests for topic loads.
 */

(function ($){
  

  /**
   * Set up variables for scroll velocity reporting
   */
  var el = document.body,
      lastPos = null,
      timer = 0,
      newPos;
  
  function clear() {
    lastPos = null;
  };
  
  function isScrolledIntoView(elem){
    var docViewTop = $(window).scrollTop();
    var docViewBottom = docViewTop + $(window).height();
    var elemTop = $(elem).offset().top;
    var elemBottom = elemTop + $(elem).height();
    return ((elemBottom <= docViewBottom) && (elemTop >= docViewTop));
  }
  
  /**
   * Monitor window scrolling and respond to scrolling events
   *
   *  - checkScrollSpeed(): returns the current velocity
   *
   *  - Watch for previous topic trigger and execute dynamic content load
   *
   *  - Watch for next topic trigger and execute dynamic content load
   */
  function updateOnScroll() {
    delta = checkScrollSpeed(.025);
  }
  
  /**
   * Get the current window scroll speed.
   *
   * @var weight (float)
   *  Return a percentage of the raw value. Use this to
   *  constrain the results to sane numbers. Value between 0 and 1.
   *
   * @return
   *  An integer velocity value
   */
  function checkScrollSpeed(weight){
    newPos = window.scrollY;
    if ( lastPos != null ){ 
      var delta = newPos - lastPos;
        
      // Keep this in local storage for now because it's easy to monitor during development
      var fetchQuantity =  Math.round(delta * weight);
      localStorage.setItem('fetchQuantity', fetchQuantity);
    }
    lastPos = newPos;
    timer && clearTimeout(timer);
    timer = setTimeout(clear, 30);
    return fetchQuantity;
  };

  function preFireCheckList(e){
    console.log(e);
    console.log($(this));
  }
  
  function freeViewAjaxCleanup(e) {
    Drupal.attachBehaviors();
  }
  
  window.onscroll = updateOnScroll;
  
  // ========= Drupal.behaviors
  
  Drupal.behaviors.freeView = {
    attach: function(context,settings){

    
    $('.dynamic-content-wrapper').not('.scroll-processed').waypoint('infinite', {
      container: 'auto',
      items: '.ajax-insert',
      more: '.topic-nav-next',
      offset: 'bottom-in-view',
      loadingClass: 'infinite-loading',
      onBeforePageLoad: preFireCheckList,//$.noop,
      onAfterPageLoad: freeViewAjaxCleanup, //$.noop,//function(e){Drupal.attachBehaviors();console.log('executed');},//$.noop, //$.freeViewAjaxCleanup,
    }).addClass('scroll-processed');
    

    
    
    } // attach
  }  
})(jQuery)