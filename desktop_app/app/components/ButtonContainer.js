import React, { Component } from 'react';
import { Button, Row, Col } from 'reactstrap';
import { flashMicrobit } from '../microbit.js'
import PropTypes from 'prop-types';

export default class ButtonContainer extends Component {

  static propTypes = {
    showRenameModal: PropTypes.func.isRequired
  };
  
  constructor(props) {
    super(props);
  };

  render() {
    return (
      <div id='buttonContainer'>
        <Row className='noMarginRow justify-content-centre'>
          <Button id='renameBtn' onClick={this.props.showRenameModal} className='controlButton'>Rename Microbit</Button>
        </Row>
        <Row className='noMarginRow justify-content-centre'>
          <Button id='flashBtn' onClick={flashMicrobit} className='controlButton'>Flash Microbit</Button>
        </Row>
        <Row className='noMarginRow justify-content-centre'>
          <Button id='helpBtn' className='controlButton'>Help</Button>
        </Row>
      </div>
    );
  }
}
