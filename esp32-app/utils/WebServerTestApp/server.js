/**
 * @summary LedClock development web server
 * @author luk6xff
 * @date   2020-11-18
 */

const PORT = 8081;
const GZIP_COMPRESSION_ENABLED = false;

if (GZIP_COMPRESSION_ENABLED) {
   const compression = require('compression');
}

const express = require('express');
const app = express();


app.use(express.json())

if (GZIP_COMPRESSION_ENABLED) {
   app.use(compression({ threshold: 0 }))
   app.use(express.static('../../data'));
}
else {

   app.use(express.static('../../src/WebServer/data'));
}




app.get('/', function (req, res) {
   res.send('Hello World');
})


// GET
app.get('/dev-app-sysinfo', function (req, res) {
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


// Cfg save
app.post('/dev-cfg-save', function (req, res) {
    console.log("Got a POST dev-cfg-save request for: " + req);
    const resp = "{\"status\":\"error\"}";
    res.status(200).send(resp);
});

// Cfg read
app.get('/dev-cfg-read', function (req, res) {
   console.log("Got a dev-cfg-read request: " + req.query.cfg);
   if (req.query.cfg == "dev-cfg-wifi") {
      const resp = '{"dev-cfg-wifi":[{"wifi-ssid":"INTEHNETOWO"},{"wifi-pass":"Faza19999"},{"wifi-ap-hostname":"ledclock"},{"wifi-ap-pass":"admin123"}]}';
      res.status(200).send(resp);
   }
   else if (req.query.cfg == "dev-cfg-time") {
      const resp = '{"dev-cfg-time":[{"time-date":"2020-11-20"},{"time-clock":"18:00:00"},{"time-timezone-1":"[CET|0|1|10|3|60]"},{"time-timezone-2":"[CEST|0|1|3|2|120]"},{"time-ntp-time-offset":"0"},{"time-ntp-sync-interval":"60"},{"time-ntp-enable":0},{"time-timezone-num":"2"}]}';
      res.status(200).send(resp);
   }
   else if (req.query.cfg == "dev-cfg-weather") {
      const resp = '{"dev-cfg-weather":[{"weather-sync-interval":"99"},{"weather-owm-api-key":"INVALID"},{"weather-owm-city":"[Ubiad|49.692167|20.708694]"},{"weather-owm-lang":"pl"},{"weather-update-enable":0}]}';
      res.status(200).send(resp);
   }
   else if (req.query.cfg == "dev-cfg-radio") {
      const resp = '{"dev-cfg-radio":[{"radio-update-interval":"99"},{"radio-crit-vbatt":"3300"}]}';
      res.status(200).send(resp);
   }
   else if (req.query.cfg == "dev-cfg-display") {
      const resp = '{"dev-cfg-display":[{"display-intensity-val":"99"},{"display-anim-speed":"70"},{"display-auto-intensity":false},{"display-time-format":"0"}]}';
      res.status(200).send(resp);
   }
   else if (req.query.cfg == "dev-cfg-app") {
      const resp = '{"dev-cfg-app":[{"app-lang":"0"}]}';
      res.status(200).send(resp);
   }
   else {
      res.status(404).send('{"dev-cfg-wifi": "failure"}');
   }
});

// App restart
app.get('/dev-app-reset', function (req, res) {
   res.status(200).send("App is being restarted success");
});

// App print some text
app.post('/dev-app-print-text', function (req, res) {
   console.log("Got a dev-app-print-text request for: " + JSON.stringify(req.body));
   const resp = '{"status":"success"}';
   res.status(200).send(resp);
});

// App set date and time
app.post('/dev-app-set-dt', function (req, res) {
   console.log("Got a dev-app-set-dt request for: " + JSON.stringify(req.body));
   const resp = '{"dev-app-set-dt":"success"}';
   res.status(200).send(resp);
});

// App set date and time
app.get('/dev-app-get-dt', function (req, res) {
   console.log("Got a dev-app-get-dt request");
   var dt = new Date();
   dt = Math.floor(dt.valueOf() / 1000);
   console.log(dt);
   res.type('application/json') // => 'application/json'
   const resp = '{"dt":"' + dt + '"}';
   res.status(200).send(resp);
});


// Ota update
app.post('/update', function (req, res) {
   console.log("Got a POST request for: " + req.baseUrl);
   res.status(200).send('OTA started...');
})




// SERVER start
var server = app.listen(PORT, function () {
   var host = server.address().address
   var port = server.address().port
   console.log("LED-CLOCK server listening at http://%s:%s", host, port);
})