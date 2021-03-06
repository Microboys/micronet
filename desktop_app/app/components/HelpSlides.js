import React, { Component } from 'react';
import { Container, UncontrolledCarousel, Row, Col, Button } from 'reactstrap';
import PropTypes from 'prop-types';

export default class HelpSlides extends Component {
  
  static propTypes = {
    onSkip: PropTypes.func.isRequired
  };

  constructor(props) {
    super(props);

    this.state = {
      items: [
        {
          src: './assets/slide_1.svg',
          caption: ''
        },
        {
          src: './assets/slide_2.svg',
          caption: ''
        },
        {
          src: './assets/slide_3.svg',
          caption: ''
        },
        {
          src: './assets/slide_4.svg',
          caption: ''
        },
        {
          src: './assets/slide_5.svg',
          caption: ''
        },
        {
          src: './assets/slide_6.svg',
          caption: ''
        }
      ]
    }
  }

  render() {
    return (
      <div className='fullsize'>
        <Row id='carouselRow' className='justify-content-center noMarginRow'>
          <Col className='align-self-center'>
            <UncontrolledCarousel items={this.state.items} />
          </Col>
        </Row>
        <Row id='skipRow' className='justify-content-center noMarginRow'>
          <Button autoFocus={true} id='skipButton' onClick={this.props.onSkip}>I'm Ready</Button>
        </Row>
      </div>
    );
  }
}
