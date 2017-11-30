import { expect } from 'chai';
import reducer from '../../app/reducers/graph';

describe('reducers', () => {

  describe('graph', () => {    
    
    it('should handle UPDATE_GRAPH', () => {
      const action = { 
        type: 'UPDATE_GRAPH',
        payload: {}
      };
      const test = Object.assign({}, action.payload);
      expect(reducer({}, action)).to.deep.equal(test);
    });
    
  });

});
