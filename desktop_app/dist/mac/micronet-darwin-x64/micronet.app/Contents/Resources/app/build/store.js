'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _defineProperty2 = require('babel-runtime/helpers/defineProperty');

var _defineProperty3 = _interopRequireDefault(_defineProperty2);

var _extends2 = require('babel-runtime/helpers/extends');

var _extends3 = _interopRequireDefault(_extends2);

exports.default = configureStore;

var _redux = require('redux');

var _reactRouterRedux = require('react-router-redux');

var _reduxThunk = require('redux-thunk');

var _reduxThunk2 = _interopRequireDefault(_reduxThunk);

var _graph = require('./reducers/graph');

var _graph2 = _interopRequireDefault(_graph);

var _graph3 = require('./actions/graph');

var _graph4 = _interopRequireDefault(_graph3);

var _connection = require('./reducers/connection');

var _connection2 = _interopRequireDefault(_connection);

var _connection3 = require('./actions/connection');

var _connection4 = _interopRequireDefault(_connection3);

var _packet = require('./reducers/packet');

var _packet2 = _interopRequireDefault(_packet);

var _packet3 = require('./actions/packet');

var _packet4 = _interopRequireDefault(_packet3);

var _dns = require('./reducers/dns');

var _dns2 = _interopRequireDefault(_dns);

var _dns3 = require('./actions/dns');

var _dns4 = _interopRequireDefault(_dns3);

var _sinkTree = require('./reducers/sinkTree');

var _sinkTree2 = _interopRequireDefault(_sinkTree);

var _sinkTree3 = require('./actions/sinkTree');

var _sinkTree4 = _interopRequireDefault(_sinkTree3);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function configureStore(initialState, routerHistory) {
  var _reducers;

  var router = (0, _reactRouterRedux.routerMiddleware)(routerHistory);

  var actionCreators = (0, _extends3.default)({}, _graph4.default, _dns4.default, _connection4.default, _graph4.default, _packet4.default, _sinkTree4.default, {
    push: _reactRouterRedux.push
  });

  var reducers = (_reducers = {
    graph: _graph2.default,
    dns: _dns2.default,
    connection: _connection2.default
  }, (0, _defineProperty3.default)(_reducers, 'graph', _graph2.default), (0, _defineProperty3.default)(_reducers, 'packet', _packet2.default), (0, _defineProperty3.default)(_reducers, 'sinkTree', _sinkTree2.default), (0, _defineProperty3.default)(_reducers, 'routing', _reactRouterRedux.routerReducer), _reducers);

  var middlewares = [_reduxThunk2.default, router];

  var composeEnhancers = function () {
    var compose_ = window && window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__;
    if (process.env.NODE_ENV === 'development' && compose_) {
      return compose_({ actionCreators: actionCreators });
    }
    return _redux.compose;
  }();

  var enhancer = composeEnhancers(_redux.applyMiddleware.apply(undefined, middlewares));
  var rootReducer = (0, _redux.combineReducers)(reducers);

  return (0, _redux.createStore)(rootReducer, initialState, enhancer);
}