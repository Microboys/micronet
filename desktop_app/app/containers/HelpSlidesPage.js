import { connect } from 'react-redux';
import HelpSlides from '../components/HelpSlides';

const mapStateToProps = (state) => {
  return state;
};

const mapDispatchToProps = (dispatch) => { // eslint-disable-line no-unused-vars
  return {};
};

export default connect(mapStateToProps, mapDispatchToProps)(HelpSlides);
