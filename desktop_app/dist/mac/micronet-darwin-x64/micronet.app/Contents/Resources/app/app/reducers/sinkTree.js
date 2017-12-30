import { handleActions } from 'redux-actions';
import actions from '../actions/sinkTree.js';

export default handleActions({
  [actions.updateSinkTree]: (state, action) => {
    return { ...state, ...action.payload };
  }
}, {});
