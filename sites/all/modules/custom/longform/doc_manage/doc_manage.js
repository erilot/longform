(function($){

Drupal.behaviors.docManage = {
    attach: function(context,settings){
	
    // Activate popovers
    
    var popovers = $('[data-toggle="popover"]').each(function(){
	var pop = $(this);
	pop.popover({
	    content: 'this is a test',
	    container: '#edit-import',
	    trigger: 'hover',
	});
    })
    
    }
}
})(jQuery)
