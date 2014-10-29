(function ($){
Drupal.behaviors.longformDashboard = {
attach: function(context,settings){
		// Listen for bootstrap dashboard "show" events and dynamically fetch content for them.
		loading = '<div class="progress-bar progress-bar-info progress-bar-striped active placeholder-content" style="width:100%;">Loading pane...</div>';
		$('a[data-toggle="pill"]', context).on('show.bs.tab', function (e) {
				id = $(e.target).attr('data-id');
				target = $('#' + id + ' .dashboard-pane-inner', context);
				thisDashboard = Drupal.settings.longformDashboard[id];
        //target.prepend(loading);
				if (thisDashboard === undefined || thisDashboard['view'] === undefined || thisDashboard['display'] === undefined) {
						target.prepend('<div class="alert alert-danger">There isn\'t any view information to show here!</div>');
				}
				else {
						// Remove existing contents and insert the "loading" message, then build the ajax url and get the view
						target.parents('.tab-pane').find('.placeholder').remove();
						target.contents().remove().end().append(loading);
						url = 'jsondata/views/' + thisDashboard['view'] + '/' + thisDashboard['display'];
						if (thisDashboard['args'] !== null && thisDashboard['args'].length > 0) {
								url += '/' + thisDashboard['args'].join('/');
						}
						$.ajax({
								url:  url,
								cache: false
						})
						.done(function(html) {
								Drupal.detachBehaviors(target);
								target.contents().remove().end().append(html);
								//target.prepend('<h3>' + $(e.target).html() + '</h3>');
								Drupal.attachBehaviors(target.children());
						})
              .fail(function(jqXHR, textStatus, errorThrown) {
                errorMessage = '<div class="alert alert-danger"><label>Well, <i>that</i> didn\'t work.</label><br/>' + textStatus + ': <b>' + errorThrown + '</b></div>';
                target.contents().remove().end().append(errorMessage);
              });
				}
		})
		
}}})(jQuery)