#include "error_handler.hpp"

namespace cl {
	namespace error {
		cl_bool isError(std::underlying_type<ErrorCode>::type error_code) {
			return error_code != CL_SUCCESS;
		}

		cl_bool isError(ErrorCode error_code) {
			return isError(static_cast<std::underlying_type<ErrorCode>::type>(error_code));
		}

		cl_bool isSuccess(std::underlying_type<ErrorCode>::type error_code) {
			return error_code == CL_SUCCESS;
		}

		cl_bool isSuccess(ErrorCode error_code) {
			return isSuccess(static_cast<std::underlying_type<ErrorCode>::type>(error_code));
		}
	}
}
