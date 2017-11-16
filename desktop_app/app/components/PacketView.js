import React, { Component } from 'react';
import { Row, Col, ListGroup, ListGroupItem, Badge, Card, CardBody, CardHeader, CardText } from 'reactstrap';
import { lookupName } from './../microbit.js';

export default class PacketView extends Component {
  
  constructor(props) {
    super(props);
  }

  getTitle(packet) {
    let name = lookupName(packet.source_ip);
    let identifier = (name) ? name : "Node " + packet.source_ip;
    
    switch (packet.ptype) {
      case "LSA":
        return "Advert from " + identifier + "!";
      case "MSG":
        return "Message from " + identifier + "!";
      case "DNS":
        return "DNS update from " + identifier + "!";
      default:
        console.log("Unrecognised packet type: " + type);
        return "";
    }
  }

  getContent(packet) {
      switch (packet.ptype) {
        case "LSA":
          return <CardText>TODO</CardText>
        case "MSG":
          return <CardText>{packet.payload}</CardText>
        case "DNS":
          return <CardText>TODO</CardText>
        default:
          console.log("Unrecognised packet type: " + type);
          return <h1>Error</h1>
      }
    }

  render() {
    const filteredPackets = this.props.packets.reverse()
      .filter((packet) => 
	    packet.ptype === "DNS" ||
	    packet.ptype === "MSG"
      );
	    //packet.ptype === "LSA");
    const packetCards = filteredPackets
      .map((packet, index) =>
        <Card key={index}>
          <CardHeader>
            {this.getTitle(packet)}
          </CardHeader>
          <CardBody className={packet.ptype}>
            <CardText>{this.getContent(packet)}</CardText>
          </CardBody>
        </Card>
      );

    return (
      <div id='packetContainer'>
        <div className='list-group'>
          {packetCards}
        </div>
      </div>
    );
  }
}
