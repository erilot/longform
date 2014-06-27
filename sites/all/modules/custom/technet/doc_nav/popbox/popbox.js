
(function($){
  

(function(){

  $.fn.popbox = function(options){
    var settings = $.extend({
      selector      : this.selector,
      open          : '.openbox',
      box           : '.box',
      arrow         : '.arrow',
      arrow_border  : '.arrow-border',
      close         : '.close'
    }, options);

    var methods = {
      open: function(event){
        event.preventDefault();

        var pop = $(this);
        var box = $(this).parent().find(settings['box']);
        var opl = $(this).parent().find(settings['open']);

        //console.log('box:',box);
        //console.log('opl:',opl);
        
 if(box.css('display') == 'block'){
          methods.close();
        } else {
          box.css({'display': 'block', 'top': 10, 'left': -540});
        }        
        
        box.find(settings['arrow']).css({'left': 600});
        box.find(settings['arrow_border']).css({'left': 600});
      },

      close: function(){
        $(settings['box']).fadeOut("fast");
      },
      
      ok: function(){
        var langCode = $(this).attr("langcode");
        switch (langCode){
          case "de":
            var language = "German";
            break;
          case "es":
            var language = "Spanish";
            break;
          default:
            var language = "Russian";
        }        
                
        $.cookie('google-translate-ok','TRUE');
        $('.popbox').fadeOut("slow");
        $('.popbox').parent('li').remove();
        $('#google_translate_element').removeClass('offscreen');
        
        methods.close();
        //simulate(document.getElementById("google_translate_element"),"click");

        var clickHere = $('.goog-te-menu-frame').contents().find("a.goog-te-menu2-item div span:contains('" + language + "')").closest('a')
        clickHere.attr('id','clickhere');
        alert('Thank you! Please select your language from the drop-down list above.');
        //clickHere.find('.text').css({'border':'1px solid pink'}).attr('id','orHere');
        //console.log ('can you find this??' + clickHere);
        //
        //simulate($('.goog-te-menu-frame').contents().getElementById("orHere"), "click");
        //
        //console.log ('Link to click: ' + document.getElementById('#clickhere'));
        //clickHere.css({'font-weight':'bold'});

      }
    };

    $(document).bind('keyup', function(event){
      if(event.keyCode == 27){
        methods.close();
      }
    });

    $('.accept').bind('click',methods.ok);
    
    $(document).bind('click', function(event){
      if(!$(event.target).closest(settings['selector']).length){
        methods.close();
      }
    });

    return this.each(function(){
      //$(this).css({'width': $(settings['box']).width()}); // Width needs to be set otherwise popbox will not move when window resized.
      $(settings['open'], this).bind('click', methods.open);
      $(settings['open'], this).parent().find(settings['close']).bind('click', function(event){
        event.preventDefault();
        methods.close();
      });
    });
  }

}).call(this);

function simulate(element, eventName)
    {
        var options = extend(defaultOptions, arguments[2] || {});
        var oEvent, eventType = null;

        for (var name in eventMatchers)
        {
            if (eventMatchers[name].test(eventName)) { eventType = name; break; }
        }

        if (!eventType)
            throw new SyntaxError('Only HTMLEvents and MouseEvents interfaces are supported');

        if (document.createEvent)
        {
            oEvent = document.createEvent(eventType);
            if (eventType == 'HTMLEvents')
            {
                oEvent.initEvent(eventName, options.bubbles, options.cancelable);
            }
            else
            {
                oEvent.initMouseEvent(eventName, options.bubbles, options.cancelable, document.defaultView,
          options.button, options.pointerX, options.pointerY, options.pointerX, options.pointerY,
          options.ctrlKey, options.altKey, options.shiftKey, options.metaKey, options.button, element);
            }
            element.dispatchEvent(oEvent);
        }
        else
        {
            options.clientX = options.pointerX;
            options.clientY = options.pointerY;
            var evt = document.createEventObject();
            oEvent = extend(evt, options);
            element.fireEvent('on' + eventName, oEvent);
        }
        return element;
    }

    function extend(destination, source) {
        for (var property in source)
          destination[property] = source[property];
        return destination;
    }

var eventMatchers = {
    'HTMLEvents': /^(?:load|unload|abort|error|select|change|submit|reset|focus|blur|resize|scroll)$/,
    'MouseEvents': /^(?:click|dblclick|mouse(?:down|up|over|move|out))$/
}
var defaultOptions = {
    pointerX: 0,
    pointerY: 0,
    button: 0,
    ctrlKey: false,
    altKey: false,
    shiftKey: false,
    metaKey: false,
    bubbles: true,
    cancelable: true
}

})(jQuery);
