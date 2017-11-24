import React, { Component } from 'react';
import { Row, Col, ListGroup, ListGroupItem, Badge, Card, CardBody, CardHeader, CardText, Modal, ModalHeader, ModalBody, ModalFooter, Button } from 'reactstrap';
import { lookupName, transformGraphJSON } from './../microbit.js';
import NetworkGraph from './NetworkGraph';

export default class PacketView extends Component {
  
  constructor(props) {
    super(props);
    this.state = {
      options: {
        autoResize: true,
        width: '100%',
        height: '100%',
        nodes: {
          shape: 'image',
          image: './assets/microbit.png'
        },
        edges: {
          font: {
            align: 'horizontal',
            vadjust: -10
          }
        },
        physics: {
          enabled: false
        },
        interaction: {
          zoomView: false,
          hideEdgesOnDrag: true
        }
      },
      modalOptions: {
        autoResize: true,
        width: '100%',
        height: '100%',
        nodes: {
          shape: 'image',
          image: './assets/microbit.png'
        },
        edges: {
          font: {
            align: 'horizontal',
            vadjust: -10
          }
        },
        physics: {
          barnesHut: {
            springConstant: 0.001,
            gravitationalConstant: -20000
          }
        }
      },
      events: {},
      graphModal: false,
      selectedGraph: {
        nodes: [],
        edges: []
      },
      selectedHeader: null
    }
    this.toggleGraphModal = this.toggleGraphModal.bind(this);
    this.graphOnClick = this.graphOnClick.bind(this);
  }

  toggleGraphModal() {
    this.setState({graphModal: !this.state.graphModal});
  }

  graphOnClick(graph, header) {
    return (event) => {
      this.toggleGraphModal();
      this.setState({selectedGraph: graph});
      this.setState({selectedHeader: header});
    };
  }

  getTitle(packet) {
    let name = lookupName(packet.source_ip);
    let identifier = (name) ? name : "Node " + packet.source_ip;
    
    switch (packet.ptype) {
      case "LSA":
        return "Advert from " + identifier + "!";
      case "MSG":
        return "Message from " + identifier + "!";
      case "NEIGHBOUR_DISCOVERED":
        return "Found a new neighbour!";
      case "ROUTER_TIMED_OUT":
        return "Lost contact with a router.";
      case "DNS":
        return "DNS update from " + identifier + "!";
      default:
        console.log("Unrecognised packet type: " + type);
        return "";
    }
  }

  getContent(packet) {
    let name = lookupName(packet.source_ip);
    let identifier = (name) ? name : "Node " + packet.source_ip;

    switch (packet.ptype) {
      case "LSA":
        const graph = transformGraphJSON(packet.payload);
        const onClick = this.graphOnClick(graph, this.getTitle(packet));
        return <NetworkGraph graph={transformGraphJSON(packet.payload)} options={this.state.options} events={{click: onClick}} />
      case "MSG":
        return <CardText>{packet.payload}</CardText>
      case "NEIGHBOUR_DISCOVERED":
        return <CardText>We can see {identifier}</CardText>
      case "ROUTER_TIMED_OUT":
        return <CardText>We did not hear anything from {identifier}</CardText>
      case "DNS":
        return <CardText>TODO</CardText>
      default:
        console.log("Unrecognised packet type: " + type);
        return <h1>Error</h1>
    }
  }

  render() {
    var filteredPackets = this.props.packets.reverse()
      .filter((packet) => 
	    packet.ptype === "DNS" ||
	    packet.ptype === "LSA" ||
	    packet.ptype === "NEIGHBOUR_DISCOVERED" ||
	    packet.ptype === "ROUTER_TIMED_OUT" ||
	    packet.ptype === "MSG");
    filteredPackets.sort((a, b) => {
      return (b.time - a.time);
    });
    const packetCards = filteredPackets
      .map((packet, index) => {
        return (
          <Card key={index}>
            <CardHeader>
              {this.getTitle(packet)}
            </CardHeader>
            <CardBody className={packet.ptype}>
              {this.getContent(packet)}
            </CardBody>
          </Card>
          );
        }
      );

    return (
      <div id='packetContainer'>
        <Modal autoFocus={false} className='modal-lg' isOpen={this.state.graphModal} toggle={this.toggleGraphModal} fade={false}>
          <ModalHeader toggle={this.toggleGraphModal}>{this.state.selectedHeader}</ModalHeader>
          <ModalBody id='graphModalBody'>
            <NetworkGraph graph={this.state.selectedGraph} options={this.state.modalOptions} events={this.state.events} />
          </ModalBody>
        </Modal>
        <div className='list-group'>
          {packetCards}
        </div>
      </div>
    );
  }
}
