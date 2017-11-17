import React, { Component } from 'react';
import Graph from 'react-graph-vis';
import PropTypes from 'prop-types';

export default class NetworkGraph extends Component {
  
  static propTypes = {
    graph: PropTypes.object.isRequired,
    options: PropTypes.object.isRequired,
    events: PropTypes.object.isRequired
  };

  constructor(props) {
    super(props);
  }

  render() {
    return (
      <Graph graph={this.props.graph} options={this.props.options} events={this.props.events} />
    );
  }
}
