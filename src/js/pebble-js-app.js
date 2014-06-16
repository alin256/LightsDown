//var addressHUE = "http://192.168.1.52/api/newdeveloper/lights";

function changeLight(bri, hue, sat) {
  var req = new XMLHttpRequest();
  req.open('PUT', 'http://192.168.1.52/api/newdeveloper/lights/2/state', true);
  req.onreadystatechange = function(e) 
    {
      //console.log("Received Status: " + e.payload.status);
      if (req.readyState == 4) {
        if(req.status == 200) {
          console.log(JSON.stringify(JSON.parse(req.responseText), null, '\t'));
        }
      } 
      else {
        console.log("Error");
      }
    };
  //http.send(document.commandform.messagebody.value);

  //req.send("{\"on\": true}");
  console.log("{" + "\"bri\":" + bri + "," +"\"hue\":" + hue*256 + "," +"\"sat\":" + sat  + "}");
  req.send("{" + "\"bri\":" + bri + "," +"\"hue\":" + hue*256 + "," +"\"sat\":" + sat + ", \"transitiontime\": 3}");
}


// Function to send a message to the Pebble using AppMessage API
function sendMessage() {
  changeLight();
	Pebble.sendAppMessage({"status": 0});
	
	// PRO TIP: If you are sending more than one message, or a complex set of messages, 
	// it is important that you setup an ackHandler and a nackHandler and call 
	// Pebble.sendAppMessage({ /* Message here */ }, ackHandler, nackHandler), which 
	// will designate the ackHandler and nackHandler that will be called upon the Pebble 
	// ack-ing or nack-ing the message you just sent. The specified nackHandler will 
	// also be called if your message send attempt times out.
}



// Called when JS is ready
Pebble.addEventListener("ready",
							function(e) {
							});
												
// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
							function(e) {
								console.log("Received Status: " + e.payload.status);
								console.log("Received Bri: " + e.payload.bri);
								console.log("Received Hue: " + e.payload.hue);
								console.log("Received Sat: " + e.payload.sat);
                changeLight(e.payload.bri,e.payload.hue,e.payload.sat);
								//sendMessage();
							});