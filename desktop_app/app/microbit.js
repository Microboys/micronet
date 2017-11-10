import SerialPort from 'serialport';
import graphActions from './actions/graph';
import connectionActions from './actions/connection';
import { fs, remote } from 'electron';
import Jimp from 'jimp';

/* Get path to prepackaged assets and local temporary storage for new assets */
const assetPath = remote.app.getAppPath() + '/build/assets';
const tempAssetPath = remote.app.getPath('appData') + '/micronet/temp';

var store = null

/* Locating and maintaining connection micro:bit. */
const microbitProductId = '0204';
const microbitVendorId = '0d28';
const microbitBaudRate = 115200;
const timeoutTime = 1500;

var microbitPort = null;
var firstMessageReceived = false;
var messageReceived = false;
var timeoutCheckId = null;

function init(store_) {
  store = store_;  // TODO: this really shouldn't be necessary...
  listen();
}

async function listen () {
  try {
    let portName = await locatePort();
    microbitPort = new SerialPort(portName, { baudRate: microbitBaudRate });
    const parser = new SerialPort.parsers.Readline();
    microbitPort.pipe(parser);
    microbitPort.on('open', handleOpen);
    microbitPort.on('error', handleError);
    microbitPort.on('close', handleClose);
    parser.on('data', handleDataLine);
  } catch (err) {
    console.log('Error on locating the micro:bit port ' + err); 
  }
}

async function locatePort() {
  var port = null;
  while (!port) {
    let ports =  await SerialPort.list();
    port = ports.find(function (port) {
      return port.productId === microbitProductId && port.vendorId === microbitVendorId;
    });
  }
  return port.comName;
}

function handleOpen() {
} 

function handleClose() {
  clearInterval(timeoutCheckId);
  firstMessageReceived = false;
  store.dispatch(connectionActions.updateConnection({'established' : false}));
  if (microbitPort) {
    microbitPort.pause();  // stop listening to events
    microbitPort.flush();  // clear all received data
    microbitPort = null;
  }
  listen();
} 

function handleError(err) {
  console.log("Port error: " + err);
} 

function handleDataLine(data) {
  try {
    var dataJSON = JSON.parse(data);
  } catch (err) {
    console.log('Failed to parse JSON with: ' + err + ' string is ' + dataJSON);
    return;
  }

  if (!dataJSON) {
    console.log('Expecting valid JSON, got: ' + dataJSON);
    return;
  }

  if (!dataJSON.hasOwnProperty('type')) {
    console.log('Expecting a type field in JSON received from micro:bit, got: ' + dataJSON);
    return;
  }

  switch (dataJSON.type) {

    case 'graph':

      messageReceived = true;

      if (!firstMessageReceived) {
	firstMessageReceived = true;
        store.dispatch(connectionActions.updateConnection({'established' : true}));
	timeoutCheckId = setInterval(timeoutCheck, timeoutTime);
      }


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
      store.dispatch(graphActions.updateGraph(transformed));
      break;

    default:
      console.log('Unrecognised JSON type from micro:bit: ' + dataJSON.type);
    }
} 

function timeoutCheck() {
  if (!messageReceived) {
    if (microbitPort) {
      microbitPort.close();
    }
  } else {
    messageReceived = false;
  }
}

/* Functions for sending commands to the micro:bit. */

function sendMsg(to, msg) {
  if (microbitPort) {
    microbitPort.write('MSG\t' + to + '\t' + msg + '\n');
  }
}

/* Functions for transforming received graph data for viewing. */

const minLength = 250; // Minimum possible arc length (on graph)
const lengthCoeff = 5; // Coefficient for multiplying arc length (on graph)

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
  var codeImgPath = tempAssetPath + '/microbit-' + code + '.png';
  var imgPath = codeImgPath;
  try {
    fs.accessSync(imgPath);
  } catch(err) {
    //Image doesn't exist, create using Jimp (async) and use default img for now
    imgPath = assetPath + '/microbit.png';
    try {
      Jimp.read(imgPath, (err, microbit) => {
        if (err) {
          throw err;
        }
        Jimp.read(assetPath + '/led.png', (err, led) => {
          if (err) {
            throw err;
          }
          var newImg = microbit.clone();
          var codeBin = parseInt(code).toString(2);
          for (var i = 24; i > 24 - codeBin.length; i--) {
            if (codeBin[codeBin.length - (25 - i)] == '1') newImg.blit(led, imgLEDX + (i % 5) * imgOffsetX, imgLEDY + Math.floor(i / 5) * imgOffsetY);
          }
          newImg.write(codeImgPath);
        });
      });
    } catch(err) {
      console.log(err);
    }
  }

  return imgPath;
}

/* Exports. */

export { init, sendMsg };
