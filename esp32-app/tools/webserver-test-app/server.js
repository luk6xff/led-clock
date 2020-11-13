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
app.post('/dev-cfg-*', function (req, res) {
    console.log("Got a POST request for: " + req.body);
    res.send('Hello POST');
})


app.post('/update', function (req, res) {
   console.log("Got a POST request for: " + req.baseUrl);
   res.send('OTA started...');
})


// SERVER
var server = app.listen(PORT, function () {
   var host = server.address().address
   var port = server.address().port
   console.log("LED-CLOCK server listening at http://%s:%s", host, port);
})