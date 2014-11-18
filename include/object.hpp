#ifndef __CL_OBJECT_HEADER
#define __CL_OBJECT_HEADER

#include "wrapper.hpp"
#include "error_handler.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <string>

namespace cl {
	template<typename OpenCLType, typename InfoIdType, typename Functions, typename ExceptionType>
	class Object {
	public:
		OpenCLType id() const {
			return m_id;
		}
	protected:
		Object(OpenCLType id) :
			m_id{id}
		{}

		void release() {
			Functions::release(m_id);
		}

		void retain() {
			Functions::retain(m_id);
		}

		template<typename T>
		T getInfo(InfoIdType info_id) const {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_value, "invalid use of getInfo function; OR invalid information queried."}
			};
			auto error = cl_int{CL_INVALID_VALUE};
			auto info  = T{};
			error = Functions::get_info(m_id, info_id, sizeof(T), std::addressof(info), nullptr);
			error::handle<ExceptionType>(error, error_map);
			return info;
		}

		template<typename T>
		std::vector<T> getInfoVector(InfoIdType info_id) const {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_value, "invalid use of getInfo function; OR invalid information queried."}
			};
			auto error       = cl_int{CL_INVALID_VALUE};
			auto buffer_size = size_t{0};
			error = Functions::get_info(m_id, info_id, 0, nullptr, std::addressof(buffer_size));
			error::handle<ExceptionType>(error, error_map);
			auto count_elems = buffer_size / sizeof(T);
			auto info = std::vector<T>(count_elems);
			error = Functions::get_info(m_id, info_id, buffer_size, info.data(), nullptr);
			error::handle<ExceptionType>(error, error_map);
			return info;
		}

		std::string getInfoString(InfoIdType info_id) const {
			auto info = getInfoVector<char>(info_id);
			return {info.begin(), info.end()};
		}

		OpenCLType m_id;
	};
}

#endif
