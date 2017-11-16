import React, { Component } from 'react';
import { Row, Col, ListGroup, ListGroupItem, Badge, Card, CardBody, CardHeader, CardText } from 'reactstrap';
import { removeLastPacket } from './../microbit.js';

export default class PacketView extends Component {
  
  constructor(props) {
    super(props);
  }

  render() {
    const packets = this.props.packets.reverse().map((packet, index) =>
      <Card key={index}>
        <CardHeader>
          {packet.ptype}
        </CardHeader>
        <CardBody className={packet.ptype}>
          <CardText></CardText>
        </CardBody>
      </Card>
    );

    return (
      <div id='packetContainer'>
        <div className='list-group'>
          {packets}
        </div>
      </div>
    );
  }
}
