import React, { Component } from 'react';
import { Button } from 'reactstrap';
import { flashMicrobit } from '../microbit.js'
import PropTypes from 'prop-types';

export default class FlashButton extends Component {
  
  constructor(props) {
    super(props);
  };

  render() {
    return (
      <Button id={this.props.id} onClick={flashMicrobit} className={this.props.className}>Flash Microbit</Button>
    );
  }
}
