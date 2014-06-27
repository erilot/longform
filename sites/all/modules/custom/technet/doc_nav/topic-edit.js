/**
 * @file: topic-edit.js
 *
 * Implements live editing and ajax updating of topic content.
 */



(function($){

/**
 * Clear out form styles that have been added during form manipulation
 *
 * @param scope
 * 	The form object to restyle. jQuery object.
 *
 */
 
function clearStyles(scope){
    scope.find('label').removeClass('selected before next after other active btn-warning btn-info disabled').addClass('btn-default');
    scope.find('label.current').removeClass('btn-default').addClass('active btn-success');
}
  
/**
 * Restore styles to their native state. Used when a cancel button is clicked.
 *
 * @param scope
 * 	The form object to restyle. jQuery object.
 *
 */
function resetStyles(scope) {
    scope.find('.workflow-comment').removeClass('in');
    scope.find('label').removeClass('disabled');
    scope.find('label.btn-success').removeClass('btn-default');
    scope.find('label.btn-warning').removeClass('btn-warning').addClass('btn-success').children('input').attr('checked','checked');
    scope.find('label.btn-info').removeClass('btn-info').addClass('btn-default');
}
  
/**
 * Store the original/default radio button state as a data attachment on the parent form.
 *
 * @param scope
 * 	The form to use. jQuery object.
 *
 * @param radios
 * 	The radio buttons to record. jQuery object.
 */
function recordOriginalRadio(scope, radios){
    var checked = scope.find('input[checked="checked"]');
    scope.data('original', checked.attr('value'));
}
  
/**
 * Restore the original/default radio states. Invoked when a cancel button is clicked.
 *
 * @param scope
 * 	The form to use. jQuery object.
 *
 * @param radios
 * 	The radio buttons to restore. jQuery object.
 */
function findOriginalRadio(scope, radios){
    clearStyles(scope);
    radios.each(function(){
	if($(this).attr('value') == scope.data('original')){
	    $(this).attr('checked','checked');
	}
    });
}
  
/**
 * Creat a collection of all radio buttons inputs in a form.
 *
 * @param scope
 * 	The form to use. jQuery object.
 */
function getRadios(scope){
    var radios = scope.find('input[type=radio]');
    return radios;
}
    
/**
 * Insert a cancel button to the workflow edit form.
 *
 * Why is this not just included in the form array server side???
 *
 * @param scope
 *	  The form to use. jQuery object.
 */
function addCancelButton(scope){
    scope.find('.workflow-button-group').prepend('<input type="button" id="edit-cancel-' + scope.attr('id') + '" name="cancel" value="Cancel" class="cancel-button btn btn-danger"/>');
}
  
  
/**
 * Style a form object's radio buttons to appear as bootstrap buttons.
 *
 * @param wfForm
 * 	The form to style. jQuery object.
 *
 * @param mode
 * 	The styling the form should get. Options are 'form-stacked', for a vertical list-group (default colors), or 'default' for horizontal.
 *
 */
function styleForm(wfForm, mode) {
    
    if (!mode) {
	mode = 'default';
    }

    switch(mode) {

	case 'form-stacked':
	    // Vertical arrangement of radio elements, modified for bootstrap
	    wfForm.find('input[checked="checked"]').parent('label').addClass('active current btn-success').removeClass('btn-primary');
      
	    wfForm.find('input[type="radio"]').on('click',function(){
	        $(this).parent('label').removeClass('btn-default').addClass('btn-success');
	    });
      
	    break;
  
	default:
	wfForm.find('input[checked="checked"]').parent('label').addClass('active current btn-success').removeClass('btn-info');
      
	wfForm.find('input[type="radio"]').on('click',function(){
	    $(this).parent('label').removeClass('btn-info').addClass('btn-success');
	});
	break;
    }
    wfForm.addClass('form-styled form-styled-' + mode);
}
  
/**
 * Process workflow state edit forms and bind click events to ajax handlers/submitters
 *
 * @param scope
 * 	The form to process. jQuery object.
 *
 * @param context
 * 	Drupal's context object.
 * 	
 */
function processForm(scope, context) {
  
    if (scope.length == 0 || scope.hasClass('form-processed')) {
	return;
    }
    
    var radios = getRadios(scope);
    var checked = scope.find('input[checked="checked"]');
  
    recordOriginalRadio(scope, radios);    
    
    scope.find('button[id^="workflow-submit"]').wrapAll('<div class="workflow-button-group panel-footer"></div>');
    addCancelButton(scope);
  
    scope.find('.workflow-button-group').wrapInner('<div class="btn-group"/>');
    scope.find('.workflow-button-group').appendTo($(scope).find('.workflow-comment'));
  
    /*radio click functions*/
    radios.parent('label').on('click', function() {
	
	// Reveal confirmation and comment form when an unselected radio is clicked
	if(!$(this).hasClass('current') && !$(this).hasClass('disabled')){
      	  
	    scope.find('.workflow-comment').addClass('in');
	  
	    // Restore everything if 'cancel' button is clicked.
	    scope.find('.cancel-button').on('click', function(){
		findOriginalRadio(scope, radios);
		scope.find('.workflow-comment').removeClass('in');
	    });
	    scope.find('input[id^="workflow-submit"]').on('click', function() {
		scope.find('label.current').children('input').attr('checked','checked');
	    });
	};
    });
  
  scope.addClass('form-processed');

}

/**
 * Move a workflow form into it's final UI location.
 *
 * Workflow forms are deposited in a big pile at the end of the page; this scoops them up and
 * drops them into their menu positions.
 *
 * @param scope
 * 	The form object to move. jQuery object.
 *
 * @param context
 * 	Drupal's context object.
 */
function moveWorkflowForm(scope, context) {   
    
    var selector = '.topic-set[data-topic-target="' + $(scope).attr('topic') + '"]';
    var formClass = 'topic-workflow-form-' + $(scope).attr('topic');
    
    var wfForm = $(selector).parent('div').parent('form').addClass(formClass);
    
    destinationWrapper = $('div[topic="' + $(scope).attr('topic') + '"]');
    destinationWrapper.parents('.topic-wrapper').find('.workflow-insert').html(wfForm);

}

/**
 * Add a color-coded stripe on the left side of each topic, signifying its workflow state.
 *
 * @param topics
 * 	A collection of topic objects to stripe. jQuery object.
 */
function stripeTopics (topics) {
    topics.each(function() {
	var topic = $(this);
        var selector = '.topic-set[data-topic-target="' + topic.attr('topic') + '"]';

	currentState = $(selector).attr('data-current');
	topic.find('.workflow-indicator').remove().end().prepend('<span class="workflow-indicator"></span>').find('.workflow-indicator').addClass('workflow-state-id-' + currentState);
	//topic.find('.workflow-indicator').tooltip();
    })
}

function collapseEditMenus(wrappers) {
    wrappers.each(function(){
	var topicBlock = $(this).find('.document-content');
	var editBlock = $(this).find('.edit-wrapper');
	
	if (topicBlock.height() < editBlock.height()) {
	    editBlock.children().addClass('vertical-collapse');
	}
	else {
	    editBlock.find('.vertical-collapse-trigger').remove();
	}
    })

}

function paintAlerts() {

    $('#doc-content .edit-wrapper .vertical-collapse-trigger').removeClass('alert');
    $('#doc-content .flag.unflag-action').parents('.edit-wrapper').find('.vertical-collapse-trigger').addClass('alert');  
}

//================


Drupal.behaviors.booknav = {
attach: function(context,settings){
    contextObject = $(context.context);
    ajaxSafety = false;    
    var content = $('#doc-content');
    
 if (context == document) {
    
    // Insert edit options and wrappers. "insert" comes from book_nav_page_alter(). Which in hindsight makes no sense.
    content.find('.document-content:not(.edit-setup-processed)').each(function(index, item){
	
	var insert = Drupal.settings.book_nav.insert;
	var topicWrapper = $(this).addClass('edit-setup-processed').wrapAll('<div class="topic-wrapper" id="topic-wrapper-' + index + '"/>').parent('.topic-wrapper').prepend(insert);
	topicWrapper.find('.edit-wrapper').on('hover',function() {
	    $(this).parent('.topic-wrapper').toggleClass('highlighted');
	})
	
    });
   
    // Move the form into its modal home

    $('#book-nav-edit-form-create:not(.form-modal-processed)')
	.addClass('form-modal-processed')
	.wrapAll('<div class="modal fade form-group edit-form-modal"><div class="modal-dialog"><div class="modal-content form-inline"><div class="modal-body form-inline></div></div></div></div>');
    $('.edit-form-modal:not(.edit-form-modal-processed) .modal-content', context)
	.prepend('<div class="modal-header"><h5 class="modal-title"></h5></div>')
	.parents('.edit-form-modal').prependTo('body').addClass('edit-form-modal-processed');

    // Brand the div we're going to replace
    $('#doc-content .field-name-body .field-item').attr('id','replace-this');

    // On 'edit' click, copy content into the edit form and display the modal
    $('.ajax-op-button:not(.ajax-in-process)').bind('click', function(){
	
	$(this).addClass('ajax-in-process');
	var mode = $(this).attr('data-mode');
	var topic = $(this).parents('.topic-wrapper').find('.document-content').attr('topic');
	
	if (topic == null) {
	    alert('This document is too old to edit and update topics or workflows. Use the page-level edit features instead.');
	    return;
	}
	// Load hidden data on form
	$('#edit-content-update').attr('edit-topic', topic);
	$("#section-topic").val(topic);
	$('#section-weight').val($(this).parents('.topic-wrapper').attr('id'));
	
	var fetchMode = 'local'; //'local' fetches the content from the current page; 'ajax' fetches it from the database.
	
	switch (fetchMode) {
	    case 'local':
		
		/**
		 * NOTE: This method preserves page-level cross-reference links, but
		 * overwrites and destroys the root topic's topic-level links.
		 *
		 * DO NOT USE THIS IN CONJUNCTION WITH TOPIC-BASED PUBLISHING until this is resolved.
		 */
		var selectedTopic = $('.document-content[topic="' + topic + '"]');
		var thisTitle = selectedTopic.find('[class^="heading-level"], .subtitle').text();
		if (thisTitle.length == 0) {
		    thisTitle = $('.book-node-title').text().trim();
		}
		var thisLevel = selectedTopic.attr('level');
		var thisBody = selectedTopic.clone().find('.workflow-indicator, h1, h2, h3, div[data-unique], .alert').remove().end().collapseElement('expand').html();
		// Populate edit form values
		$('#edit-title-form').attr('value', thisTitle);
		$('#edit-heading-form').attr('value', thisLevel);

		
		// Insert selected topic into ckeditor window and reveal form

		for(var id in CKEDITOR.instances) {CKEDITOR.instances[id].setData(thisBody);}		
		$('body .edit-form-modal .modal-dialog').addClass('modal-xl').parent().modal('show');
		
		break;
	    
	    case 'ajax':

		/**
		 * NOTE: AJAX METHOD IS NOT FULLY FUNCTIONAL YET
		 *
		 * There is a significant bug in handling ajax.done() that doesn't dump old content or re-attach
		 * click handlers.
		 *
		 * Additionally, fetching the topic from the database will break cross-reference links at the page level
		 * until a more robust link handling system is implemented.
		 */
		
		// Set up ajax call
		if (!window.location.origin) window.location.origin = window.location.protocol+"//"+window.location.host;
		var requestUrl = window.location.origin + Drupal.settings.book_nav.basePath + 'ajax/topic-live-edit/' + topic + '/' + mode;
		
	
		var editModalRequest = $.ajax({
		    url: requestUrl,
		    context: '.edit-form-modal',
		    global: false,
		    });
		
		editModalRequest.done(function(response) {
		    topic = jQuery.parseJSON(response[1].data);
		    switch (topic.response) {
			
			case 'success':
		
			    var topicBody = $().add('<div></div>').prepend(topic.result.body).find('.document-content').contents();
			    
			    // Populate edit form values
			    $('#edit-title-form').attr('value', topic.result.title);
			    $('#edit-heading-form').attr('value', topic.result.level);
			    $('#cke_edit-content-form-value iframe').contents().find('body').html(topicBody);
			    // Insert selected topic into ckeditor window and reveal form
			    ckeditor.contents().html(topicBody);
			    $('body .edit-form-modal .modal-dialog').addClass('modal-xl').parent().modal('show');
			    
			    break;
				    
			default:
			    var errorModal = '<div class="modal fade ajax-error"><div class="modal-dialog modal-error"><div class="modal-content">'
			    errorModal+='<div class="modal-header"><h5 class="modal-title">' + topic.title + '</h5></div>'
			    errorModal+='<div class="modal-content"><div class="modal-body">' + topic.result + '</div></div>'
			    errorModal+='<div class="modal-footer"><button class="btn btn-default pull-right modal-error-close" data-dismiss="modal">Close</div></div></div></div>';
			    $(errorModal).prependTo('body').modal('show');
			    break;
			} //switch
		});
		break;
	} //switch
	
	$(document).ajaxStop(function(){
	    $('.edit-form-modal').modal('hide');
	    $('.ajax-in-process').removeClass('ajax-in-process').addClass('ajax-process-complete');
	    });    
	    
	});

    // Abandon edits on cancel
	$('#cancel-content', context).on('click', function(){
	    $('#cke_contents_edit-content-form-value iframe').contents().find('body').html('');
	    $('#edit-content-update').attr('edit-topic', -1);
	    $('#section-topic').attr('value', -1);
	    $('.ajax-in-process').removeClass('ajax-in-process');
	    $('.edit-form-modal').modal('hide');
	})
    }

    
    // === Workflow report ===
    
    // Move workflow forms into the flyin nav
    if (context == document) {
	
	styleForm($('#shift-workflow'), 'default');
	processForm($('#shift-workflow'), context);
	
	workflowForms = $('form[id^="book-nav-workflow-form-create"]');
	
	
	var pageTopics = $('.document-content');
	
	pageTopics.each(function(){
	   thisTopic = $(this);
	   thisEditMenu = thisTopic.parent('.topic-wrapper').find('.edit-wrapper .workflow-edit-button');
	   thisID = thisTopic.attr('topic');
	   thisForm = $('div[data-topic-target="' + thisID + '"]');
	   thisFlag = thisForm.nextUntil(':not(.flag-wrapper)').insertBefore(thisEditMenu);
	});
	
	pageTopics.parents('.topic-wrapper').children('.edit-wrapper').once('hover', function() {
	    var rootTopic = $(this).parent().find('.document-content');

	    moveWorkflowForm(rootTopic, context);
	    styleForm($(this).find('form:not(.form-styled)'),'form-stacked')
	    processForm($(this).find('form:not(.form-processed)'), context);	    
	    
	});
	stripeTopics($('.document-content'));
	
	collapseEditMenus($('.topic-wrapper'));
    }
    else {
	var contextString = $(context).attr('id');


    	if (contextString && (contextString.indexOf('book-nav-workflow-form-create') != -1 || contextString == 'shift-workflow')) {
	    var element = $(context);

	    // Style form	    
	    mode = 'default';
	    if (element.find('.topic-set').hasClass('form-stacked')) {
		mode = 'form-stacked';
	    }
	  
	    
	    if (!element.hasClass('form-styled')) {
		styleForm(element, mode);

	    }
	    if (!element.hasClass('form-processed')) {
		processForm(element, context);
	    }
	    
	    var topicID = element.find('.topic-set').attr('data-topic-target');
	    var topicTarget = $('div[topic="' + topicID + '"]');
	    
	    topicTarget.parents('.topic-wrapper').removeClass('highlighted');
	    
	}
	else {
	}
    }
        paintAlerts();

}

    
}
})(jQuery)