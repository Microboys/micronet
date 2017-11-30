import { handleActions } from 'redux-actions';
import actions from '../actions/dns.js';

export default handleActions({
  [actions.updateDNS]: (state, action) => {
    return { ...state, ...action.payload };
  }
}, {});
