function getBusID(idLigne){
    var bus = ["", "0001", "0004", "0064", "0014", "0040", "0041", "0035"];
    var index = bus.indexOf(idLigne);
    if(index<0) return 0;
    return index;
}

function fetchBusInfo(){
  var requestString = "https://data.explore.star.fr/api/records/1.0/search/?dataset=tco-bus-circulation-passages-tr" +
                         "&rows=1" +
                         "&sort=-arrivee" +
                         "&facet=idligne" +
                         "&facet=nomcourtligne" +
                         "&facet=sens" +
                         "&facet=destination" +
                         "&facet=precision" +
                         "&refine.nomarret=Beaulieu+Restau+U" +
                         "&refine.sens=1" +
                         "&timezone=Europe%2FParis";
    
  var req = new XMLHttpRequest();
  req.open('GET', requestString, true);
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        console.log(req.responseText);
        var response = JSON.parse(req.responseText);
        
        //Fields to send
        var idLigne = getBusID(response.records[0].fields.idligne);
        var arriveTime =  new Date(response.records[0].fields.arrivee).toLocaleTimeString();
        var busName = response.records[0].fields.nomcourtligne;
        
        console.log(response);
        Pebble.sendAppMessage({
          'BUS_ICON_KEY': idLigne,
          'BUS_NAME_KEY': busName,
          'ARRIVE_TIME_KEY': arriveTime
        });
      } else {
        console.log('Error');
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
  fetchBusInfo();
  console.log(e.type);
  console.log(e.payload.busDatakey);
  console.log('message!');
});

Pebble.addEventListener('webviewclosed', function (e) {
  console.log('webview closed');
  console.log(e.type);
  console.log(e.response);
});
