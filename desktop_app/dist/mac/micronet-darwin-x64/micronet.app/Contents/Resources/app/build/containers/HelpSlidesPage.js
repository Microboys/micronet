'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _reactRedux = require('react-redux');

var _reactRouterRedux = require('react-router-redux');

var _HelpSlides = require('../components/HelpSlides');

var _HelpSlides2 = _interopRequireDefault(_HelpSlides);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var mapStateToProps = function mapStateToProps(state) {
  return state;
};

var mapDispatchToProps = function mapDispatchToProps(dispatch) {
  // eslint-disable-line no-unused-vars
  return {
    onSkip: function onSkip() {
      dispatch((0, _reactRouterRedux.push)('/graph'));
    }
  };
};

exports.default = (0, _reactRedux.connect)(mapStateToProps, mapDispatchToProps)(_HelpSlides2.default);