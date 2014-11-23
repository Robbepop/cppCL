#ifndef __CL_OBJECT_HEADER
#define __CL_OBJECT_HEADER

#include "wrapper.hpp"
#include "error_handler.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <string>

/*
 * ObjectInfo struct contains several useful information
 * about the wrapping class and its wrapped object,
 * types, states and functions.
 * 
 * Types:
 *     cl_type : the OpenCL type being wrapped
 *     exception_type : the exception type for the object being wrapped
 *     info_type : the OpenCL type to query information of the object being wrapped
 * 
 * Functions:
 *     func_retain : the OpenCL retain function for the object being wrapped
 *     func_release : the OpenCL release function for the object being wrapped
 *     func_info : the OpenCL information function for the object being wrapped
 */

namespace cl {
	template<typename ObjectInfo>
	class Object {
	public:
		typename ObjectInfo::cl_type id() const {
			return m_id;
		}
	protected:
		Object<ObjectInfo>(typename ObjectInfo::cl_type id) :
			m_id{id}
		{}

		Object<ObjectInfo>(Object<ObjectInfo> const& other) :
			m_id{other.id()}
		{
			ObjectInfo::func_retain(m_id);
		}

		Object<ObjectInfo>(Object<ObjectInfo> && other) :
			m_id{other.id()}
		{
			ObjectInfo::func_retain(m_id);
		}

		~Object<ObjectInfo>() {
			ObjectInfo::func_release(m_id);
		}

		Object<ObjectInfo> & operator=(Object<ObjectInfo> const& other) {
			m_id = other.id();
			ObjectInfo::func_retain(m_id);
		}

		Object<ObjectInfo> & operator=(Object<ObjectInfo> && other) {
			m_id = other.id();
			ObjectInfo::func_retain(m_id);
		}

		template<typename T>
		T getInfo(typename ObjectInfo::info_type info_id) const {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_value, "invalid use of getInfo function; OR invalid information queried."}
			};
			auto error = cl_int{CL_INVALID_VALUE};
			auto info  = T{};
			error = ObjectInfo::func_info(m_id, info_id, sizeof(T), std::addressof(info), nullptr);
			error::handle<typename ObjectInfo::exception_type>(error, error_map);
			return info;
		}

		template<typename T>
		std::vector<T> getInfoVector(typename ObjectInfo::info_type info_id) const {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_value, "invalid use of getInfo function; OR invalid information queried."}
			};
			auto error       = cl_int{CL_INVALID_VALUE};
			auto buffer_size = size_t{0};
			error = ObjectInfo::func_info(m_id, info_id, 0, nullptr, std::addressof(buffer_size));
			error::handle<typename ObjectInfo::exception_type>(error, error_map);
			auto count_elems = buffer_size / sizeof(T);
			auto info = std::vector<T>(count_elems);
			error = ObjectInfo::func_info(m_id, info_id, buffer_size, info.data(), nullptr);
			error::handle<typename ObjectInfo::exception_type>(error, error_map);
			return info;
		}

		std::string getInfoString(typename ObjectInfo::info_type info_id) const {
			auto info = getInfoVector<char>(info_id);
			return {info.begin(), info.end()};
		}

		typename ObjectInfo::cl_type m_id;
	};
}

#endif
