import React from 'react';
import { Switch, Route } from 'react-router';

import GraphPage from './containers/GraphPage';

export default (
  <Switch>
    <Route exact path="/" component={GraphPage} />
  </Switch>
);
