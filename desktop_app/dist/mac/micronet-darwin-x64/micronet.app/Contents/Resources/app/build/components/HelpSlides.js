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

var _propTypes = require('prop-types');

var _propTypes2 = _interopRequireDefault(_propTypes);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var HelpSlides = (_temp = _class = function (_Component) {
  (0, _inherits3.default)(HelpSlides, _Component);

  function HelpSlides(props) {
    (0, _classCallCheck3.default)(this, HelpSlides);

    var _this = (0, _possibleConstructorReturn3.default)(this, (HelpSlides.__proto__ || (0, _getPrototypeOf2.default)(HelpSlides)).call(this, props));

    _this.state = {
      items: [{
        src: './assets/slide_1.svg',
        caption: ''
      }, {
        src: './assets/slide_2.svg',
        caption: ''
      }, {
        src: './assets/slide_3.svg',
        caption: ''
      }, {
        src: './assets/slide_4.svg',
        caption: ''
      }, {
        src: './assets/slide_5.svg',
        caption: ''
      }, {
        src: './assets/slide_6.svg',
        caption: ''
      }]
    };
    return _this;
  }

  (0, _createClass3.default)(HelpSlides, [{
    key: 'render',
    value: function render() {
      return _react2.default.createElement(
        'div',
        { className: 'fullsize' },
        _react2.default.createElement(
          _reactstrap.Row,
          { id: 'carouselRow', className: 'justify-content-center noMarginRow' },
          _react2.default.createElement(
            _reactstrap.Col,
            { className: 'align-self-center' },
            _react2.default.createElement(_reactstrap.UncontrolledCarousel, { items: this.state.items })
          )
        ),
        _react2.default.createElement(
          _reactstrap.Row,
          { id: 'skipRow', className: 'justify-content-center noMarginRow' },
          _react2.default.createElement(
            _reactstrap.Button,
            { autoFocus: true, id: 'skipButton', onClick: this.props.onSkip },
            'I\'m Ready'
          )
        )
      );
    }
  }]);
  return HelpSlides;
}(_react.Component), _class.propTypes = {
  onSkip: _propTypes2.default.func.isRequired
}, _temp);
exports.default = HelpSlides;