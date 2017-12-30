'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _defineProperty2 = require('babel-runtime/helpers/defineProperty');

var _defineProperty3 = _interopRequireDefault(_defineProperty2);

var _extends2 = require('babel-runtime/helpers/extends');

var _extends3 = _interopRequireDefault(_extends2);

var _handleActions;

var _reduxActions = require('redux-actions');

var _packet = require('../actions/packet.js');

var _packet2 = _interopRequireDefault(_packet);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

exports.default = (0, _reduxActions.handleActions)((_handleActions = {}, (0, _defineProperty3.default)(_handleActions, _packet2.default.addPacket, function (state, action) {
  var received = state.received;
  if (received.length == state.capacity) {
    received = received.slice(1, received.length);
  }
  received.push(action.payload);
  return (0, _extends3.default)({}, state, { "received": received });
}), (0, _defineProperty3.default)(_handleActions, _packet2.default.removeAllPackets, function (state, action) {
  return (0, _extends3.default)({}, state, { "received": [] });
}), _handleActions), {});