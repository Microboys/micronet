import { handleActions } from 'redux-actions';
import actions from '../actions/packet.js';

export default handleActions({
  [actions.addPacket]: (state, action) => {
    let received = state.received;
    if (received.length == state.capacity) {
      received = received.slice(1, received.length);
    }
    received.push(action.payload);
    return { ...state, "received" : received };
  },
  [actions.removeAllPackets]: (state, action) => {
    return { ...state, "received" : [] };
  }
}, {});
