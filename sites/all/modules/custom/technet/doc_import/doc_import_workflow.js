(function ($){
  
Drupal.behaviors.docImport= {
attach: function(context,settings){
  console.log('executing');
  function clearStyles(scope){
    scope.find('label').removeClass('selected before next after other active btn-warning btn-info disabled').addClass('btn-default');
    scope.find('label.current').removeClass('btn-default').addClass('active btn-success');
  }
  
  function resetStyles(scope) {
    scope.find('label').removeClass('disabled');
    scope.find('label.btn-success').removeClass('btn-default');
    scope.find('label.btn-warning').removeClass('btn-warning').addClass('btn-success').children('input').attr('checked','checked');
    scope.find('label.btn-info').removeClass('btn-info').addClass('btn-default');
  }
  
  function recordOriginalRadio(scope, radios){
    var checked = scope.find('input[checked="checked"]');
    scope.data('original', checked.attr('value'));
  }
  
  function findOriginalRadio(scope, radios){
    clearStyles(scope);
    radios.each(function(){
      if($(this).attr('value') == scope.data('original')){
        $(this).attr('checked','checked');
      }
    });
  }
  
  function getRadios(scope){
    var radios = scope.find('input[type=radio]');
    return radios;
  }
  
  function showComment(scope){
    
    scope.find('.workflow-comment').fadeIn('fast');
  }
  
  function hideComment(scope){
    scope.find('.workflow-comment').fadeOut('fast');
  }
    
  function addCancelButton(scope){
    scope.find('.workflow-button-group').prepend('<input type="button" id="edit-cancel-' + scope.attr('id') + '" name="cancel" value="Cancel" class="cancel-button btn btn-danger"/>');
  }
  
  function styleForm(wfForm, mode) {
    if (!mode) {
      mode = 'default';
    }

    switch(mode) {

      case 'form-stacked':

        wfForm.find('.form-radios .workflow-comment').remove();
        wfForm.find('input[checked="checked"]').parent('label').addClass('active current btn-success').removeClass('btn-primary');
        
        wfForm.find('input[type="radio"]').on('click',function(){
          $(this).parent('label').removeClass('btn-default').addClass('btn-success');
        });
        
        break;
    
      default:

        wfForm.find('.workflow-comment').remove();
        wfForm.find('input[checked="checked"]').parent('label').addClass('active current btn-success').removeClass('btn-primary');
        
        wfForm.find('input[type="radio"]').on('click',function(){
          $(this).parent('label').removeClass('btn-default').addClass('btn-success');
        });
        break;
    }
  }
  
  function stripeTopics (topics) {
    
    topics.each(function() {
      topic = $(this);
      currentState = topic.parents('.topic-wrapper').find('.topic-set').attr('data-current');
      //topic.find('.workflow-indicator').remove().end().prepend('<span class="workflow-indicator"></span>').find('.workflow-indicator').addClass('workflow-state-id-' + currentState);

    })
  }
  

    
    workflowForms = $('form[id^="book-nav-workflow-form-create"]');
    
    workflowForms.each(function() {
      var scope = $(this);
      
      // Style form
      
      var mode = 'default';
      
      if (scope.find('.topic-set').hasClass('form-stacked')) {
        mode = 'form-stacked';
      }
      styleForm(scope, mode);
            
      var radios = getRadios(scope);
      var checked = scope.find('input[checked="checked"]');

      
      recordOriginalRadio(scope, radios);    
        
      scope.find('input[id^="workflow-submit"]').wrapAll('<div class="workflow-button-group panel-footer"></div>');
      addCancelButton(scope);
      
      //scope.find('.workflow-button-group').wrapInner('<div class="btn-group"/>');
    
      scope.find('.workflow-comment').appendTo(scope.find('.form-radios'));
      scope.find('.workflow-button-group').appendTo($(scope).find('.workflow-comment'));
    
      /*radio click functions*/
      radios.parent('label').on('click', function() {
      
        //do this only if the click is on a new radio button (not the currently selected one)
        if(!$(this).hasClass('current') && !$(this).hasClass('btn-info')){
          
          var label = $(this).text();
          $(radios).parent('label').removeClass('btn-default').addClass('btn-info disabled');
          $(this).addClass('btn-warning').removeClass('btn-default');
          
          scope.find('.workflow-comment').addClass('in');
          
          //update message area when clicked to cancel update
          scope.find('.cancel-button').on('click', function(){
            clearStyles(scope);
            findOriginalRadio(scope, radios);
            scope.find('.workflow-comment').removeClass('in');
          });
          scope.find('input[id^="workflow-submit"]').on('click', function() {
              $('label.current').child('input').attr('checked','checked');
          });
        };
      });
  
      //ajax complete function
      $(document).ajaxComplete(function(){
  
        var radios = getRadios(scope);
        hideComment(scope);
        resetStyles(scope);
        recordOriginalRadio(scope, radios);
        stripeTopics($('.document-content'));

      });
  
      var topicID = scope.find('.topic-set').attr('data-topic-target');
      
      destinationWrapper = $('div[topic="' + topicID + '"]');
      destinationWrapper.parents('.topic-wrapper').find('.workflow-insert').html(scope);
  
    });
      stripeTopics($('.document-content'));

}

}
})(jQuery)