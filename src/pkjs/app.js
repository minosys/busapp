function getBusID(idLigne){
    var bus = ["", "0001", "0004", "0064", "0014", "0040", "0041", "0035","0050","0032"];
    var index = bus.indexOf(idLigne);
    if(index<0) return 0;
    return index;
}

function fetchBusInfo(busStopID){
	var apikey = "772a6340c271927e34f4a6d624205de69db0a2011828c1230348f894";
 	var busStops = ["1313","1036","2126","2114"];
 	//var busOpposite = ["1292","1004","2127","2115"];
	var requestString = "https://data.explore.star.fr/api/records/1.0/search/?" +
  						 "apikey=" + apikey +
 						 "&dataset=tco-bus-circulation-passages-tr" +
                         "&rows=5" +
                         "&sort=-arrivee" +
                         "&facet=idligne" +
                         "&facet=nomcourtligne" +
                         "&facet=sens" +
                         "&facet=destination" +
                         "&facet=precision" +
                         "&refine.idarret=" + busStops[parseInt(busStopID)] +
                         //"&refine.sens=1"+ //busSens[parseInt(busStopID)] +
                         "&timezone=Europe%2FParis";

  var responseString = "";  
  var req = new XMLHttpRequest();
  req.open('GET', requestString, true);
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        //console.log(req.responseText);
        var response = JSON.parse(req.responseText);
        
        //Fields to send
        response.records.forEach(function(item) {  
            var idLigne = getBusID(item.fields.idligne);
            var busName = item.fields.nomcourtligne;
            var arriveTime =  new Date(item.fields.arrivee).toLocaleTimeString();
        
            if((0 !== responseString.length)) responseString+="|";
            responseString += idLigne + "," + busName + "," + arriveTime; 
        });
        
        console.log(responseString);
        Pebble.sendAppMessage({
          'BUS_STRING_KEY': responseString
        });
        
      } else {
        console.log('Error');
        Pebble.sendAppMessage({
          'ERROR_MESSAGE_KEY': "req.status : " + req.status
        });
      }
    }
  };
  req.send(null);
} 

Pebble.addEventListener('ready', function (e) {
  console.log('connect!' + e.ready);
  console.log(e.type);
});

Pebble.addEventListener('appmessage', function (e) {
  
  console.log(e.type);
  console.log('message received : ' + JSON.stringify(e.payload.BUS_STOP_KEY));
  fetchBusInfo(e.payload.BUS_STOP_KEY);
});

Pebble.addEventListener('webviewclosed', function (e) {
  console.log('webview closed');
  console.log(e.type);
  console.log(e.response);
});
