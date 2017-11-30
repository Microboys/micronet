import { connect } from 'react-redux';
import { push } from 'react-router-redux';
import HelpSlides from '../components/HelpSlides';

const mapStateToProps = (state) => {
  return state;
};

const mapDispatchToProps = (dispatch) => { // eslint-disable-line no-unused-vars
  return {
  	onSkip: () => {
			dispatch(push('/graph'));
  	}
  };
};

export default connect(mapStateToProps, mapDispatchToProps)(HelpSlides);
