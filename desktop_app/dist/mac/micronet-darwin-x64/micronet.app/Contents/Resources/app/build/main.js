'use strict';

var _regenerator = require('babel-runtime/regenerator');

var _regenerator2 = _interopRequireDefault(_regenerator);

var _getIterator2 = require('babel-runtime/core-js/get-iterator');

var _getIterator3 = _interopRequireDefault(_getIterator2);

var _asyncToGenerator2 = require('babel-runtime/helpers/asyncToGenerator');

var _asyncToGenerator3 = _interopRequireDefault(_asyncToGenerator2);

var _path = require('path');

var _path2 = _interopRequireDefault(_path);

var _url = require('url');

var _url2 = _interopRequireDefault(_url);

var _electron = require('electron');

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var isDevelopment = process.env.NODE_ENV === 'development';

var mainWindow = null;
var forceQuit = false;
var installExtensions = function () {
  var _ref = (0, _asyncToGenerator3.default)(_regenerator2.default.mark(function _callee() {
    var installer, extensions, forceDownload, _iteratorNormalCompletion, _didIteratorError, _iteratorError, _iterator, _step, name;

    return _regenerator2.default.wrap(function _callee$(_context) {
      while (1) {
        switch (_context.prev = _context.next) {
          case 0:
            installer = require('electron-devtools-installer');
            extensions = ['REACT_DEVELOPER_TOOLS', 'REDUX_DEVTOOLS'];
            forceDownload = !!process.env.UPGRADE_EXTENSIONS;
            _iteratorNormalCompletion = true;
            _didIteratorError = false;
            _iteratorError = undefined;
            _context.prev = 6;
            _iterator = (0, _getIterator3.default)(extensions);

          case 8:
            if (_iteratorNormalCompletion = (_step = _iterator.next()).done) {
              _context.next = 21;
              break;
            }

            name = _step.value;
            _context.prev = 10;
            _context.next = 13;
            return installer.default(installer[name], forceDownload);

          case 13:
            _context.next = 18;
            break;

          case 15:
            _context.prev = 15;
            _context.t0 = _context['catch'](10);

            console.log('Error installing ' + name + ' extension: ' + _context.t0.message);

          case 18:
            _iteratorNormalCompletion = true;
            _context.next = 8;
            break;

          case 21:
            _context.next = 27;
            break;

          case 23:
            _context.prev = 23;
            _context.t1 = _context['catch'](6);
            _didIteratorError = true;
            _iteratorError = _context.t1;

          case 27:
            _context.prev = 27;
            _context.prev = 28;

            if (!_iteratorNormalCompletion && _iterator.return) {
              _iterator.return();
            }

          case 30:
            _context.prev = 30;

            if (!_didIteratorError) {
              _context.next = 33;
              break;
            }

            throw _iteratorError;

          case 33:
            return _context.finish(30);

          case 34:
            return _context.finish(27);

          case 35:
          case 'end':
            return _context.stop();
        }
      }
    }, _callee, undefined, [[6, 23, 27, 35], [10, 15], [28,, 30, 34]]);
  }));

  return function installExtensions() {
    return _ref.apply(this, arguments);
  };
}();

_electron.crashReporter.start({
  productName: 'YourName',
  companyName: 'YourCompany',
  submitURL: 'https://your-domain.com/url-to-submit',
  uploadToServer: false
});

_electron.app.on('window-all-closed', function () {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    _electron.app.quit();
  }
});

_electron.app.on('ready', (0, _asyncToGenerator3.default)(_regenerator2.default.mark(function _callee2() {
  return _regenerator2.default.wrap(function _callee2$(_context2) {
    while (1) {
      switch (_context2.prev = _context2.next) {
        case 0:
          if (!isDevelopment) {
            _context2.next = 3;
            break;
          }

          _context2.next = 3;
          return installExtensions();

        case 3:

          mainWindow = new _electron.BrowserWindow({
            show: false,
            //frame: false,
            backgroundColor: 0x000000,
            darkTheme: true,
            webPreferences: {
              zoomFactor: 1
            }
          });

          mainWindow.loadURL(_url2.default.format({
            pathname: _path2.default.join(__dirname, 'index.html'),
            protocol: 'file:',
            slashes: true
          }));

          // show window once on first load
          mainWindow.webContents.once('did-finish-load', function () {
            mainWindow.show();
          });

          mainWindow.webContents.on('did-finish-load', function () {
            // Handle window logic properly on macOS:
            // 1. App should not terminate if window has been closed
            // 2. Click on icon in dock should re-open the window
            // 3. ⌘+Q should close the window and quit the app
            if (process.platform === 'darwin') {
              mainWindow.on('close', function (e) {
                if (!forceQuit) {
                  e.preventDefault();
                  mainWindow.hide();
                }
              });

              _electron.app.on('activate', function () {
                mainWindow.show();
              });

              _electron.app.on('before-quit', function () {
                forceQuit = true;
              });
            } else {
              mainWindow.on('closed', function () {
                mainWindow = null;
              });
            }
          });
          mainWindow.maximize();

          //For testing packaged app
          // mainWindow.webContents.openDevTools();

          if (isDevelopment) {
            // auto-open dev tools
            mainWindow.webContents.openDevTools();

            // add inspect element on right click menu
            mainWindow.webContents.on('context-menu', function (e, props) {
              _electron.Menu.buildFromTemplate([{
                label: 'Inspect element',
                click: function click() {
                  mainWindow.inspectElement(props.x, props.y);
                }
              }]).popup(mainWindow);
            });
          }

        case 9:
        case 'end':
          return _context2.stop();
      }
    }
  }, _callee2, undefined);
})));