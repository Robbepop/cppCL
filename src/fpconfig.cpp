#include "fpconfig.hpp"

namespace cl {
	FPConfig::FPConfig(cl_device_fp_config mask) :
		m_mask{mask}
	{}

	FPConfig::FPConfig() :
		m_mask{0}
	{}

	cl_device_fp_config FPConfig::mask() const {
		return m_mask;
	}

	FPConfig & FPConfig::denorm(cl_bool flag) {
		return modifyMask<CL_FP_DENORM>(flag);
	}

	FPConfig & FPConfig::infAndNan(cl_bool flag) {
		return modifyMask<CL_FP_INF_NAN>(flag);
	}

	FPConfig & FPConfig::roundToNearest(cl_bool flag) {
		return modifyMask<CL_FP_ROUND_TO_NEAREST>(flag);
	}

	FPConfig & FPConfig::roundToZero(cl_bool flag) {
		return modifyMask<CL_FP_ROUND_TO_ZERO>(flag);
	}

	FPConfig & FPConfig::roundToInf(cl_bool flag) {
		return modifyMask<CL_FP_ROUND_TO_INF>(flag);
	}

	FPConfig & FPConfig::fma(cl_bool flag) {
		return modifyMask<CL_FP_FMA>(flag);
	}

	FPConfig & FPConfig::softFloat(cl_bool flag) {
		return modifyMask<CL_FP_SOFT_FLOAT>(flag);
	}

	cl_bool FPConfig::denorm() const {
		return readMask<CL_FP_DENORM>();
	}

	cl_bool FPConfig::infAndNan() const {
		return readMask<CL_FP_INF_NAN>();
	}

	cl_bool FPConfig::roundToNearest() const {
		return readMask<CL_FP_ROUND_TO_NEAREST>();
	}

	cl_bool FPConfig::roundToZero() const {
		return readMask<CL_FP_ROUND_TO_ZERO>();
	}

	cl_bool FPConfig::roundToInf() const {
		return readMask<CL_FP_ROUND_TO_INF>();
	}

	cl_bool FPConfig::fma() const {
		return readMask<CL_FP_FMA>();
	}

	cl_bool FPConfig::softFloat() const {
		return readMask<CL_FP_SOFT_FLOAT>();
	}
}
