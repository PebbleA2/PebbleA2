function getMessage() {
	responseText = '{"message": "PebbleA2 rocks!!! :D"}';

	// responseText is hard-coded for now
	var json = JSON.parse(responseText);
	console.log(responseText); // JSON.stringify() not necessary!

	// TODO: error checking???
	var message = json.message;
	console.log('message is "' + message + '"');

	// Assemble dictionary using our keys
	var dictionary = {
		"message": message
	};

	// Send to Pebble
	Pebble.sendAppMessage(dictionary,
		function(e) {
			console.log("AppMessage sent to Pebble successfully!");
		},
		function(e) {
			console.log("Error sending AppMessage to Pebble!");
		}
	);
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');

    // Get the message
    getMessage();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    getMessage();
  }                     
);