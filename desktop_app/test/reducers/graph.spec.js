import { expect } from 'chai';
import reducer from '../../app/reducers/graph';

describe('reducers', () => {

  describe('graph', () => {    
    
    it('should handle DRAW_GRAPH', () => {
      const action = { 
        type: 'DRAW_GRAPH',
        payload: {}
      };
      const test = Object.assign({}, action.payload);
      expect(reducer({}, action)).to.deep.equal(test);
    });
    
  });

});
