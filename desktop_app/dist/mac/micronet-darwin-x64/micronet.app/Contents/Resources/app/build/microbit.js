'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.getRoute = exports.transformGraphJSON = exports.lookupName = exports.flashMicrobit = exports.renameMicrobit = exports.sendMsg = exports.init = undefined;

var _regenerator = require('babel-runtime/regenerator');

var _regenerator2 = _interopRequireDefault(_regenerator);

var _asyncToGenerator2 = require('babel-runtime/helpers/asyncToGenerator');

var _asyncToGenerator3 = _interopRequireDefault(_asyncToGenerator2);

var listen = function () {
  var _ref = (0, _asyncToGenerator3.default)(_regenerator2.default.mark(function _callee() {
    var portName;
    return _regenerator2.default.wrap(function _callee$(_context) {
      while (1) {
        switch (_context.prev = _context.next) {
          case 0:
            _context.prev = 0;
            _context.next = 3;
            return locatePort();

          case 3:
            portName = _context.sent;

            microbitPort = new _serialport2.default(portName, { baudRate: microbitBaudRate, encoding: 'ascii' });
            microbitPort.on('open', handleOpen);
            microbitPort.on('error', handleError);
            microbitPort.on('close', handleClose);
            if (!initialisedParser) {
              parser.on('data', handleDataLine);
              initialisedParser = true;
            }
            //parser.on('invalid-line', (err) => { console.log('ignore invalid JSON: ' + err.source) });
            _context.next = 14;
            break;

          case 11:
            _context.prev = 11;
            _context.t0 = _context['catch'](0);

            console.log('Error on locating the micro:bit port ' + _context.t0);

          case 14:
          case 'end':
            return _context.stop();
        }
      }
    }, _callee, this, [[0, 11]]);
  }));

  return function listen() {
    return _ref.apply(this, arguments);
  };
}();

var locatePort = function () {
  var _ref2 = (0, _asyncToGenerator3.default)(_regenerator2.default.mark(function _callee2() {
    var port, ports;
    return _regenerator2.default.wrap(function _callee2$(_context2) {
      while (1) {
        switch (_context2.prev = _context2.next) {
          case 0:
            port = null;

          case 1:
            if (port) {
              _context2.next = 8;
              break;
            }

            _context2.next = 4;
            return _serialport2.default.list();

          case 4:
            ports = _context2.sent;

            port = ports.find(function (port) {
              return port.productId === microbitProductId;
            });
            _context2.next = 1;
            break;

          case 8:
            return _context2.abrupt('return', port.comName);

          case 9:
          case 'end':
            return _context2.stop();
        }
      }
    }, _callee2, this);
  }));

  return function locatePort() {
    return _ref2.apply(this, arguments);
  };
}();

var _serialport = require('serialport');

var _serialport2 = _interopRequireDefault(_serialport);

var _jsonlines = require('jsonlines');

var _jsonlines2 = _interopRequireDefault(_jsonlines);

var _graph = require('./actions/graph');

var _graph2 = _interopRequireDefault(_graph);

var _dns = require('./actions/dns');

var _dns2 = _interopRequireDefault(_dns);

var _sinkTree = require('./actions/sinkTree');

var _sinkTree2 = _interopRequireDefault(_sinkTree);

var _connection = require('./actions/connection');

var _connection2 = _interopRequireDefault(_connection);

var _packet = require('./actions/packet');

var _packet2 = _interopRequireDefault(_packet);

var _electron = require('electron');

var _jimp = require('jimp');

var _jimp2 = _interopRequireDefault(_jimp);

var _fsExtra = require('fs-extra');

var _fsExtra2 = _interopRequireDefault(_fsExtra);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

/* Get path to prepackaged assets and local temporary storage for new assets */
var fs = _electron.remote.require('fs');
var assetPath = _electron.remote.app.getAppPath() + '/build/assets';
var routerPath = _electron.remote.app.getAppPath() + '/build/router/router.hex';
var tempAssetPath = _electron.remote.app.getPath('appData') + '/micronet/temp';

/* Get dialog for flashing microbit */
var dialog = _electron.remote.dialog;

var store = null;

/* Locating and maintaining connection micro:bit. */
var microbitProductId = '0204';
var microbitBaudRate = 115200;
var timeoutTime = 2000;
var pollSerialTime = 10;
var parser = _jsonlines2.default.parse({ emitInvalidLines: true });

var microbitPort = null;
var firstMessageReceived = false;
var initialisedParser = false;
var messageReceived = false;
var timeoutCheckId = null;
var connectedIp = null;

var imgWaitBuffer = [];

function init(store_) {
  store = store_; // TODO: this really shouldn't be necessary...
  listen();

  // Serialport garbles our data if we use they're event based interface, hence the polling.
  setInterval(function () {
    if (microbitPort) {
      var bytes = microbitPort.read();
      if (bytes) {
        parser.write(bytes);
      }
    }
  }, pollSerialTime);
}

function handleOpen() {}

