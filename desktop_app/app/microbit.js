import SerialPort from 'serialport';
import graphActions from './actions/graph';

const microbitProductId = '0204';
const microbitVendorId = '0d28';
const microbitBaudRate = 115200;

const minLength = 140; // TODO: Daniel explain
const lengthCoeff = 2; // TODO: Daniel explain

/* Functions for locating and maintaining connection micro:bit. */
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
          var transformed = transformGraphJSON(dataJSON);
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

function transformGraphJSON(graphJSON) {
  var graph = graphJSON.graph;
  var nodes = [];
  var edges = [];
  var ip = graphJSON.ip;
  for (var i = 0; i < graph.length; i++) {
    var arc = graph[i];
    addNode(nodes, arc.from, ip);
    addNode(nodes, arc.to, ip);
    addEdge(edges, arc);
  }
  return {
    nodes: nodes,
    edges: edges
  };
}

function addNode(nodes, id, ip) {
  var obj = {id: id, label: 'Node ' + id};
  if (ip == id) {
    obj.shadow = {enabled: true, color: '#59B4FF', x: 0, y: 0, size: 20};
  }
  if (!nodeExists(nodes, id)) {
    nodes.push(obj);
  }
}

function nodeExists(nodes, id) {
  for (var i = 0; i < nodes.length; i++) {
    if (nodes[i].id == id) {
      return true;
    }
  }
  return false;
}

function addEdge(edges, edge) {
  var distance = RSSIToAbstractDistanceUnits(edge.distance);
  edges.push({from: edge.from, to: edge.to, label: distance.toString(), length: minLength + (lengthCoeff * distance)});
}

function RSSIToAbstractDistanceUnits(rssi) {
  let scaling = -(Math.PI/2) / -255;
  return Math.round(100 - (Math.abs(Math.cos(rssi * scaling)) * 100));
}

export { listen, sendMsg };
