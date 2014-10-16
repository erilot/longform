/**
 * Technet Whitebranding Javascript
 */

(function($){
    Drupal.behaviors.techNetWhiteBrand = {
	attach: function (context,settings) {
	    
	    //Remove default system logo and links
	    var logoHtml = Drupal.settings.techNetWhiteBrand.imageHtml;
	    var partner = Drupal.settings.techNetWhiteBrand.partner;
	    $('div#logo').contents().remove();
	    $('div#logo').prepend(logoHtml);
	    $('body').addClass('whitebrand').addClass(partner);
	}
    }
})(jQuery)