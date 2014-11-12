#include "exception.hpp"

#include <utility>

namespace cl {
	Exception::Exception(ErrorCode error_id, std::string information) :
		m_error_id{error_id},
		m_information{std::move(information)}
	{}

	Exception::Exception(ErrorCode error_id, std::experimental::string_view information) :
		m_error_id{error_id},
		m_information{information.begin(), information.end()}
	{}

	Exception::~Exception() noexcept {}

	const char * Exception::what() const noexcept {
		return m_information.c_str();
	}

	ErrorCode Exception::code() const {
		return m_error_id;
	}
}
