#include "command_queue.hpp"
#include "error_handler.hpp"

namespace cl {
	const error::ErrorMap CommandQueueInfo::error_map = {
		{ErrorCode::invalid_command_queue, "the given command queue is invalid."}
	};

	CommandQueue::CommandQueue(cl_command_queue command_queue_id) :
		Object{command_queue_id}
	{}

	CommandQueue::CommandQueue(Context const& context, Device const& device, CommandQueueProperties const& properties) :
		Object{}
	{
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_context, "the given context is invalid."},
			{ErrorCode::invalid_device, "the given device is invalid."},
			{ErrorCode::invalid_value, "values specified in properties are invalid."},
			{ErrorCode::invalid_queue_properties, "values specified in properties are not supported by the given device."}
		};
		auto error = cl_int{CL_INVALID_VALUE};
		auto new_id = clCreateCommandQueue(
			context.id(), device.id(), properties.mask(), std::addressof(error));
		if (error::handle<CommandQueueException>(error, error_map)) m_id = new_id;
	}

	Context CommandQueue::context() const {
		return {getInfo<cl_context>(CL_QUEUE_CONTEXT)};
	}

	Device CommandQueue::device() const {
		return {getInfo<cl_device_id>(CL_QUEUE_DEVICE)};
	}

	cl_uint CommandQueue::referenceCount() const {
		return getInfo<cl_uint>(CL_QUEUE_REFERENCE_COUNT);
	}

	CommandQueueProperties CommandQueue::properties() const {
		return {getInfo<cl_command_queue_properties>(CL_QUEUE_PROPERTIES)};
	}
}
