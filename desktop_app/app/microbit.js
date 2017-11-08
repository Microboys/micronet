import SerialPort from 'serialport';
import graphActions from './actions/graph';

const remote = require('electron').remote;
const fs = remote.require('fs');
const assetPath = remote.app.getAppPath() + "/build/assets";
const tempAssetPath = remote.app.getPath('appData') + "/micronet/temp";
const Jimp = require("jimp");

const microbitProductId = "0204";
const microbitVendorId = "0d28";
const microbitBaudRate = 115200;

const minLength = 140;
const lengthCoeff = 2;

const imgLEDX = 142;
const imgLEDY = 114;
const imgOffsetX = 33;
const imgOffsetY = 33;

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
  try {
    var obj = JSON.parse(graphJSON);
    if (obj.type === 'graph') {
      var graph = obj.graph;
      var nodes = [];
      var edges = [];
      var ip = obj.ip;
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
    } else {
      throw new Error("Non-graph JSON passed to transformGraphJSON");
    }
  } catch(err) {
    console.log("Invalid graph obtained. Ignoring:");
    console.log(err);
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

function addNode(nodes, id, connected) {
  if (nodeExists(nodes, id)) {
    return;
  }
  var node = {id: id, label: "Node " + id};
  if (connected) {
    node.shadow = {enabled: true, color: "#59B4FF", x: 0, y: 0, size: 20};
    node.label += " (connected)"
  }
  node.image = generateMicrobitImage(id);
  nodes.push(node);
}

function addEdge(edges, edge) {
  var distance = RSSIToAbstractDistanceUnits(edge.distance);
  edges.push({from: edge.from, to: edge.to, label: distance.toString(), length: minLength + (lengthCoeff * distance)});
}


//Generates an image of the microbit showing the visual ID derived from code
//code = denary of ID, converted to binary gives length 25 bin string - each bit
//represents 1 LED (e.g. code = '2' = 0b10 = 000...0010, second last LED is lit)
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

function RSSIToAbstractDistanceUnits(rssi) {
  let scaling = -(Math.PI/2) / -255;
  return Math.round(100 - (Math.abs(Math.cos(rssi * scaling)) * 100));
}

export { getGraph, sendMsg };
