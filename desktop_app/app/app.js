import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux';
import { ConnectedRouter } from 'react-router-redux';
import { createMemoryHistory } from 'history';

import routes from './routes';
import configureStore from './store';
import { init } from './microbit.js';

//Setup temp folder for microbit images
const remote = require('electron').remote;
const fs = remote.require('fs');
const app = remote.app;
app.setPath('temp', app.getPath('appData') + '/micronet/temp');
try {
  var items = fs.readdirSync(app.getPath('temp'));
  for (var i = 0; i < items.length; i++) {
    var item = app.getPath('temp') + '/' + items[i];
    console.log('Deleting: ' + item);
    try {
      fs.unlinkSync(item);
    } catch(err) {
      fs.rmdirSync(item);
    }
  }
} catch(err) {
  console.log(err);
  fs.mkdirSync(app.getPath('temp'));
}

const syncHistoryWithStore = (store, history) => {
  const { routing } = store.getState();
  if(routing && routing.location) {
    history.replace(routing.location);
  }
};

const initialState = {
  'graph' : {
    'nodes' : [],
    'edges' : []
  },
  'dns' : {
    'entries' : []
  },
  'connection' : {
    'established' : false
  }
};

const routerHistory = createMemoryHistory();
const store = configureStore(initialState, routerHistory);
syncHistoryWithStore(store, routerHistory);

const rootElement = document.querySelector(document.currentScript.getAttribute('data-container'));

ReactDOM.render(
  <Provider store={store}>
    <ConnectedRouter history={routerHistory}>
      {routes}
    </ConnectedRouter>
  </Provider>,
  rootElement
);

init(store);
