#include "execution_capabilities.hpp"

namespace cl {
	ExecutionCapabilities::ExecutionCapabilities(cl_device_exec_capabilities mask) :
		m_mask{mask}
	{}

	cl_device_exec_capabilities ExecutionCapabilities::mask() const {
		return m_mask;
	}

	ExecutionCapabilities & ExecutionCapabilities::executeKernel(cl_bool flag) {
		return modifyMask<CL_EXEC_KERNEL>(flag);
	}

	ExecutionCapabilities & ExecutionCapabilities::executeNativeKernel(cl_bool flag) {
		return modifyMask<CL_EXEC_NATIVE_KERNEL>(flag);
	}

	cl_bool ExecutionCapabilities::executeKernel() const {
		return readMask<CL_EXEC_KERNEL>();
	}

	cl_bool ExecutionCapabilities::executeNativeKernel() const {
		return readMask<CL_EXEC_NATIVE_KERNEL>();
	}
}
