const express = require('express');
const expressWs = require('express-ws');

const app = express();
expressWs(app);

//serve static files, 'public/index.html' will be served as '/'
app.use(express.static('../data'));

app.get("/", function(req,res) {
  res.send("Hello");
});
// normal express.js handler for HTTP GET
app.get('/switch/:mode', function(req, res, next){
  res.send('Switched to mode: ' + req.params.mode);
});

// websocket handler
app.ws('/websocket', function(ws, req) {
  ws.on('connect', () => {console.log("Connected")} );
  ws.on('message', function(message) {
    console.log('Received: %s', message);
    let data = parseMessage(message);
    console.log(data);
  });
});

console.log("Listening on port 3000");
app.listen(3000);

function parseMessage(message) {
  var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(message);
  if (result) {
    return {
      id: parseInt(result[1], 16),
      r: parseInt(result[2], 16),
      g: parseInt(result[3], 16),
      b: parseInt(result[4], 16)
    }
  }
  result = /^s([0-3])/i.exec(message);
  return result ? {mode: result[1]} : null;
}
