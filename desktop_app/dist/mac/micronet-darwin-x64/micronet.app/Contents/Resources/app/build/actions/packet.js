'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _reduxActions = require('redux-actions');

exports.default = {
  addPacket: (0, _reduxActions.createAction)('ADD_PACKET'),
  removeOldestPacket: (0, _reduxActions.createAction)('REMOVE_OLDEST_PACKET'),
  removeAllPackets: (0, _reduxActions.createAction)('REMOVE_ALL_PACKETS')
};