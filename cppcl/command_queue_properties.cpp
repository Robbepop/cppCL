#include "command_queue_properties.hpp"

namespace cl {
	CommandQueueProperties::CommandQueueProperties(cl_command_queue_properties mask) :
		m_mask{mask}
	{}

	CommandQueueProperties::CommandQueueProperties() :
		m_mask{0}
	{}

	cl_command_queue_properties CommandQueueProperties::mask() const {
		return m_mask;
	}

	CommandQueueProperties& CommandQueueProperties::outOfOrderExecModeEnabled(cl_bool flag) {
		return modifyMask<CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE>(flag);
	}

	CommandQueueProperties& CommandQueueProperties::profilingEnabled(cl_bool flag) {
		return modifyMask<CL_QUEUE_PROFILING_ENABLE>(flag);
	}

	cl_bool CommandQueueProperties::outOfOrderExecModeEnabled() const {
		return readMask<CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE>();
	}

	cl_bool CommandQueueProperties::profilingEnabled() const {
		return readMask<CL_QUEUE_PROFILING_ENABLE>();
	}
}
