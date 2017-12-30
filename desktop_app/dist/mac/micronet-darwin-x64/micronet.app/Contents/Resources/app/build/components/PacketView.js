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

var _react = require('react');

var _react2 = _interopRequireDefault(_react);

var _reactstrap = require('reactstrap');

var _microbit = require('./../microbit.js');

var _NetworkGraph = require('./NetworkGraph');

var _NetworkGraph2 = _interopRequireDefault(_NetworkGraph);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var PacketView = function (_Component) {
  (0, _inherits3.default)(PacketView, _Component);

  function PacketView(props) {
    (0, _classCallCheck3.default)(this, PacketView);

    var _this = (0, _possibleConstructorReturn3.default)(this, (PacketView.__proto__ || (0, _getPrototypeOf2.default)(PacketView)).call(this, props));

    _this.state = {
      options: {
        autoResize: true,
        width: '100%',
        height: '100%',
        nodes: {
          shape: 'image',
          image: './assets/microbit.png'
        },
        edges: {
          font: {
            align: 'horizontal',
            vadjust: -10
          }
        },
        physics: {
          enabled: false
        },
        interaction: {
          zoomView: false,
          hideEdgesOnDrag: true
        }
      },
      modalOptions: {
        autoResize: true,
        width: '100%',
        height: '100%',
        nodes: {
          shape: 'image',
          image: './assets/microbit.png'
        },
        edges: {
          font: {
            align: 'horizontal',
            vadjust: -10
          }
        },
        physics: {
          barnesHut: {
            springConstant: 0.001,
            gravitationalConstant: -20000
          }
        }
      },
      events: {},
      graphModal: false,
      selectedGraph: {
        nodes: [],
        edges: []
      },
      selectedHeader: null
    };
    _this.toggleGraphModal = _this.toggleGraphModal.bind(_this);
    _this.graphOnClick = _this.graphOnClick.bind(_this);
    return _this;
  }

  (0, _createClass3.default)(PacketView, [{
    key: 'toggleGraphModal',
    value: function toggleGraphModal() {
      this.setState({ graphModal: !this.state.graphModal });
    }
  }, {
    key: 'graphOnClick',
    value: function graphOnClick(graph, header) {
      var _this2 = this;

      return function (event) {
        _this2.toggleGraphModal();
        _this2.setState({ selectedGraph: graph });
        _this2.setState({ selectedHeader: header });
      };
    }
  }, {
    key: 'getTitle',
    value: function getTitle(packet) {
      var name = (0, _microbit.lookupName)(packet.source_ip);
      var identifier = name ? name : "Node " + packet.source_ip;

      switch (packet.ptype) {
        case "LSA":
          return "Advert from " + identifier + "!";
        case "MSG":
          return "Message from " + identifier + "!";
        case "NEIGHBOUR_DISCOVERED":
          return "Found a new neighbour!";
        case "ROUTER_TIMED_OUT":
          return "Lost contact with a router.";
        case "DNS":
          return "DNS update from " + identifier + "!";
        default:
          console.log("Unrecognised packet type: " + type);
          return "";
      }
    }
  }, {
    key: 'getContent',
    value: function getContent(packet) {
      var name = (0, _microbit.lookupName)(packet.source_ip);
      var identifier = name ? name : "Node " + packet.source_ip;

      switch (packet.ptype) {
        case "LSA":
          var graph = (0, _microbit.transformGraphJSON)(packet.payload);
          var onClick = this.graphOnClick(graph, this.getTitle(packet));
          return _react2.default.createElement(_NetworkGraph2.default, { graph: (0, _microbit.transformGraphJSON)(packet.payload), options: this.state.options, events: { click: onClick } });
        case "MSG":
          return _react2.default.createElement(
            _reactstrap.CardText,
            null,
            packet.payload
          );
        case "NEIGHBOUR_DISCOVERED":
          return _react2.default.createElement(
            _reactstrap.CardText,
            null,
            'We can see ',
            identifier
          );
        case "ROUTER_TIMED_OUT":
          return _react2.default.createElement(
            _reactstrap.CardText,
            null,
            'We did not hear anything from ',
            identifier
          );
        case "DNS":
          return _react2.default.createElement(
            _reactstrap.CardText,
            null,
            'TODO'
          );
        default:
          console.log("Unrecognised packet type: " + type);
          return _react2.default.createElement(
            'h1',
            null,
            'Error'
          );
      }
    }
  }, {
    key: 'render',
    value: function render() {
      var _this3 = this;

      var filteredPackets = this.props.packets.reverse().filter(function (packet) {
        return packet.ptype === "DNS" || packet.ptype === "LSA" || packet.ptype === "NEIGHBOUR_DISCOVERED" || packet.ptype === "ROUTER_TIMED_OUT" || packet.ptype === "MSG";
      });
      filteredPackets.sort(function (a, b) {
        return b.time - a.time;
      });
      var packetCards = filteredPackets.map(function (packet, index) {
        return _react2.default.createElement(
          _reactstrap.Card,
          { key: index },
          _react2.default.createElement(
            _reactstrap.CardHeader,
            null,
            _this3.getTitle(packet)
          ),
          _react2.default.createElement(
            _reactstrap.CardBody,
            { className: packet.ptype },
            _this3.getContent(packet)
          )
        );
      });

      return _react2.default.createElement(
        'div',
        { id: 'packetContainer' },
        _react2.default.createElement(
          _reactstrap.Modal,
          { autoFocus: false, className: 'modal-lg', isOpen: this.state.graphModal, toggle: this.toggleGraphModal, fade: false },
          _react2.default.createElement(
            _reactstrap.ModalHeader,
            { toggle: this.toggleGraphModal },
            this.state.selectedHeader
          ),
          _react2.default.createElement(
            _reactstrap.ModalBody,
            { id: 'graphModalBody' },
            _react2.default.createElement(_NetworkGraph2.default, { graph: this.state.selectedGraph, options: this.state.modalOptions, events: this.state.events })
          )
        ),
        _react2.default.createElement(
          'div',
          { className: 'list-group' },
          packetCards
        )
      );
    }
  }]);
  return PacketView;
}(_react.Component);

exports.default = PacketView;