import React, { Component } from 'react'
import PacketView from './PacketView';
import NetworkGraph from './NetworkGraph';
import FlashButton from './FlashButton';
import ButtonContainer from './ButtonContainer';
import { Row, Col, Container, Modal, ModalHeader, ModalBody, ModalFooter, Input, Form, Button, ListGroup, ListGroupItem, Badge } from 'reactstrap';
import { sendMsg, renameMicrobit, getRoute } from './../microbit.js';
import PropTypes from 'prop-types';

const defaultEdgeColor = '#848484';
const defaultEdgeWidth = 1.5;

export default class MainPage extends Component {
  
  static propTypes = {
    onHelp: PropTypes.func.isRequired
  };

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
          },
          width: defaultEdgeWidth
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
        },
        click: (event) => {
          if (event.nodes.length > 1) return;
          let node = event.nodes[0];
          let path = getRoute(node);
          let newGraph = JSON.parse(JSON.stringify(this.state.graph));
          let edges = newGraph.edges;

          if (!path) {
            for (let i = 0; i < edges.length; i++) {
              edges[i].color = defaultEdgeColor;
              edges[i].width = defaultEdgeWidth;
            }
          } else {
            let edgesInPath = [];
            console.log(path);
            for (let i = 0; i < path.length - 1; i++) {
              edgesInPath.push({from: path[i], to: path[i + 1]});
            }

            for (let i = 0; i < edges.length; i++){
              let edge = edges[i];
              if (this.inPath(edge, edgesInPath)) {
                edge.color = {color: '#f44245', highlight: '#f44245', opacity: 1.0};
                edge.width = 2 * defaultEdgeWidth;
              } else {
                edge.color = {color: defaultEdgeColor, opacity: 0.5};
                edge.width = defaultEdgeWidth;
              }
            }
          }
          this.setState({graph: newGraph}, () => {console.log(this.state.graph);});
        }
      },
      msgModal: false,
      selectedNode: "",
      msg: "",
      msgId:'msgInput',
      nameModal: false,
      name: "",
      graph: props.graph
    };
    this.handleChangeMsg = this.handleChangeMsg.bind(this);
    this.handleSendMsg = this.handleSendMsg.bind(this);
    this.toggleMsgModal = this.toggleMsgModal.bind(this);

    this.handleChangeName = this.handleChangeName.bind(this);
    this.handleRename = this.handleRename.bind(this);
    this.toggleNameModal = this.toggleNameModal.bind(this);
  }

  inPath(edge, path) {
    for (var i = 0; i < path.length; i++){
      if (edge.to == path[i].to && edge.from == path[i].from) {
        return true;
      }
    }
    return false;
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

  componentWillReceiveProps(nextProps) {
    let graph = nextProps.graph;
    let edges = graph.edges;
    let oldEdges = this.state.graph.edges;
    for (let i = 0; i < edges.length; i++) {
      for (let j = 0; j < oldEdges.length; j++) {
        if (edges[i].to == oldEdges[j].to && edges[i].from == oldEdges[j].from) {
          edges[i].color = oldEdges[j].color;
          edges[i].width = oldEdges[j].width;
        }
      }
    }
    this.setState({graph : nextProps.graph});
  }

  render() {
    if (!this.props.connection.established) {
      return (
        <Container className='fullsize justify-content-center'>
          <Row className='justify-content-center noMarginRow halfHeight'>
            <h4 id='searching'>Looking for a router micro:bit - make sure it's flashed, connected and activated!</h4>
          </Row>
          <Row className='justify-content-center noMarginRow halfHeight'>
            <FlashButton id='mainFlashBtn' className='flashBtn' />
          </Row>
        </Container>
      );
    } else {
      return (
        <div className="graph fullsize">
          <Row id="mainRow" className="fullsize">
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
                  <p>Rename your microbit (may take a few seconds to update)!</p>
                  <Form onSubmit={this.handleRename}>
                    <Input autoFocus={true} onChange={this.handleChangeName} value={this.state.name} />
                  </Form>
                </ModalBody>
                <ModalFooter>
                  <Button onClick={this.handleRename} color='primary'>Rename</Button>
                </ModalFooter>
              </Modal>
              <NetworkGraph graph={this.state.graph} options={this.state.options} events={this.state.events} />
            </Col>
            <Col id='packetCol'>
              <PacketView packets={this.props.packet.received} />
              <ButtonContainer showRenameModal={this.toggleNameModal} onHelp={this.props.onHelp} />
            </Col>
          </Row>
        </div>
      );
    }
  }
}