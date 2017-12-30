'use strict';

var _react = require('react');

var _react2 = _interopRequireDefault(_react);

var _reactDom = require('react-dom');

var _reactDom2 = _interopRequireDefault(_reactDom);

var _reactRedux = require('react-redux');

var _reactRouterRedux = require('react-router-redux');

var _history = require('history');

var _routes = require('./routes');

var _routes2 = _interopRequireDefault(_routes);

var _store = require('./store');

var _store2 = _interopRequireDefault(_store);

var _microbit = require('./microbit.js');

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

//Setup temp folder for microbit images
var remote = require('electron').remote;
var fs = remote.require('fs');
var app = remote.app;
app.setPath('temp', app.getPath('appData') + '/micronet/temp');
try {
  var items = fs.readdirSync(app.getPath('temp'));
  for (var i = 0; i < items.length; i++) {
    var item = app.getPath('temp') + '/' + items[i];
    console.log('Deleting: ' + item);
    try {
      fs.unlinkSync(item);
    } catch (err) {
      fs.rmdirSync(item);
    }
  }
} catch (err) {
  console.log(err);
  fs.mkdirSync(app.getPath('temp'));
}

var syncHistoryWithStore = function syncHistoryWithStore(store, history) {
  var _store$getState = store.getState(),
      routing = _store$getState.routing;

  if (routing && routing.location) {
    history.replace(routing.location);
  }
};

var initialState = {
  'graph': {
    'nodes': [],
    'edges': []
  },
  'packet': {
    'received': [],
    'capacity': 15
  },
  'dns': {
    'entries': []
  },
  'connection': {
    'established': false
  },
  'sinkTree': {
    routes: []
  }
};

var routerHistory = (0, _history.createMemoryHistory)();
var store = (0, _store2.default)(initialState, routerHistory);
syncHistoryWithStore(store, routerHistory);

var rootElement = document.querySelector(document.currentScript.getAttribute('data-container'));

_reactDom2.default.render(_react2.default.createElement(
  _reactRedux.Provider,
  { store: store },
  _react2.default.createElement(
    _reactRouterRedux.ConnectedRouter,
    { history: routerHistory },
    _routes2.default
  )
), rootElement);

(0, _microbit.init)(store);