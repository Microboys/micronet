import SerialPort from 'serialport';
import graphActions from './actions/graph';

const microbitProductId = "0204";
const microbitVendorId = "0d28";
const microbitBaudRate = 115200;

var microbitPort = null;
var locating = false;

const getGraph = (store) => {
  if (microbitPort) {
    microbitPort.write("GRAPH\n", function(err) {
      if (err) {
        console.log(err);
      } else {
        var response = microbitPort.read();
      	if (response) {
      	  console.log("Response is " + response);
      	  var transformed = transformGraphJSON(response);
      	  store.dispatch(graphActions.drawGraph(transformed));
      	}
      }
    });
  } else if (!locating) {
    locatePort();
  }
}

const sendMsg = (to, msg) => {
  console.log(msg);
  if (microbitPort) {
    microbitPort.write("MSG\t" + to + "\t" + msg + "\n", function(err) {
      if(err) {
        console.log(err);
      } else {
        console.log("Message sent!");
      }
    })
  } else if (!locating) {
    locatePort();
  }
}

function locatePort() {
  locating = true;
  const promise = SerialPort.list();
  promise.then((ports) => {
    for (let port of ports) {
      if (port.productId === microbitProductId && port.vendorId === microbitVendorId) {
        var microbitCom = port.comName;
	microbitPort = new SerialPort(microbitCom, {baudRate : microbitBaudRate, autoOpen: true});
	locating = false;
      }
    }
  });
  promise.catch((err) => {
    console.log(err);
  });
}

function transformGraphJSON(graphJSON) {
  var obj = JSON.parse(graphJSON);
  if (obj.type === 'graph') {
    var graph = obj.graph;
    var nodes = [];
    var edges = [];
    var ip = obj.ip;
    console.log("Connected micro:bit: " + ip);
    for (var i = 0; i < graph.length; i++) {
      var arc = graph[i];
      addNode(nodes, arc.from, "Node " + arc.from);
      addNode(nodes, arc.to, "Node " + arc.to);
      edges.push({from: arc.from, to: arc.to, label: RSSIToAbstractDistanceUnits(arc.distance).toString()});
    }
    return {
      nodes: nodes,
      edges: edges
    };
  } else {
    throw new Error("Non-graph JSON passed to transformGraphJSON");
  }
}

function addNode(nodes, id, label, ip) {
  var obj = (ip == id) 
  ? {id: id, label: label, shadow: {enabled: true, color: "#59B4FF", x: 0, y: 0, size: 20}} 
  : {id: id, label: label};
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

function RSSIToAbstractDistanceUnits(rssi) {
  let scaling = -(Math.PI/2) / -255;
  return Math.round((Math.abs(Math.cos(rssi * scaling)) * 100));
}

export { getGraph, sendMsg };
