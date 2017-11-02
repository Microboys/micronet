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
        }
      },
      events: {}
    };
  }

  render() {
    return (
      <Graph graph={this.props.graph} options={this.state.options} events={this.state.events} />
    );
  }
}
