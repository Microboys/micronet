import SerialPort from 'serialport';
import graphActions from './actions/graph';

const microbitProductId = "0204";
const microbitVendorId = "0d28";
const microbitBaudRate = 115200;

var microbitPort = null;
var locating = false;

const getGraph = (store) => {
  if (microbitPort) {
    console.log("Sending the graph command");
    microbitPort.write("GRAPH\n", function(err) {
      if (err) {
        console.log(err);
      } else {
        console.log("Sent the graph command");
        var response = microbitPort.read();
	if (response) {
	  transformGraphJSON(response);
	  console.log("Response is: " + response);
	} else {
	  console.log("No response");
	}
        //const action = graphActions.drawGraph();
        //store.dispatch(action)
      }
    });
  } else if (!locating) {
    locatePort();
  }
}

function locatePort() {
  console.log("locating the port...");
  locating = true;
  const promise = SerialPort.list();
  promise.then((ports) => {
    for (let port of ports) {
      if (port.productId === microbitProductId && port.vendorId === microbitVendorId) {
        var microbitCom = port.comName;
	microbitPort = new SerialPort(microbitCom, {baudRate : microbitBaudRate, autoOpen: true});
        console.log("found it");
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
    for (var i = 0; i < graph.length; i++) {
      var arc = graph[i];
      addNode(nodes, arc.from, "Node " + arc.from);
      addNode(nodes, arc.to, "Node " + arc.to);
      edges.push({from: arc.from, to: arc.to, label: arc.distance.toString()});
    }
    return {
      nodes: nodes,
      edges: edges
    };
  } else {
    throw new Error("Non-graph JSON passed to transformGraphJSON");
  }
}

function addNode(nodes, id, label) {
  var obj = {id: id, label: label};
  if (!nodeExists(nodes, id)) {
    nodes.push({id: id, label: label});
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

export { getGraph };
