import { connect } from 'react-redux';
import NetworkGraph from '../components/NetworkGraph';

const mapStateToProps = (state) => {
  return { graph: state.graph};
};

const mapDispatchToProps = (dispatch) => { // eslint-disable-line no-unused-vars
  return {};
};

export default connect(mapStateToProps, mapDispatchToProps)(NetworkGraph);
