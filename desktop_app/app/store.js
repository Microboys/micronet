import { createStore, applyMiddleware, combineReducers, compose } from 'redux';
import { routerMiddleware, routerReducer as routing, push } from 'react-router-redux';
import thunk from 'redux-thunk';

import graph from './reducers/graph';
import graphActions from './actions/graph';
import connection from './reducers/connection';
import connectionActions from './actions/connection';
import packet from './reducers/packet';
import packetActions from './actions/packet';
import dns from './reducers/dns';
import dnsActions from './actions/dns';
import sinkTree from './reducers/sinkTree';
import sinkTreeActions from './actions/sinkTree';

export default function configureStore(initialState, routerHistory) {
  const router = routerMiddleware(routerHistory);

  const actionCreators = {
    ...graphActions,
    ...dnsActions,
    ...connectionActions,
    ...graphActions,
    ...packetActions,
    ...sinkTreeActions,
    push
  };

  const reducers = {
    graph,
    dns,
    connection,
    graph,
    packet,
    sinkTree,
    routing
  };

  const middlewares = [ thunk, router ];

  const composeEnhancers = (() => {
    const compose_ = window && window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__;
    if(process.env.NODE_ENV === 'development' && compose_) {
      return compose_({ actionCreators });
    }
    return compose;
  })();

  const enhancer = composeEnhancers(applyMiddleware(...middlewares));
  const rootReducer = combineReducers(reducers);

  return createStore(rootReducer, initialState, enhancer);
}
