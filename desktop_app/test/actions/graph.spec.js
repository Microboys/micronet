import { expect } from 'chai';
import configureMockStore from 'redux-mock-store';
import thunk from 'redux-thunk';
import actions from '../../app/actions/graph';

const mockStore = configureMockStore([ thunk ]);

describe('actions', () => {

  describe('graph', () => {
    
    it('should draw the graph', () => {
      const store = mockStore({});
      const expectedActions = [{
        type: 'DRAW_GRAPH',
        payload: {}
      }];

      store.dispatch(actions.drawGraph({}));

      expect(store.getActions()).deep.equal(expectedActions);
    });

  });
});
