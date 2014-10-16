(function ($)
{

    /*
    
    highlight v4
    
    Highlights arbitrary terms.
    
    <http://johannburkard.de/blog/programming/javascript/highlight-javascript-text-higlighting-jquery-plugin.html>
    
    MIT license.
    
    Johann Burkard
    <http://johannburkard.de>
    <mailto:jb@eaio.com>
    
    */
  jQuery.fn.highlight=function(c){function e(b,c){var d=0;if(3==b.nodeType){var a=b.data.toUpperCase().indexOf(c);if(0<=a){d=document.createElement("span");d.className="search-keys";a=b.splitText(a);a.splitText(c.length);var f=a.cloneNode(!0);d.appendChild(f);a.parentNode.replaceChild(d,a);d=1}}else if(1==b.nodeType&&b.childNodes&&!/(script|style)/i.test(b.tagName))for(a=0;a<b.childNodes.length;++a)a+=e(b.childNodes[a],c);return d}return this.length&&c&&c.length?this.each(function(){e(this,c.toUpperCase())}): this};jQuery.fn.removeHighlight=function(){return this.find("span.search-keys").each(function(){this.parentNode.firstChild.nodeName;with(this.parentNode)replaceChild(this.firstChild,this),normalize()}).end()};
  
    $(document).ready(function (){
        //Acquire search keys from Drupal
        //@TODO: Implement this, and not the hacky PHP version which corrupts URLs
        if(typeof Drupal.settings.document_search !='undefined'){
            var keys = Drupal.settings.document_search.keys;
            console.log(keys);
            keys.forEach(function(element, index, array){
                $('#content').highlight(element);
              });
            
            //if there are flagged search terms on the page
              //add 'click to remove' div and bind action
              $('.title-block-fixed').append('<div id="clear-search"><p><span class="glyphicon glyphicon-remove-circle"></span> Click to clear highlighting</p></div>');
              $('#clear-search').bind('click',function(){
                $('#content').removeHighlight();
                $('#clear-search').slideUp(200);
              })
              
              //if any terms are inside collapsed procedures/tables, open them
              $('.procedure-wrapper.collapse ol').each(function(){
                if($(this).find('.search-keys').length != 0){
                    $(this).parent('.procedure-wrapper').collapse('show').prev('button').removeClass('collapsed');
                };
            });
            
            $('.table-wrapper.collapse table').each(function(){
                if($(this).find('.search-keys').length != 0){
                    $(this).parent('.table-wrapper').collapse('show').prev('button').removeClass('collapsed');
                };
            });
              
            //scroll to first instance of search term
            setTimeout(function (){
              $('html,body').stop().animate({
                scrollTop: ($('.search-keys').offset().top) - ($('.title-block.fixed').outerHeight() + 50)
              }, 500);
            }, 300)
        }
    });
})(jQuery)
