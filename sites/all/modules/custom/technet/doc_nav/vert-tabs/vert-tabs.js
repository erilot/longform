(function($){
    
$(function () {
    var items = $('#v-nav>ul>li').each(function () {
        $(this).click(function () {
            
            //remove previous class and add it to clicked tab
            items.removeClass('current');
            $(this).addClass('current');

            //hide all content divs and show current one
            $('#v-nav>div.tab-content').hide().eq(items.index($(this))).fadeIn('fast');

        });
    });
    
    $('#v-nav ul li:[tab="tab1"]').click();
});

})(jQuery);
