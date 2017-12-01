import SerialPort from 'serialport';
import jsonlines from 'jsonlines';
import graphActions from './actions/graph';
import dnsActions from './actions/dns';
import sinkTreeActions from './actions/sinkTree';
import connectionActions from './actions/connection';
import packetActions from './actions/packet';
import { remote } from 'electron';
import Jimp from 'jimp';
import fse from 'fs-extra';

/* Get path to prepackaged assets and local temporary storage for new assets */
const fs = remote.require('fs');
const assetPath = remote.app.getAppPath() + '/build/assets';
const routerPath = remote.app.getAppPath() + '/build/router/router.hex';
const tempAssetPath = remote.app.getPath('appData') + '/micronet/temp';

/* Get dialog for flashing microbit */
const dialog = remote.dialog;

var store = null

/* Locating and maintaining connection micro:bit. */
const microbitProductId = '0204';
const microbitVendorId = '0d28';
const microbitBaudRate = 115200;
const timeoutTime = 2000;
const pollSerialTime = 10;
const parser = jsonlines.parse({ emitInvalidLines : true });

var microbitPort = null;
var firstMessageReceived = false;
var messageReceived = false;
var timeoutCheckId = null;
var connectedIp = null;

function init(store_) {
  store = store_;  // TODO: this really shouldn't be necessary...
  listen();

  // Serialport garbles our data if we use they're event based interface, hence the polling.
  setInterval(() => {
    if (microbitPort) {
      let bytes = microbitPort.read();
      if (bytes) {
	parser.write(bytes)
      }
    }
  }, pollSerialTime);
}

async function listen () {
  try {
    let portName = await locatePort();
    microbitPort = new SerialPort(portName, { baudRate: microbitBaudRate, encoding: 'ascii' });
    microbitPort.on('open', handleOpen);
    microbitPort.on('error', handleError);
    microbitPort.on('close', handleClose);
    parser.on('data', handleDataLine);
    //parser.on('invalid-line', (err) => { console.log('ignore invalid JSON: ' + err.source) });
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

function timeoutUpdate() {
  messageReceived = true;
  if (!firstMessageReceived) {
    firstMessageReceived = true;
    store.dispatch(connectionActions.updateConnection({'established' : true}));
    timeoutCheckId = setInterval(timeoutCheck, timeoutTime);
  }
}

function handleDataLine(dataJSON) {

  if (!dataJSON) {
    console.log('Expecting valid JSON, got: ' + dataJSON);
    return;
  }

  if (!dataJSON.hasOwnProperty('type')) {
    console.log('Expecting a type field in JSON received from micro:bit, got: ' + dataJSON);
    return;
  }

  switch (dataJSON.type) {
    case 'packet':
      timeoutUpdate();

      if (!dataJSON.hasOwnProperty('ptype')) {
        console.log('Expecting a ptype field in JSON received with type \'packet\' from micro:bit, got: '
          + dataJSON);
	break;
      }

      dataJSON.time = new Date().getTime();
      console.log(dataJSON);
      store.dispatch(packetActions.addPacket(dataJSON));
      break;

    case 'graph':
      timeoutUpdate();
      messageReceived = true;

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

      connectedIp = dataJSON.ip;
      var transformed = transformGraphJSON(dataJSON.graph, connectedIp);
      store.dispatch(graphActions.updateGraph(transformed));
      break;

    case 'sink-tree':
     timeoutUpdate();
     let sinkTree = {};
     sinkTree.routes = dataJSON['sink-tree'];
     store.dispatch(sinkTreeActions.updateSinkTree(sinkTree));
     break;

    case 'dns':
      timeoutUpdate();

      if (!dataJSON.hasOwnProperty('dns')) {
        console.log('Expecting a DNS field in JSON received with type \'dns\' from micro:bit, got: '
          + dataJSON);
        break;
      }
      store.dispatch(dnsActions.updateDNS({'entries': dataJSON.dns}));
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
    console.log("Sending message");
    microbitPort.write('MSG\t' + to + '\t' + msg + '\n');
  }
}

function renameMicrobit(name) {
  if (microbitPort) {
    console.log("Renaming microbit to " + name);
    microbitPort.write('DNS\t' + name + '\n');
  }
}

/* Functions for transforming received graph data for viewing. */

const minLength = 250; // Minimum possible arc length (on graph)
const lengthCoeff = 5; // Coefficient for multiplying arc length (on graph)

function transformGraphJSON(graph, ip) {
  var nodes = [];
  var edges = [];
  if (ip) {
    addNode(nodes, ip, true);
  }
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
  var node = {id: id, label: getLabel(id, connected)};
  if (connected) {
    node.shadow = {enabled: true, color: '#59B4FF', x: 0, y: 0, size: 20};
    node.label += ' (connected)';
  }
  node.image = generateMicrobitImage(id);
  nodes.push(node);
}

function addEdge(edges, edge) {
  var distance = RSSIToAbstractDistanceUnits(edge.distance);
  for (var i = 0; i < edges.length; i++) {
    if (edges[i].from == edge.to && edges[i].to == edge.from) {
      distance = Math.round((distance + parseInt(edges[i].label)) / 2);
      edges[i].label = distance.toString();
    }
  }
  edges.push({from: edge.from, to: edge.to, label: distance.toString()});
}

function getLabel(id) {
  let name = lookupName(id);
  if (name) {
    return name;
  } else {
    return 'Node ' + id;
  }
}

function RSSIToAbstractDistanceUnits(rssi) {
  return Math.max((-rssi) - 50, 0);
}

function getRoute(dest) {
  let routes = store.getState().sinkTree.routes;
  for (var i = 0; i < routes.length; i++) {
    if (routes[i].to == dest) {
      routes[i].path.unshift(connectedIp);
      routes[i].path.push(dest);
      return routes[i].path;
    }
  }
  return null;
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

function lookupName(id) {
  var entries = store.getState().dns.entries;
  for (var i = 0; i < entries.length; i++) {
    var entry = entries[i];
    if (id == entry.ip) {
      return entry.name;
    }
  }
  return null;
}

function flashMicrobit() {
  var dialogProperties = {
    properties: ["openDirectory"],
    title: "Select the connected MICROBIT folder",
    filters: [
      { name: 'Directories', extensions: [''] }
    ]
  };
  dialog.showOpenDialog(dialogProperties, function (fileNames) {
    if (!fileNames) return;
    var fileName = fileNames[0];
    fse.copySync(routerPath, fileName + '/router.hex');
  })
}

/* Exports. */

export { init, sendMsg, renameMicrobit, flashMicrobit, lookupName, transformGraphJSON, getRoute };
