# micro:net desktop app

The desktop app used to interact with a router micro:bit over serial in order to visualise the network, send custom packets and more.

Based on boiilerplate for using [Electron](http://electron.atom.io/), [React](https://facebook.github.io/react/) and [Redux](http://redux.js.org/), taken from [this repo](https://raw.githubusercontent.com/jschr/electron-react-redux-boilerplate/master/README.md).

## Developing 

**At the moment, developing the GUI requires you to be able to build the router hex.
Please see instructions for setting that up in the router directory.**

To update the router.hex build used in the electron app, run:
```bash
sh update-router-hex.sh
```

From this directory, with node installed, run:
```bash
npm install
```

To launch locally, run:
```bash
npm run develop
```

To use DevTools:

* OSX: <kbd>Cmd</kbd> <kbd>Alt</kbd> <kbd>I</kbd> or <kbd>F12</kbd>
* Linux: <kbd>Ctrl</kbd> <kbd>Shift</kbd> <kbd>I</kbd> or <kbd>F12</kbd>
* Windows: <kbd>Ctrl</kbd> <kbd>Shift</kbd> <kbd>I</kbd> or <kbd>F12</kbd>

Create a package for OSX, Windows and Linux:
```
npm run pack
```

Or target a specific platform:
```
npm run pack:mac
npm run pack:win
npm run pack:linux
```

To run tests:
```
npm run test
```
