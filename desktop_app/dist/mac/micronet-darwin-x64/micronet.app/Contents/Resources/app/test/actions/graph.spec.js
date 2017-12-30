import { expect } from 'chai';
import configureMockStore from 'redux-mock-store';
import thunk from 'redux-thunk';
import actions from '../../app/actions/graph';

const mockStore = configureMockStore([ thunk ]);

describe('actions', () => {

  describe('graph', () => {
    
    it('should update the graph', () => {
      const store = mockStore({});
      const expectedActions = [{
        type: 'UPDATE_GRAPH',
        payload: {}
      }];

      store.dispatch(actions.updateGraph({}));

      expect(store.getActions()).deep.equal(expectedActions);
    });

  });
});
