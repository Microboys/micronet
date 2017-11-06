import React, { Component } from 'react';
import { Input, InputGroup, InputGroupAddon, InputGroupButton, Button, Form } from 'reactstrap';
import { sendMsg } from './../microbit.js';

export default class MsgTable extends Component {
  
  constructor(props) {
    super(props);
    this.state = {
      msg: '',
      to: ''
    };
    this.handleChangeTo = this.handleChangeTo.bind(this);
    this.handleChangeMsg = this.handleChangeMsg.bind(this);
    this.handleSubmit = this.handleSubmit.bind(this);
  }

  handleChangeTo(event) {
    this.setState({to: event.target.value});
  }

  handleChangeMsg(event) {
    this.setState({msg: event.target.value});
  }

  handleSubmit(event) {
    sendMsg(this.state.to, this.state.msg);
    event.preventDefault();
  }

  render() {
    return (
      <Form onSubmit={this.handleSubmit}>
        <InputGroup>
          <InputGroupAddon>To:</InputGroupAddon>
          <Input onChange={this.handleChangeTo} value={this.state.to}/>
        </InputGroup>
        <InputGroup>
          <InputGroupAddon>Message:</InputGroupAddon>
          <Input onChange={this.handleChangeMsg} value={this.state.msg}/>
          <InputGroupButton><Button>Send</Button></InputGroupButton>
        </InputGroup>
      </Form>
    );
  }
}
