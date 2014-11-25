#ifndef CPPCL_EXECUTION_CAPABILITIES_HEADER
#define CPPCL_EXECUTION_CAPABILITIES_HEADER

#include "wrapper.hpp"

namespace cl {
	class ExecutionCapabilities final {
	public:
		ExecutionCapabilities(cl_device_exec_capabilities mask);

		cl_device_exec_capabilities mask() const;

		ExecutionCapabilities & executeKernel(cl_bool flag);
		ExecutionCapabilities & executeNativeKernel(cl_bool flag);

		cl_bool executeKernel() const;
		cl_bool executeNativeKernel() const;

	private:
		cl_device_exec_capabilities m_mask;

		template <cl_device_exec_capabilities Option>
		ExecutionCapabilities & modifyMask(cl_bool flag) {
			if (flag) m_mask |=  Option;
			else      m_mask &= ~Option;
			return * this;
		}

		template <cl_device_exec_capabilities Option>
		cl_bool readMask() const {
			return !(m_mask & Option);
		}
	};
}
#endif
