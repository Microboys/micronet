import { handleActions } from 'redux-actions';
import actions from '../actions/graph.js';

export default handleActions({
  [actions.drawGraph]: (state, action) => {
    return { ...state, ...action.payload };
  }
}, {});
