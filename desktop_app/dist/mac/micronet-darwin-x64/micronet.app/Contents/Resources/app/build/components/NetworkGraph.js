'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = undefined;

var _getPrototypeOf = require('babel-runtime/core-js/object/get-prototype-of');

var _getPrototypeOf2 = _interopRequireDefault(_getPrototypeOf);

var _classCallCheck2 = require('babel-runtime/helpers/classCallCheck');

var _classCallCheck3 = _interopRequireDefault(_classCallCheck2);

var _createClass2 = require('babel-runtime/helpers/createClass');

var _createClass3 = _interopRequireDefault(_createClass2);

var _possibleConstructorReturn2 = require('babel-runtime/helpers/possibleConstructorReturn');

var _possibleConstructorReturn3 = _interopRequireDefault(_possibleConstructorReturn2);

var _inherits2 = require('babel-runtime/helpers/inherits');

var _inherits3 = _interopRequireDefault(_inherits2);

var _class, _temp;

var _react = require('react');

var _react2 = _interopRequireDefault(_react);

var _reactGraphVis = require('react-graph-vis');

var _reactGraphVis2 = _interopRequireDefault(_reactGraphVis);

var _propTypes = require('prop-types');

var _propTypes2 = _interopRequireDefault(_propTypes);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var NetworkGraph = (_temp = _class = function (_Component) {
  (0, _inherits3.default)(NetworkGraph, _Component);

  function NetworkGraph(props) {
    (0, _classCallCheck3.default)(this, NetworkGraph);
    return (0, _possibleConstructorReturn3.default)(this, (NetworkGraph.__proto__ || (0, _getPrototypeOf2.default)(NetworkGraph)).call(this, props));
  }

  (0, _createClass3.default)(NetworkGraph, [{
    key: 'render',
    value: function render() {
      return _react2.default.createElement(_reactGraphVis2.default, { graph: this.props.graph, options: this.props.options, events: this.props.events });
    }
  }]);
  return NetworkGraph;
}(_react.Component), _class.propTypes = {
  graph: _propTypes2.default.object.isRequired,
  options: _propTypes2.default.object.isRequired,
  events: _propTypes2.default.object.isRequired
}, _temp);
exports.default = NetworkGraph;