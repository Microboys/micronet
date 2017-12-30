'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _reactRedux = require('react-redux');

var _reactRouterRedux = require('react-router-redux');

var _MainPage = require('../components/MainPage');

var _MainPage2 = _interopRequireDefault(_MainPage);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var mapStateToProps = function mapStateToProps(state) {
  return state;
};

var mapDispatchToProps = function mapDispatchToProps(dispatch) {
  // eslint-disable-line no-unused-vars
  return {
    onHelp: function onHelp() {
      dispatch((0, _reactRouterRedux.push)('/'));
    }
  };
};

exports.default = (0, _reactRedux.connect)(mapStateToProps, mapDispatchToProps)(_MainPage2.default);