import React from 'react';
import { Switch, Route } from 'react-router';
import { Container, Row, Col } from 'reactstrap';

import GraphPage from './containers/GraphPage';
import MsgPage from './containers/MsgPage';



export default (
	<Container id="mainContainer">
		<Row style={{height:"70%"}}>
			<Col style={{height:"100%"}}>
			  <Switch>
			    <Route exact path="/" component={GraphPage} />
			  </Switch>
			</Col>
		</Row>
		<Row style={{height:"30%"}}>
			<Col style={{height:"100%"}}>
				<Switch>
					<Route exact path="/" component={MsgPage} />
				</Switch>
			</Col>
		</Row>
	</Container>
);
