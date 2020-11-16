var express = require('express');
var app = express();
const PORT = 8081;

app.use(express.static('../../data'));


app.get('/', function (req, res) {
   res.send('Hello World');
})


// GET
app.get('/devinfo', function (req, res) {
    res.json([{"name":"TEST SERVER APP","value":"TEST_SERVER","unit":"","glyph":"glyphicon-scale"},
                {"name":"Build author","value":"luk6xff","unit":"","glyph":"glyphicon-scale"},
                {"name":"Build date","value":"Nov 11 2020","unit":"","glyph":"glyphicon-scale"},
                {"name":"Build time","value":"14:36:35","unit":"","glyph":"glyphicon-scale"},
                {"name":"Chip ID","value":"1","unit":"","glyph":"glyphicon-scale"},
                {"name":"SDKversion","value":"v3.2.3-14-gd3e562907","unit":"","glyph":"glyphicon-scale"},
                {"name":"CPU Freq","value":"240","unit":"MHz","glyph":"glyphicon-scale"},
                {"name":"Chip ID","value":"4194304","unit":"","glyph":"glyphicon-scale"},
                {"name":"Application size","value":"1023136","unit":"Bytes","glyph":"glyphicon-scale"}]);
    console.log("devinfo request handled");
 })


// POST
app.post('/dev-cfg-save', function (req, res) {
    console.log("Got a POST dev-cfg-save request for: " + req);
    res.send('Hello POST');
})

// GET
app.get('/dev-cfg-read', function (req, res) {
   console.log("Got a dev-cfg-read request: " + req.query.cfg);
   if (req.query.cfg == "dev-cfg-wifi") {
      const resp = '{"dev-cfg-wifi":[{"wifi-ssid":"INTEHNETOWO"},{"wifi-pass":"Faza19999"},{"wifi-ap-hostname":"ledclock"},{"wifi-ap-pass":"admin123"}]}';
      res.status(200).send(JSON.stringify(resp));
   }
   else if (req.query.cfg == "dev-cfg-time") {
      const resp = '{"dev-cfg-time":[{"time-date":"2020-11-20"},{"time-clock":"18:00:00"},{"time-timezone-1":"[CET|0|1|10|3|60]"},{"time-timezone-2":"[CEST|0|1|3|2|120]"},{"time-ntp-time-offset":"0"},{"time-ntp-sync-interval":"60"},{"time-ntp-enable":0},{"time-timezone-num":"2"}]}';
      res.status(200).send(JSON.stringify(resp));
   }
   else if (req.query.cfg == "dev-cfg-weather") {
      const resp = '{"dev-cfg-weather":[{"weather-sync-interval":"99"},{"weather-owm-api-key":"INVALID"},{"weather-owm-city":"[Ubiad|49.692167|20.708694]"},{"weather-owm-lang":"pl"},{"weather-update-enable":0}]}';
      res.status(200).send(JSON.stringify(resp));
   }
   else if (req.query.cfg == "dev-cfg-radio") {
      const resp = '{"dev-cfg-radio":[{"radio-update-interval":"99"},{"radio-crit-vbatt":"3300"}]}';
      res.status(200).send(JSON.stringify(resp));
   }
   else if (req.query.cfg == "dev-cfg-display") {
      const resp = '{"dev-cfg-display":[{"display-intensity-val":"99"},{"display-anim-speed":"70"},{"display-auto-intensity":false},{"display-time-format":"0"}]}';
      res.status(200).send(JSON.stringify(resp));
   }
   else {
      res.status(404).send('{"dev-cfg-wifi": "failure"}');
   }
})


app.post('/update', function (req, res) {
   console.log("Got a POST request for: " + req.baseUrl);
   res.status(200).send('OTA started...');
})


// SERVER
var server = app.listen(PORT, function () {
   var host = server.address().address
   var port = server.address().port
   console.log("LED-CLOCK server listening at http://%s:%s", host, port);
})