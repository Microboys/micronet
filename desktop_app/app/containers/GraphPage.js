import { connect } from 'react-redux';
import { push } from 'react-router-redux';
import NetworkGraph from '../components/NetworkGraph';

const mapStateToProps = (state) => {
  return state;
};

const mapDispatchToProps = (dispatch) => { // eslint-disable-line no-unused-vars
  return {
  	onHelp: () => {
			dispatch(push('/'));
  	}
  };
};

export default connect(mapStateToProps, mapDispatchToProps)(NetworkGraph);
