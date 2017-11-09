import SerialPort from 'serialport';
import graphActions from './actions/graph';
var fs = require('electron').remote.require('fs'); // TODO: Daniel, should these be import statements?
var Jimp = require('jimp'); // TODO: as above

/* Locating and maintaining connection micro:bit. */

const microbitProductId = '0204';
const microbitVendorId = '0d28';
const microbitBaudRate = 115200;

var microbitPort = null;

async function locatePort() {
  var portName = null;
  while (!portName) {
    let portNames =  await SerialPort.list();
    portName = portNames.find(function (port) {
      return port.productId === microbitProductId && port.vendorId === microbitVendorId;
    });
  }
  return portName;
}

function close() {
  microbitPort.pause();  // stop listening to events
  microbitPort.flush();  // clear all received data
  microbitPort = null;
}

function listen(store) {
  locatePort()
    .then((portName) => {
      microbitPort = new SerialPort(portName, { baudRate: microbitBaudRate });
      const parser = SerialPort.parsers.Readline;
      microbitPort.pipe(parser);
      microbitPort.on('error', function(err) {
        console.log('Error on using the port: ' + err);
        close();
        listen();
      });
      microbitPort.on('close', function() {
        close();
        listen();
      });
      microbitPort.on('open', function() {
        close();
        listen();
      });
      parser.on('data', function(data) {
        try {
          var dataJSON = JSON.parse(data);
        } catch (err) {
          console.log('Failed to parse JSON with: ' + err);
        }

        if (!dataJSON.hasOwnProperty('type')) {
          console.log('Expecting a type field in JSON received from micro:bit, got: ' + dataJSON);
          return;
        }

        switch (dataJSON.type) {
        case 'graph':

          if (!dataJSON.hasOwnProperty('graph')) {
            console.log('Expecting a graph field in JSON received with type \'graph\' from micro:bit, got: '
              + dataJSON);
            break;
          }

          if (!dataJSON.hasOwnProperty('ip')) {
            console.log('Expecting a ip field in JSON received with type \'graph\' from micro:bit, got: '
              + dataJSON);
            break;
          }

          var transformed = transformGraphJSON(dataJSON.graph, dataJSON.ip);
          store.dispatch(graphActions.drawGraph(transformed));
          break;
        default:
          console.log('Unrecognised JSON type from micro:bit: ' + dataJSON.type);
        }
      });
    })
    .catch(err => console.log('Finding the port failed with: ' + err));
}

/* Functions for sending commands to the micro:bit. */

function sendMsg(to, msg) {
  if (microbitPort) {
    microbitPort.write('MSG\t' + to + '\t' + msg + '\n');
  }
}

/* Functions for transforming received graph data for viewing. */

const minLength = 140; // TODO: Daniel explain
const lengthCoeff = 2; // TODO: Daniel explain

function transformGraphJSON(graph, ip) {
  var nodes = [];
  var edges = [];
  addNode(nodes, ip, true);
  for (var i = 0; i < graph.length; i++) {
    var arc = graph[i];
    addNode(nodes, arc.from, false);
    addNode(nodes, arc.to, false);
    addEdge(edges, arc);
  }
  return {
    nodes: nodes,
    edges: edges
  };
}

function nodeExists(nodes, id) {
  for (var i = 0; i < nodes.length; i++) {
    if (nodes[i].id == id) {
      return true;
    }
  }
  return false;
}

function addNode(nodes, id, connected) {
  if (nodeExists(nodes, id)) {
    return;
  }
  var node = {id: id, label: 'Node ' + id};
  if (connected) {
    node.shadow = {enabled: true, color: '#59B4FF', x: 0, y: 0, size: 20};
    node.label += ' (connected)';
  }
  node.image = generateMicrobitImage(id);
  nodes.push(node);
}

function addEdge(edges, edge) {
  var distance = RSSIToAbstractDistanceUnits(edge.distance);
  edges.push({from: edge.from, to: edge.to, label: distance.toString(), length: minLength + (lengthCoeff * distance)});
}

function RSSIToAbstractDistanceUnits(rssi) {
  let scaling = -(Math.PI/2) / -255;
  return Math.round(100 - (Math.abs(Math.cos(rssi * scaling)) * 100));
}

/* Generating micro:bit images with a visual id (for the graph). */

const imgLEDX = 142;
const imgLEDY = 114;
const imgOffsetX = 33;
const imgOffsetY = 33;

/* Visual id given by the denary of the micro:bit's ID converted to binary as 25-bit string, each bit
 represents 1 LED (e.g. code = '2' = 0b10 = 000...0010, so the second to last LED is lit). */
function generateMicrobitImage(code) {
  //First check if image exists for this ID
  var codeImgPath = 'assets/microbit-' + code + '.png';
  var imgPath = codeImgPath;
  try {
    fs.readdir('.', (err, items) => {console.log(items);});
    fs.accessSync('./build/' + imgPath);
  } catch(err) {
    //Image doesn't exist, create using Jimp and use default img for now
    imgPath = 'assets/microbit.png';
    Jimp.read('./build/assets/microbit.png', (err, microbit) => {
      if (err) {
        throw err;
      }
      Jimp.read('./build/assets/led.png', (err, led) => {
        if (err) {
          throw err;
        }
        var newImg = microbit.clone();
        var codeBin = parseInt(code).toString(2);
        for (var i = 24; i > 24 - codeBin.length; i--) {
          if (codeBin[codeBin.length - (25 - i)] == '1') newImg.blit(led, imgLEDX + (i % 5) * imgOffsetX, imgLEDY + Math.floor(i / 5) * imgOffsetY);
        }
        newImg.write('./build/' + codeImgPath);
      });
    });
  }

  return './' + imgPath;
}

/* Exports. */

export { listen, sendMsg };
