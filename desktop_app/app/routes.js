import React from 'react';
import { Switch, Route } from 'react-router';
import { Container, Row, Col } from 'reactstrap';

import GraphPage from './containers/GraphPage';
import HelpSlidesPage from './containers/HelpSlidesPage';

export default (
  <Container fluid={true} id="mainContainer" className='fullsize'>
    <Switch>
      <Route exact path="/graph" component={GraphPage} />
      <Route exact path="/" component={HelpSlidesPage} />
    </Switch>
  </Container>
);
