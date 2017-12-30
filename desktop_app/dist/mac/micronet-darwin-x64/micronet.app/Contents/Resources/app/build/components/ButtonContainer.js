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

var _reactstrap = require('reactstrap');

var _FlashButton = require('./FlashButton');

var _FlashButton2 = _interopRequireDefault(_FlashButton);

var _microbit = require('../microbit.js');

var _propTypes = require('prop-types');

var _propTypes2 = _interopRequireDefault(_propTypes);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var ButtonContainer = (_temp = _class = function (_Component) {
  (0, _inherits3.default)(ButtonContainer, _Component);

  function ButtonContainer(props) {
    (0, _classCallCheck3.default)(this, ButtonContainer);
    return (0, _possibleConstructorReturn3.default)(this, (ButtonContainer.__proto__ || (0, _getPrototypeOf2.default)(ButtonContainer)).call(this, props));
  }

  (0, _createClass3.default)(ButtonContainer, [{
    key: 'render',
    value: function render() {
      return _react2.default.createElement(
        'div',
        { id: 'buttonContainer' },
        _react2.default.createElement(
          _reactstrap.Row,
          { className: 'noMarginRow justify-content-centre' },
          _react2.default.createElement(
            _reactstrap.Button,
            { id: 'renameBtn', onClick: this.props.showRenameModal, className: 'controlButton' },
            'Rename Microbit'
          )
        ),
        _react2.default.createElement(
          _reactstrap.Row,
          { className: 'noMarginRow justify-content-centre' },
          _react2.default.createElement(_FlashButton2.default, { id: 'flashBtn', className: 'controlButton' })
        ),
        _react2.default.createElement(
          _reactstrap.Row,
          { className: 'noMarginRow justify-content-centre' },
          _react2.default.createElement(
            _reactstrap.Button,
            { id: 'helpBtn', onClick: this.props.onHelp, className: 'controlButton' },
            'Help'
          )
        )
      );
    }
  }]);
  return ButtonContainer;
}(_react.Component), _class.propTypes = {
  showRenameModal: _propTypes2.default.func.isRequired,
  onHelp: _propTypes2.default.func.isRequired
}, _temp);
exports.default = ButtonContainer;