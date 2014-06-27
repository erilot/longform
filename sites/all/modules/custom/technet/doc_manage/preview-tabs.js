(function($){

Drupal.behaviors.docManagePreview = {
    attach: function (context,settings) {

//Tab and content swap action    
$(function () {
    var items = $('#preview-nav ul.preview-tabs>li').each(function () {
        $(this).click(function () {
            //remove previous class and add it to clicked tab
            items.removeClass('current');
            $(this).addClass('current');
            //hide all content divs and show current one
            $('#preview-nav div.preview-section').hide().eq(items.index($(this))).fadeIn('fast');

        });
    });
    
    $('#preview-nav ul li:[tab="tab0"]').click();
});


//// Insert current content into CKEDitor window on click
//
//$('fieldset[id^="#edit-edit"]').addClass('show-editor');
//
//$(function() {
//    var items = $('.preview-contents>fieldset.form-wrapper>.fieldset-wrapper .document-content', context).each(function() {
//        $(this).addClass('active').click(function() {
//            var body=$(this).clone();
//            var baseHeading = $(this).find(':header:first')
//            var thisHeading= baseHeading.text().trim();
//            var thisLevel = baseHeading.attr('level');
//            var sectionId = $(this).attr('id').split('-');
//            var Id = sectionId[1];
//            var topic = $(this).attr('topic');
//                    
//            $('#edit-title-form').attr('value',thisHeading);
//            $('#section-id').attr('value',Id);
//            $('#section-level').attr('value',thisLevel);
//            $('#section-topic').attr('value',$(this).attr('topic'));
//            var ckeditor=$('#cke_contents_edit-content-form-value iframe').contents().find('body');
//            //hide preview title and reveal edit form
//            ckeditor.html(body).find(':header:first').remove();
//            //position editor and reveal
//            topPosition = window.scrollY;
//            $('fieldset[id^="edit-edit"]').css({'top':topPosition}).fadeIn('fast');
//        });
//    });
//});
//
//// Abandon edits on cancel
//$(function(){
//    $('#cancel-content').click(function(){
//        var ckeditor=$('#cke_contents_edit-content-form-value iframe').contents().find('body').html('');
//        $('fieldset[id^="edit-edit"]').fadeOut('fast');
//    })
//});


function updateTab() {
    //update tab field
    var title = $('#edit-title-form').attr('value');
    $('ul.preview-tabs li.tab.current').css({'color':'red'}).html(title);    
}

// Reset elements on update

    //$('input[id^="edit-content-update"]').click(function(){
    //    
    //    console.log($('body'));
    //    var ckeditor=$('#cke_contents_edit-content-form-value iframe').contents().find('body').html('');
    //
    //    $('fieldset[id^="edit-edit"]').fadeOut('fast');
    //    
    //})


    }
};


})(jQuery);
