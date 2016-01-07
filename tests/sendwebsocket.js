var WebSocket = require('ws');
var wsserver = "ws://127.0.0.1:12150";
var wstoken = "foome";
var stdargs = "";
var ignorefirst = "true";

function connectWebSocket()
{
  websocket = new WebSocket(wsserver);
  websocket.onopen = function(evt) { onOpen(evt) };
  websocket.onclose = function(evt) { onClose(evt) };
  websocket.onmessage = function(evt) { onMessage(evt) };
  websocket.onerror = function(evt) { onError(evt) };
}

function onOpen(evt)
{
  var authjson = '{ "namespace":"rpc", "name":"auth", "id":"authrequest", "args": { "username":"' + process.argv[2] +'", "password":"' + process.argv[3] + '" } }';
  nstatus = "auth";
  wstatus = "idle";
  doSend(authjson);
  doSend(stdargs);
  websocket.close();
}

function onClose(evt)
{
}

function onError(evt)
{
  console.log(evt);
}

function onMessage(evt)
{
  var jsonobj = JSON.parse(evt.data);
  if ( ignorefirst == "true" ) {
    ignorefirst = "false";
  } else {
    console.log(JSON.stringify(jsonobj, null, 2));
  }
}

function doSend(message)
{
  //console.log('Sent: ' + message);
  websocket.send(message);
}

function readStdIn(msg)
{
  stdargs = msg;
}

process.stdin.resume();
process.stdin.setEncoding('utf8');

process.stdin.on('data', function(message) {
  readStdIn(message);
});

connectWebSocket();
