import { handleActions } from 'redux-actions';
import actions from '../actions/connection.js';

export default handleActions({
  [actions.updateConnection]: (state, action) => {
    return { ...state, ...action.payload };
  }
}, {});
