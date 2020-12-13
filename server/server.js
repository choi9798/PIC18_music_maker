var express = require('express');
var fs = require('fs');
var port = 12345;
var app = express();
const SerialPort = require('serialport')
const serialport = new SerialPort("/dev/ttyUSB0", {
  baudRate: 1200
  // baudRate: 4800
});

app.use(express.static(__dirname +'/public'));

app.get('/play',function (req,res) {
    console.log('play clicked');
    setTimeout(function() {
        console.log(req.query.bfr0+"=0");
        serialport.write(req.query.bfr0);
    }, 0);
    setTimeout(function() {
        console.log(req.query.bfr1+"=1");
        serialport.write(req.query.bfr1);
    }, 200);
    setTimeout(function() {
        console.log(req.query.bfr2+"=2");
        serialport.write(req.query.bfr2);
    }, 400);
    setTimeout(function() {
        console.log(req.query.bfr3+"=3");
        serialport.write(req.query.bfr3);
    }, 600);
    setTimeout(function() {
        console.log(req.query.bfr4+"=4");
        serialport.write(req.query.bfr4);
    }, 800);
    res.end()
})

app.get('/rst',function (req,res) {
    console.log('reset clicked');
    console.log('Q');
    serialport.write('Q');
    res.end();
})

app.listen(port,()=>{
  console.log(`Listening on port: ${port}`);
});
