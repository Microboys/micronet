import { handleActions } from 'redux-actions';
import actions from '../actions/packet.js';

export default handleActions({
  [actions.addPacket]: (state, action) => {
    return { ...state, "recieved" : state.received.push(action.payload) };
  },
  [actions.removeOldestPacket]: (state, action) => {
    console.log("Reducing Reducing Reducing");
    return { ...state, "recieved" : state.received.slice(0, state.received.length) };
  },
  [actions.removeAllPackets]: (state, action) => {
    return { ...state, "received" : [] };
  }
}, {});
