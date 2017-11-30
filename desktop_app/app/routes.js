import React from 'react';
import { Switch, Route } from 'react-router';
import { Container, Row, Col } from 'reactstrap';

import MainPage from './containers/MainPage';
import HelpSlidesPage from './containers/HelpSlidesPage';

export default (
  // TODO: change path back
  <Container fluid={true} id="mainContainer" className='fullsize'>
    <Switch>
      <Route exact path="/graph" component={MainPage} />
      <Route exact path="/" component={HelpSlidesPage} />
    </Switch>
  </Container>
);
