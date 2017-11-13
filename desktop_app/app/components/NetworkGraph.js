import React, { Component } from 'react';
import Graph from 'react-graph-vis';

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
      events: {}
    };
  }

  render() {
    if (!this.props.connection.established) {
      return (
	<h1>Looking for a router micro:bit..</h1>
      );
    } else {
      return (
        <Graph graph={this.props.graph} options={this.state.options} events={this.state.events} />
      );
    }
  }
}
