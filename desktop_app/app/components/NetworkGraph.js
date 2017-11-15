import React, { Component } from 'react';
import Graph from 'react-graph-vis';
import { Container, Modal, ModalHeader, ModalBody, ModalFooter, Input, Form, Button } from 'reactstrap';
import { sendMsg } from './../microbit.js';

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
          this.setState({modal: !this.state.modal, selectedNode: node});
        }
      },
      modal: false,
      selectedNode: "",
      msg: "",
      msgId:'msgInput'
    };
    this.handleChangeMsg = this.handleChangeMsg.bind(this);
    this.handleSend = this.handleSend.bind(this);
    this.toggleModal = this.toggleModal.bind(this);
    this.focusMsg = this.focusMsg.bind(this);
  }

  toggleModal() {
    this.setState({modal: !this.state.modal, msg: ""});
  }

  handleChangeMsg(event) {
    this.setState({msg: event.target.value});
  }

  handleSend(event) {
    sendMsg(this.state.to, this.state.msg);
    event.preventDefault();
    this.toggleModal();
  }

  focusMsg() {
    this.msgInput.focus();
  }

  render() {
    if (!this.props.connection.established) {
      return (
	  <h1>Looking for a router micro:bit..</h1>
      );
    } else {
      return (
	<div className="graph">
        <Modal className={this.props.className} isOpen={this.state.modal} toggle={this.toggleModal} fade={false} onEnter={this.focusMsg}>
          <ModalHeader toggle={this.toggleModal}>Send Message to {this.state.selectedNode}</ModalHeader>
          <ModalBody>
            <Form onSubmit={this.handleSend}>
              <Input id={this.msgId} onChange={this.handleChangeMsg} value={this.state.msg} getRef={(input) => {this.msgInput = input;}}/>
            </Form>
          </ModalBody>
          <ModalFooter>
            <Button onClick={this.handleSend} color='primary'>Send Message</Button>
          </ModalFooter>
	</Modal>
        <Graph graph={this.props.graph} options={this.state.options} events={this.state.events} />
	</div>
      );
    }
  }
}
