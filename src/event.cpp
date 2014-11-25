#include "event.hpp"
#include "command_queue.hpp"

#include <type_traits>

namespace cl {
	const error::ErrorMap EventInfo::error_map = {
		{ErrorCode::invalid_event, "the given event is invalid."}
	};

	Event::Event(cl_event event_id) :
		Object{event_id}
	{}

	Event::Event(Context const& context) :
		Object{}
	{
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_context, "the given context is invalid."}
		};
		auto error = cl_int{CL_INVALID_VALUE};
		auto new_id = clCreateUserEvent(context.id(), &error);
		if (error::handle<EventException>(error, error_map)) m_id = new_id;
	}

	void Event::setStatus(CommandExecutionStatus status) {
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_event, "the given user event was invalid."},
			{ErrorCode::invalid_value, "the given execution status is invalid."},
			{ErrorCode::invalid_operation, "the execution status has already been set for this event."}
		};
		error::handle<EventException>(
			clSetUserEventStatus(
				m_id, static_cast<std::underlying_type<CommandExecutionStatus>::type>(status)
			),
			error_map
		);
	}

	void Event::finish() {
		this->setStatus(CommandExecutionStatus::complete);
	}

	void Event::fail() {
		this->setStatus(CommandExecutionStatus::error);
	}

	void Event::wait() const {
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_event, "the given event is invalid."},
			{ErrorCode::execute_status_error_for_events_in_wait_list, "the execution status of the given event is invalid."}
		};
		error::handle<EventException>(clWaitForEvents(1, std::addressof(m_id)), error_map);
	}

	void Event::callback(
		CommandExecutionCallbackType type,
		void (*pfn_notify) (cl_event event, cl_int status, void * user_data),
		void * user_data
	) {
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_event, "the given event is invalid."},
			{ErrorCode::invalid_value, "the given callback is null; OR the given callback type is invalid."}
		};
		error::handle<EventException>(
			clSetEventCallback(
				m_id,
				static_cast<std::underlying_type<CommandExecutionCallbackType>::type>(type),
				pfn_notify,
				user_data
			),
			error_map
		);
	}

	std::unique_ptr<CommandQueue> Event::commandQueue() const {
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_event, "can not get command queue of a user event."}
		};
		auto queue = getInfo<cl_command_queue>(CL_EVENT_COMMAND_QUEUE);
		auto error = (queue == NULL) ? ErrorCode::invalid_event : ErrorCode::success;
		error::handle<EventException>(error, error_map);
		//return {queue}; //invalid due to cyclic dependency with command_queue.hpp
		return std::make_unique<CommandQueue>(queue);
	}

	Context Event::context() const {
		return {getInfo<cl_context>(CL_EVENT_CONTEXT)};
	}

	CommandType Event::commandType() const {
		return static_cast<CommandType>(
			getInfo<std::underlying_type<CommandType>::type>(CL_EVENT_COMMAND_TYPE));
	}

	CommandExecutionStatus Event::status() const {
		auto state = getInfo<std::underlying_type<CommandExecutionStatus>::type>(
			CL_EVENT_COMMAND_EXECUTION_STATUS);
		return (state < 0) ? CommandExecutionStatus::error
						   : static_cast<CommandExecutionStatus>(state);
	}

	cl_uint Event::referenceCount() const {
		return getInfo<cl_uint>(CL_EVENT_REFERENCE_COUNT);
	}

	void Event::waitForEvents(std::vector<Event> const& events) {
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_context, "not all given events do belong to the same context."},
			{ErrorCode::invalid_event, "there is one or more invalid given event."},
			{ErrorCode::execute_status_error_for_events_in_wait_list, "the execution status for one or more given events is invalid."}
		};
		auto error = clWaitForEvents(
			events.size(),
			reinterpret_cast<const cl_event*>(events.data()));
		error::handle<EventException>(error, error_map);
	}
}