function handleClose() {
  clearInterval(timeoutCheckId);
  firstMessageReceived = false;
  store.dispatch(_connection2.default.updateConnection({ 'established': false }));
  if (microbitPort) {
    microbitPort.pause(); // stop listening to events
    microbitPort.flush(); // clear all received data
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
    store.dispatch(_connection2.default.updateConnection({ 'established': true }));
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
        console.log('Expecting a ptype field in JSON received with type \'packet\' from micro:bit, got: ' + dataJSON);
        break;
      }

      dataJSON.time = new Date().getTime();
      console.log(dataJSON);
      store.dispatch(_packet2.default.addPacket(dataJSON));
      break;

    case 'graph':
      timeoutUpdate();
      messageReceived = true;

      if (!dataJSON.hasOwnProperty('graph')) {
        console.log('Expecting a graph field in JSON received with type \'graph\' from micro:bit, got: ' + dataJSON);
        break;
      }

      if (!dataJSON.hasOwnProperty('ip')) {
        console.log('Expecting a ip field in JSON received with type \'graph\' from micro:bit, got: ' + dataJSON);
        break;
      }

      connectedIp = dataJSON.ip;
      var transformed = transformGraphJSON(dataJSON.graph, connectedIp);
      store.dispatch(_graph2.default.updateGraph(transformed));
      break;

    case 'sink-tree':
      timeoutUpdate();
      var sinkTree = {};
      sinkTree.routes = dataJSON['sink-tree'];
      store.dispatch(_sinkTree2.default.updateSinkTree(sinkTree));
      break;

    case 'dns':
      timeoutUpdate();

      if (!dataJSON.hasOwnProperty('dns')) {
        console.log('Expecting a DNS field in JSON received with type \'dns\' from micro:bit, got: ' + dataJSON);
        break;
      }
      store.dispatch(_dns2.default.updateDNS({ 'entries': dataJSON.dns }));
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

var minLength = 250; // Minimum possible arc length (on graph)
var lengthCoeff = 5; // Coefficient for multiplying arc length (on graph)

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
  var node = { id: id, label: getLabel(id, connected) };
  if (connected) {
    node.shadow = { enabled: true, color: '#59B4FF', x: 0, y: 0, size: 20 };
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
  edges.push({ from: edge.from, to: edge.to, label: distance.toString() });
}

function getLabel(id) {
  var name = lookupName(id);
  if (name) {
    return name;
  } else {
    return 'Node ' + id;
  }
}

function RSSIToAbstractDistanceUnits(rssi) {
  return Math.max(-rssi - 50, 0);
}

function getRoute(dest) {
  var routes = store.getState().sinkTree.routes;
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

var imgLEDX = 142;
var imgLEDY = 114;
var imgOffsetX = 33;
var imgOffsetY = 33;

/* Visual id given by the denary of the micro:bit's ID converted to binary as 25-bit string, each bit
 represents 1 LED (e.g. code = '2' = 0b10 = 000...0010, so the second to last LED is lit). */
function generateMicrobitImage(code) {
  //First check if image exists for this ID
  var codeImgPath = tempAssetPath + '/microbit-' + code + '.png';
  var genericImgPath = assetPath + '/microbit.png';
  var imgPath = codeImgPath;
  try {
    fs.accessSync(imgPath);

    //Check if image is in wait buffer (i.e. can't be loaded yet to avoid loading non-finished images)
    for (var i = 0; i < imgWaitBuffer.length; i++) {
      if (imgWaitBuffer[i] == imgPath) {
        console.log(imgWaitBuffer);
        imgWaitBuffer.splice(i, 1);
        console.log(imgWaitBuffer);
        imgPath = genericImgPath;
        break;
      }
    }
  } catch (err) {
    //Add image to image wait buffer so the image isn't used too soon (before being generated)
    imgWaitBuffer.push(codeImgPath);

    //Image doesn't exist, create using Jimp (async) and use default img for now
    imgPath = genericImgPath;
    try {
      _jimp2.default.read(imgPath, function (err, microbit) {
        if (err) {
          throw err;
        }
        _jimp2.default.read(assetPath + '/led.png', function (err, led) {
          if (err) {
            throw err;
          }
          var newImg = microbit.clone();
          var codeBin = parseInt(code).toString(2);
          for (var i = 24; i > 24 - codeBin.length; i--) {
            if (codeBin[codeBin.length - (25 - i)] == '1') newImg.blit(led, imgLEDX + i % 5 * imgOffsetX, imgLEDY + Math.floor(i / 5) * imgOffsetY);
          }
          newImg.write(codeImgPath);
        });
      });
    } catch (err) {
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
    filters: [{ name: 'Directories', extensions: [''] }]
  };
  dialog.showOpenDialog(dialogProperties, function (fileNames) {
    if (!fileNames) return;
    var fileName = fileNames[0];
    _fsExtra2.default.copySync(routerPath, fileName + '/router.hex');
  });
}

/* Exports. */

exports.init = init;
exports.sendMsg = sendMsg;
exports.renameMicrobit = renameMicrobit;
exports.flashMicrobit = flashMicrobit;
exports.lookupName = lookupName;
exports.transformGraphJSON = transformGraphJSON;
exports.getRoute = getRoute;