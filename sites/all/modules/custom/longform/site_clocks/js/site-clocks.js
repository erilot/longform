// site-clocks.js
//
// Adds site clocks

(function ($){
  
 $(document).ready(function(){
    
 var clocks = $('.clock');

 //console.log(clocks);
    
    clocks.each(function(index,item){
	var e = $(item);
	var offset = e.attr('start');
		
	var clock = new StationClock(e.attr('id'), offset);
	
	clock.body = StationClock.RoundBody;
	clock.dial = StationClock.GermanStrokeDial;
	clock.hourHand = StationClock.PointedHourHand;
	clock.minuteHand = StationClock.PointedMinuteHand;
	clock.secondHand = StationClock.BarSecondHand;
	clock.boss = StationClock.NoBoss;
	clock.minuteHandBehavoir = StationClock.BouncingMinuteHand;
	clock.secondHandBehavoir = StationClock.OverhastySecondHand;
	clock.offset = offset;
	
	//console.log(clock);
	window.setInterval(function() { clock.draw(offset) }, 50);

    })
 });
		   
    
})(jQuery);