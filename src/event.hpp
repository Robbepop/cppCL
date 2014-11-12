#ifndef __CL_EVENT_HEADER
#define __CL_EVENT_HEADER

#include "object.hpp"
#include "context.hpp"
#include "command_queue.hpp"

namespace cl {
	class CommandQueue;
	class Event;

	enum class UserEventStatus : cl_int {
		complete = CL_COMPLETE,
		terminate = -1
	};

	enum class CommandExecutionStatus : cl_int {
		queued = CL_QUEUED,
		submitted = CL_SUBMITTED,
		running = CL_RUNNING,
		complete = CL_COMPLETE,
		error = -1
	};

	enum class CommandExecutionCallbackType : cl_int {
		submitted = CL_SUBMITTED,
		running = CL_RUNNING,
		complete = CL_COMPLETE
	};

	struct EventFunctions final {
	private:
		static const error::ErrorMap error_map;

	public:
		static decltype(auto) release(cl_event id) {
			error::handle<EventException>(clReleaseEvent(id), error_map);
		}

		static decltype(auto) retain(cl_event id) {
			error::handle<EventException>(clRetainEvent(id), error_map);
		}

		static decltype(auto) get_info
		(
			cl_event event,
			cl_event_info param_name,
			size_t param_value_size,
			void * param_value,
			size_t * param_value_size_ret
		) {
			return clGetEventInfo(
				event, param_name, param_value_size, param_value, param_value_size_ret);
		}
	};

	class Event final : public Object<cl_event, cl_event_info, EventFunctions, EventException> {
	public:
		Event(cl_event event_id);
		Event(Context const& context);
		void status(UserEventStatus s);
		void callback(
			CommandExecutionCallbackType status,
			void (*pfn_notify) (cl_event event, cl_int status, void * user_data),
			void * user_data
		);
		void wait() const;

		CommandQueue commandQueue() const;
		Context context() const;
		CommandType commandType() const;
		CommandExecutionStatus status() const;
		cl_uint referenceCount() const;

		static void waitForEvents(std::vector<Event> const& events);
	};
}

#endif
