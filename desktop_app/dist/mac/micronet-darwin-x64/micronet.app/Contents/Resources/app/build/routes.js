'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _react = require('react');

var _react2 = _interopRequireDefault(_react);

var _reactRouter = require('react-router');

var _reactstrap = require('reactstrap');

var _MainPage = require('./containers/MainPage');

var _MainPage2 = _interopRequireDefault(_MainPage);

var _HelpSlidesPage = require('./containers/HelpSlidesPage');

var _HelpSlidesPage2 = _interopRequireDefault(_HelpSlidesPage);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

exports.default =
// TODO: change path back
_react2.default.createElement(
  _reactstrap.Container,
  { fluid: true, id: 'mainContainer', className: 'fullsize' },
  _react2.default.createElement(
    _reactRouter.Switch,
    null,
    _react2.default.createElement(_reactRouter.Route, { exact: true, path: '/graph', component: _MainPage2.default }),
    _react2.default.createElement(_reactRouter.Route, { exact: true, path: '/', component: _HelpSlidesPage2.default })
  )
);