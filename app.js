var serialPort = require("serialport");
var SerialPort = serialPort.SerialPort;
var uart;
serialPort.list(function (err, ports) {
	var portName = "";
	ports.forEach(function(port) {
		if((port.manufacturer+"").indexOf("wch.cn")>-1){
			portName= port.comName;
		}
	});
	if(portName!=""){
		uart = new SerialPort(portName, {
			baudrate: 115200
		});
		uart.on('open',function(err){
			if(err){
				console.log(err);
			}else{
				console.log(portName);
			}
		});
		uart.on('data',function(data){
			//console.log(data.toString());
		});
	}
});

var webSocket = require('ws'),
ws = new webSocket('ws://127.0.0.1:6437');
var lastTime = 0;
ws.on('message', function(data, flags) {
    var frame = JSON.parse(data);
    if (frame.hands && frame.hands.length == 1) {
		var x = Math.floor(frame.hands[0].t[0])+20;
		var y = Math.floor(frame.hands[0].t[2])+10;
		var z = Math.floor(frame.hands[0].t[1]);
		var w = Math.floor(frame.hands[0].sphereRadius);
		if(new Date().getTime()-lastTime>50){
			lastTime = new Date().getTime();
			if(uart){
				//console.log(frame.hands[0])
				var sendBuf = ("x="+x+"&y="+y+"&z="+z+"&w="+w+"\n");
				console.log(sendBuf);
				uart.write(sendBuf);
			}
		}
    }else {
     
	}
});