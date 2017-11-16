import React, { Component } from 'react';
import Graph from 'react-graph-vis';
import PacketView from './PacketView';
import { Row, Col, Container, Modal, ModalHeader, ModalBody, ModalFooter, Input, Form, Button, ListGroup, ListGroupItem, Badge } from 'reactstrap';
import { sendMsg, renameMicrobit } from './../microbit.js';

export default class NetworkGraph extends Component {
  
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
          barnesHut: {
            springConstant: 0.001,
            gravitationalConstant: -20000
          }
        }
      },
      events: {
        doubleClick: (event) => {
          if (event.nodes.length != 1) return;
          var node = event.nodes[0];
          this.setState({selectedNode: node});
          this.toggleMsgModal();
        }
      },
      msgModal: false,
      selectedNode: "",
      msg: "",
      msgId:'msgInput',
      nameModal: false,
      name: ""
    };
    this.handleChangeMsg = this.handleChangeMsg.bind(this);
    this.handleSendMsg = this.handleSendMsg.bind(this);
    this.toggleMsgModal = this.toggleMsgModal.bind(this);

    this.handleChangeName = this.handleChangeName.bind(this);
    this.handleRename = this.handleRename.bind(this);
    this.toggleNameModal = this.toggleNameModal.bind(this);
  }

  toggleMsgModal() {
    this.setState({msgModal: !this.state.msgModal, msg: ""});
  }

  handleChangeMsg(event) {
    this.setState({msg: event.target.value});
  }

  handleSendMsg(event) {
    sendMsg(this.state.selectedNode, this.state.msg);
    event.preventDefault();
    this.toggleMsgModal();
  }

  toggleNameModal() {
    this.setState({nameModal: !this.state.nameModal});
  }

  handleChangeName(event) {
    this.setState({name: event.target.value});
  }

  handleRename(event) {
    renameMicrobit(this.state.name);
    event.preventDefault();
    this.toggleNameModal();
  }

  render() {
    if (!this.props.connection.established) {
      return (
        <Row className='fullsize justify-content-center'>
          <Col className='align-self-center'>
            <center>
              <b>Looking for a router micro:bit - make sure it's flashed, connected and activated!</b>
            </center>
          </Col>
        </Row>
      );
    } else {
      return (
	<div className="graph fullsize">
	  <Row className="fullsize">
	    <Col xs="9">
              <Modal autoFocus={false} className={this.props.className} isOpen={this.state.msgModal} toggle={this.toggleMsgModal} fade={false}>
                <ModalHeader toggle={this.toggleMsgModal}>Send Message to {this.state.selectedNode}</ModalHeader>
                <ModalBody>
                  <Form onSubmit={this.handleSendMsg}>
                    <Input autoFocus={true} id={this.msgId} onChange={this.handleChangeMsg} value={this.state.msg} />
                  </Form>
                </ModalBody>
                <ModalFooter>
                  <Button onClick={this.handleSendMsg} color='primary'>Send Message</Button>
                </ModalFooter>
              </Modal>
              <Modal autoFocus={false} className={this.props.className} isOpen={this.state.nameModal} toggle={this.toggleNameModal} fade={false}>
                <ModalHeader toggle={this.toggleNameModal}>Rename Node</ModalHeader>
                <ModalBody>
                  <Form onSubmit={this.handleRename}>
                    <Input autoFocus={true} onChange={this.handleChangeName} value={this.state.name} />
                  </Form>
                </ModalBody>
                <ModalFooter>
                  <Button onClick={this.handleRename} color='primary'>Rename</Button>
                </ModalFooter>
              </Modal>
              <Graph graph={this.props.graph} options={this.state.options} events={this.state.events} />
	    </Col>
	    <Col>
	      <PacketView packets={this.props.packet.received} />
	    </Col>
	  </Row>
	</div>
      );
    }
  }
}
