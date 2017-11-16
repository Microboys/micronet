import React, { Component } from 'react';
import { Row, Col, ListGroup, ListGroupItem, Badge } from 'reactstrap';
import { removeLastPacket } from './../microbit.js';

export default class PacketView extends Component {
  
  constructor(props) {
    super(props);
  }

  render() {
    const packets = this.props.packets.map((packet, index) =>
      <ListGroupItem key={index}>{packet.ptype}</ListGroupItem>
    );

    return (
      <ListGroup>
	{packets}
      </ListGroup>
    );
  }
}
