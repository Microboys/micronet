'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = undefined;

var _stringify = require('babel-runtime/core-js/json/stringify');

var _stringify2 = _interopRequireDefault(_stringify);

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

var _PacketView = require('./PacketView');

var _PacketView2 = _interopRequireDefault(_PacketView);

var _NetworkGraph = require('./NetworkGraph');

var _NetworkGraph2 = _interopRequireDefault(_NetworkGraph);

var _FlashButton = require('./FlashButton');

var _FlashButton2 = _interopRequireDefault(_FlashButton);

var _ButtonContainer = require('./ButtonContainer');

var _ButtonContainer2 = _interopRequireDefault(_ButtonContainer);

var _reactstrap = require('reactstrap');

var _microbit = require('./../microbit.js');

var _propTypes = require('prop-types');

var _propTypes2 = _interopRequireDefault(_propTypes);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var defaultEdgeColor = '#848484';
var defaultEdgeWidth = 1.5;

var MainPage = (_temp = _class = function (_Component) {
  (0, _inherits3.default)(MainPage, _Component);

  function MainPage(props) {
    (0, _classCallCheck3.default)(this, MainPage);

    var _this = (0, _possibleConstructorReturn3.default)(this, (MainPage.__proto__ || (0, _getPrototypeOf2.default)(MainPage)).call(this, props));

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
          },
          width: defaultEdgeWidth
        },
        physics: {
          barnesHut: {
            springConstant: 0.001,
            gravitationalConstant: -20000
          }
        }
      },
      events: {
        doubleClick: function doubleClick(event) {
          if (event.nodes.length != 1) return;
          var node = event.nodes[0];
          _this.setState({ selectedNode: node });
          _this.toggleMsgModal();
        },
        click: function click(event) {
          if (event.nodes.length > 1) return;
          var node = event.nodes[0];
          var path = (0, _microbit.getRoute)(node);
          var newGraph = JSON.parse((0, _stringify2.default)(_this.state.graph));
          var edges = newGraph.edges;

          if (!path) {
            for (var i = 0; i < edges.length; i++) {
              edges[i].color = defaultEdgeColor;
              edges[i].width = defaultEdgeWidth;
            }
          } else {
            var edgesInPath = [];
            for (var _i = 0; _i < path.length - 1; _i++) {
              edgesInPath.push({ from: path[_i], to: path[_i + 1] });
            }

            for (var _i2 = 0; _i2 < edges.length; _i2++) {
              var edge = edges[_i2];
              if (_this.inPath(edge, edgesInPath)) {
                edge.color = { color: '#f44245', highlight: '#f44245', opacity: 1.0 };
                edge.width = 2 * defaultEdgeWidth;
              } else {
                edge.color = { color: defaultEdgeColor, opacity: 0.5 };
                edge.width = defaultEdgeWidth;
              }
            }
          }
          _this.setState({ graph: newGraph }, function () {
            console.log(_this.state.graph);
          });
        }
      },
      msgModal: false,
      selectedNode: "",
      msg: "",
      msgId: 'msgInput',
      nameModal: false,
      name: "",
      graph: props.graph
    };
    _this.handleChangeMsg = _this.handleChangeMsg.bind(_this);
    _this.handleSendMsg = _this.handleSendMsg.bind(_this);
    _this.toggleMsgModal = _this.toggleMsgModal.bind(_this);

    _this.handleChangeName = _this.handleChangeName.bind(_this);
    _this.handleRename = _this.handleRename.bind(_this);
    _this.toggleNameModal = _this.toggleNameModal.bind(_this);
    return _this;
  }

  (0, _createClass3.default)(MainPage, [{
    key: 'inPath',
    value: function inPath(edge, path) {
      for (var i = 0; i < path.length; i++) {
        if (edge.to == path[i].to && edge.from == path[i].from) {
          return true;
        }
      }
      return false;
    }
  }, {
    key: 'toggleMsgModal',
    value: function toggleMsgModal() {
      this.setState({ msgModal: !this.state.msgModal, msg: "" });
    }
  }, {
    key: 'handleChangeMsg',
    value: function handleChangeMsg(event) {
      this.setState({ msg: event.target.value });
    }
  }, {
    key: 'handleSendMsg',
    value: function handleSendMsg(event) {
      (0, _microbit.sendMsg)(this.state.selectedNode, this.state.msg);
      event.preventDefault();
      this.toggleMsgModal();
    }
  }, {
    key: 'toggleNameModal',
    value: function toggleNameModal() {
      this.setState({ nameModal: !this.state.nameModal });
    }
  }, {
    key: 'handleChangeName',
    value: function handleChangeName(event) {
      this.setState({ name: event.target.value });
    }
  }, {
    key: 'handleRename',
    value: function handleRename(event) {
      (0, _microbit.renameMicrobit)(this.state.name);
      event.preventDefault();
      this.toggleNameModal();
    }
  }, {
    key: 'componentWillReceiveProps',
    value: function componentWillReceiveProps(nextProps) {
      var graph = nextProps.graph;
      var edges = graph.edges;
      var oldEdges = this.state.graph.edges;
      for (var i = 0; i < edges.length; i++) {
        for (var j = 0; j < oldEdges.length; j++) {
          if (edges[i].to == oldEdges[j].to && edges[i].from == oldEdges[j].from) {
            edges[i].color = oldEdges[j].color;
            edges[i].width = oldEdges[j].width;
          }
        }
      }
      this.setState({ graph: nextProps.graph });
    }
  }, {
    key: 'render',
    value: function render() {
      if (!this.props.connection.established) {
        return _react2.default.createElement(
          _reactstrap.Container,
          { className: 'fullsize justify-content-center' },
          _react2.default.createElement(
            _reactstrap.Row,
            { className: 'justify-content-center noMarginRow halfHeight' },
            _react2.default.createElement(
              'h4',
              { id: 'searching' },
              'Looking for a router micro:bit - make sure it\'s flashed, connected and activated!'
            )
          ),
          _react2.default.createElement(
            _reactstrap.Row,
            { className: 'justify-content-center noMarginRow halfHeight' },
            _react2.default.createElement(_FlashButton2.default, { id: 'mainFlashBtn', className: 'flashBtn' })
          )
        );
      } else {
        return _react2.default.createElement(
          'div',
          { className: 'graph fullsize' },
          _react2.default.createElement(
            _reactstrap.Row,
            { id: 'mainRow', className: 'fullsize' },
            _react2.default.createElement(
              _reactstrap.Col,
              { xs: '9' },
              _react2.default.createElement(
                _reactstrap.Modal,
                { autoFocus: false, className: this.props.className, isOpen: this.state.msgModal, toggle: this.toggleMsgModal, fade: false },
                _react2.default.createElement(
                  _reactstrap.ModalHeader,
                  { toggle: this.toggleMsgModal },
                  'Send Message to ',
                  this.state.selectedNode
                ),
                _react2.default.createElement(
                  _reactstrap.ModalBody,
                  null,
                  _react2.default.createElement(
                    _reactstrap.Form,
                    { onSubmit: this.handleSendMsg },
                    _react2.default.createElement(_reactstrap.Input, { autoFocus: true, id: this.msgId, onChange: this.handleChangeMsg, value: this.state.msg })
                  )
                ),
                _react2.default.createElement(
                  _reactstrap.ModalFooter,
                  null,
                  _react2.default.createElement(
                    _reactstrap.Button,
                    { onClick: this.handleSendMsg, color: 'primary' },
                    'Send Message'
                  )
                )
              ),
              _react2.default.createElement(
                _reactstrap.Modal,
                { autoFocus: false, className: this.props.className, isOpen: this.state.nameModal, toggle: this.toggleNameModal, fade: false },
                _react2.default.createElement(
                  _reactstrap.ModalHeader,
                  { toggle: this.toggleNameModal },
                  'Rename Node'
                ),
                _react2.default.createElement(
                  _reactstrap.ModalBody,
                  null,
                  _react2.default.createElement(
                    'p',
                    null,
                    'Rename your microbit (may take a few seconds to update)!'
                  ),
                  _react2.default.createElement(
                    _reactstrap.Form,
                    { onSubmit: this.handleRename },
                    _react2.default.createElement(_reactstrap.Input, { autoFocus: true, onChange: this.handleChangeName, value: this.state.name })
                  )
                ),
                _react2.default.createElement(
                  _reactstrap.ModalFooter,
                  null,
                  _react2.default.createElement(
                    _reactstrap.Button,
                    { onClick: this.handleRename, color: 'primary' },
                    'Rename'
                  )
                )
              ),
              _react2.default.createElement(_NetworkGraph2.default, { graph: this.state.graph, options: this.state.options, events: this.state.events })
            ),
            _react2.default.createElement(
              _reactstrap.Col,
              { id: 'packetCol' },
              _react2.default.createElement(_PacketView2.default, { packets: this.props.packet.received }),
              _react2.default.createElement(_ButtonContainer2.default, { showRenameModal: this.toggleNameModal, onHelp: this.props.onHelp })
            )
          )
        );
      }
    }
  }]);
  return MainPage;
}(_react.Component), _class.propTypes = {
  onHelp: _propTypes2.default.func.isRequired
}, _temp);
exports.default = MainPage;