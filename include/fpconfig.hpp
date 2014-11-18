#ifndef __CL_FP_CONFIG_HEADER
#define __CL_FP_CONFIG_HEADER

#include "wrapper.hpp"

namespace cl {
	class FPConfig final {
	public:
		FPConfig(cl_device_fp_config mask);
		FPConfig();

		cl_device_fp_config mask() const;

		FPConfig & denorm(cl_bool flag);
		FPConfig & infAndNan(cl_bool flag);
		FPConfig & roundToNearest(cl_bool flag);
		FPConfig & roundToZero(cl_bool flag);
		FPConfig & roundToInf(cl_bool flag);
		FPConfig & fma(cl_bool flag);
		FPConfig & softFloat(cl_bool flag);

		cl_bool denorm() const;
		cl_bool infAndNan() const;
		cl_bool roundToNearest() const;
		cl_bool roundToZero() const;
		cl_bool roundToInf() const;
		cl_bool fma() const;
		cl_bool softFloat() const;

	private:
		template <cl_device_fp_config Option>
		FPConfig & modifyMask(cl_bool flag) {
			if (flag) m_mask |=  Option;
			else      m_mask &= ~Option;
			return * this;
		}

		template <cl_device_fp_config Option>
		cl_bool readMask() const {
			return !(m_mask & Option);
		}

		cl_device_fp_config m_mask;
	};
}

#endif
