import { createAction } from 'redux-actions';

export default {
  addPacket: createAction('ADD_PACKET'),
  removeOldestPacket: createAction('REMOVE_OLDEST_PACKET'),
  removeAllPackets: createAction('REMOVE_ALL_PACKETS')
};
