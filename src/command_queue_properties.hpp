#ifndef __CL_COMMAND_QUEUE_PROPERTIES_HEADER
#define __CL_COMMAND_QUEUE_PROPERTIES_HEADER

#include "wrapper.hpp"

namespace cl {
	class CommandQueueProperties final {
	public:
		CommandQueueProperties(cl_command_queue_properties mask);
		CommandQueueProperties();

		cl_bool outOfOrderExecModeEnabled() const;
		cl_bool profilingEnabled() const;

		CommandQueueProperties& outOfOrderExecModeEnabled(cl_bool flag);
		CommandQueueProperties& profilingEnabled(cl_bool flag);

		cl_command_queue_properties mask() const;

	private:
		cl_command_queue_properties m_mask;

		template <cl_command_queue_properties Option>
		CommandQueueProperties & modifyMask(cl_bool flag) {
			if (flag) m_mask |=  Option;
			else      m_mask &= ~Option;
			return * this;
		}

		template <cl_command_queue_properties Option>
		cl_bool readMask() const {
			return !(m_mask & Option);
		}
	};
}

#endif
