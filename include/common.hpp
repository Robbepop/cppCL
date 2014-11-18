#ifndef __CL_COMMON_HEADER
#define __CL_COMMON_HEADER

#include "wrapper.hpp"

#include <sstream>
#include <vector>

#include <iostream>

#include "error_handler.hpp"

namespace cl {
	namespace common {
		std::vector<std::string> split(
			const std::string& s,
			const std::string& delim,
			const bool keep_empty = true);

		template<typename ReturnType, typename ObjectIdType, typename InfoIdType, typename InfoFunc>
		ReturnType getInfo(ObjectIdType m_id, InfoIdType info_id, InfoFunc get_info) {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_value, "invalid use of getInfo function; OR invalid information queried."}
			};
			auto error = cl_int{CL_INVALID_VALUE};
			auto info  = ReturnType{};
			error = get_info(m_id, info_id, sizeof(ReturnType), std::addressof(info), nullptr);
			error::handle<Exception>(error, error_map);
		}

		template<typename ReturnType, typename ObjectIdType, typename InfoIdType, typename InfoFunc>
		std::vector<ReturnType> getInfoVector(
			ObjectIdType m_id,
			InfoIdType info_id,
			InfoFunc get_info,
			bool count_element_wise = false
		) {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_value, "invalid use of getInfo function; OR invalid information queried."}
			};
			auto error       = cl_int{CL_INVALID_VALUE};
			auto buffer_size = cl_uint{0};
			error = get_info(m_id, info_id, 0, nullptr, std::addressof(buffer_size));
			error::handle<Exception>(error, error_map);
			auto count_elems = (count_element_wise) ? buffer_size : buffer_size / sizeof(ReturnType);
			auto info = std::vector<ReturnType>(count_elems);
			error = get_info(m_id, info_id, buffer_size, info.data(), nullptr);
			error::handle<Exception>(error, error_map);
			return info;
		}

		template<typename ObjectIdType, typename InfoIdType, typename InfoFunc>
		std::string getInfoString(
			ObjectIdType m_id,
			InfoIdType info_id,
			InfoFunc get_info,
			bool count_element_wise = false
		) {
			auto info = getInfoVector<char, ObjectIdType, InfoIdType, InfoFunc>(
				m_id, info_id, get_info, count_element_wise);
			return {info.begin(), info.end()};
		}
	}
}

#endif
