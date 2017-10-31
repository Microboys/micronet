import React, { Component } from 'react';
import Graph from 'react-graph-vis';

export default class NetworkGraph extends Component {
  
  constructor(props) {
    super(props);
    this.state = {
      graph: props.graph,
      options: {
        autoResize: true,
        width: '100%',
        height: '100%'
      },
      events: {}
    };
  }

  render() {
    return (
      <Graph graph={this.state.graph} options={this.state.options} events={this.state.events} />
    );
  }
}
