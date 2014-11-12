#ifndef __CL_EXCEPTION_HEADER
#define __CL_EXCEPTION_HEADER

#include "wrapper.hpp"

#include <exception>
#include <string>
#include <experimental/string_view>

namespace cl {
	class Exception : public std::exception {
	private:
		ErrorCode m_error_id;
		std::string m_information;

	public:
		Exception(ErrorCode error_id, std::string information);
		Exception(ErrorCode error_id, std::experimental::string_view information);

		~Exception() noexcept;

		virtual const char * what() const noexcept;

		ErrorCode code() const;
	};

	class PlatformException final     : public Exception { using Exception::Exception; };
	class DeviceException final       : public Exception { using Exception::Exception; };
	class ContextException final      : public Exception { using Exception::Exception; };
	class EventException final        : public Exception { using Exception::Exception; };
	class ProgramException final      : public Exception { using Exception::Exception; };
	class MemoryObjectException final : public Exception { using Exception::Exception; };
	class KernelException final       : public Exception { using Exception::Exception; };
	class CommandQueueException final : public Exception { using Exception::Exception; };
}

#endif
