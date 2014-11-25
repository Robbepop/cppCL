#ifndef CPPCL_ERROR_HANDLER_HEADER
#define CPPCL_ERROR_HANDLER_HEADER

#include "wrapper.hpp"
#include "exception.hpp"

#include <iostream>
#include <type_traits>
#include <map>

#include <functional>
#include <experimental/string_view>

namespace cl {
	namespace error {
		using ErrorMap = std::map<ErrorCode, std::experimental::string_view>;

		constexpr cl_bool use_error_maps = CL_TRUE;

		cl_bool isError(std::underlying_type<ErrorCode>::type error_code);
		cl_bool isError(ErrorCode error_code);
		cl_bool isSuccess(std::underlying_type<ErrorCode>::type error_code);
		cl_bool isSuccess(ErrorCode error_code);

		template <typename ExceptionType, typename InfoFunc>
		cl_bool handle(ErrorCode error_code) {
			static const auto global_info_func = [](ErrorCode error_code) -> std::experimental::string_view {
				switch (error_code) {
					case ErrorCode::out_of_resources  : return "there was a failure to allocate resources required by the OpenCL implementation on the device.";
					case ErrorCode::out_of_host_memory: return "there was a failure to allocate resources required by the OpenCL implementation on the host.";
					default                           : return "";
				}
			};
			if (isSuccess(error_code)) return true;
			auto error_info = InfoFunc(error_code);
			if (error_info == "") error_info = global_info_func(error_code);
			throw ExceptionType(error_code, error_info);
		}

		template <typename ExceptionType, typename InfoFunc>
		cl_bool handle(std::underlying_type<ErrorCode>::type error_code) {
			return handle<ExceptionType, InfoFunc>(static_cast<ErrorCode>(error_code));
		}

		template <typename ExceptionType>
		cl_bool handle(
			ErrorCode error_code,
			ErrorMap const& error_map
		) {
			/*
			std::ignore = error_map;
			std::ignore = error_code;
			return true;
			/*///*
			static const auto global_error_map = ErrorMap{
				{ErrorCode::out_of_resources, "there was a failure to allocate resources required by the OpenCL implementation on the device."},
				{ErrorCode::out_of_host_memory, "there was a failure to allocate resources required by the OpenCL implementation on the host."}
			};
			//std::cout << "ErrorCode = " << static_cast<std::underlying_type<ErrorCode>::type>(error_code) << '\n';
			if (isSuccess(error_code)) return true;
			const auto error_info
				= (       error_map.count(error_code)) ?        error_map.at(error_code)
				: (global_error_map.count(error_code)) ? global_error_map.at(error_code) : "";
			throw ExceptionType(error_code, error_info);
			/**/
		}

		template <typename ExceptionType>
		cl_bool handle(
			std::underlying_type<ErrorCode>::type error_code,
			ErrorMap const& error_map
		) {
			return handle<ExceptionType>(static_cast<ErrorCode>(error_code), error_map);
		}
	}
}

#endif
