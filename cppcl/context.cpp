#include "context.hpp"
#include "context_properties.hpp"
#include "error_handler.hpp"

#include <map>

namespace cl {
	const error::ErrorMap ContextFunctions::error_map = {
		{ErrorCode::invalid_context, "the given context is invalid."}
	};

	Context::Context(cl_context context_id) :
		Object{context_id}
	{}

	/*
	Context::Context(
		const ContextProperties & properties,
		const std::vector<Device> & devices,
		void (*pfn_notify) (
			const char * error_info,
			const void * private_info, size_t cb,
			void * user_data),
		void * user_data
	) : 
		Object{0}
	{
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_platform,     "no valid platform was found or specified."},
			{ErrorCode::invalid_property,     "context property name in properties is not a supported property name, the value specified for a supported property name is not valid, OR the same property name is specified more than once."},
			{ErrorCode::invalid_value,        "user_data and pfn_notify parameters are unset inequally."},
			{ErrorCode::invalid_device,       "devices contained an invalid device."},
			{ErrorCode::invalid_operation,    "two or more different graphics APIs are set to interoperate in this context."},
			{ErrorCode::device_not_available, "a device in devices is not available."}
		};
		auto error = cl_int{CL_INVALID_VALUE};
		auto new_id = clCreateContext(
			properties.get().data(),
			devices.size(), reinterpret_cast<const cl_device_id*>(devices.data()),
			pfn_notify,
			user_data,
			& error
		);
		if (error::handle<ContextException>(error, error_map)) m_id = new_id;
	}

	Context::Context(
		const ContextProperties & properties,
		DeviceType device_type,
		void (*pfn_notify) (
			const char * error_info,
			const void * private_info, size_t cb,
			void * user_data),
		void * user_data
	) :
		Object{0}
	{
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_platform,     "no valid platform was found or specified."},
			{ErrorCode::invalid_property,     "context property name in properties is not a supported property name, the value specified for a supported property name is not valid, OR the same property name is specified more than once."},
			{ErrorCode::invalid_value,        "user_data and pfn_notify parameters are unset inequally."},
			{ErrorCode::invalid_device_type,  "the given device type is invalid."},
			{ErrorCode::device_not_available, "no devices that match the given device type are available."},
			{ErrorCode::device_not_found,     "no devices that match the given device type were found."},
			{ErrorCode::invalid_operation,    "two or more different graphics APIs are set to interoperate in this context."}
		};
		auto error = cl_int{CL_INVALID_VALUE};
		auto new_id = clCreateContextFromType(
			properties.get().data(),
			static_cast<cl_device_type>(device_type),
			pfn_notify,
			user_data,
			& error
		);
		if (error::handle<ContextException>(error, error_map)) m_id = new_id;
	}
	*/

	cl_uint Context::referenceCount() const {
		return getInfo<cl_uint>(CL_CONTEXT_REFERENCE_COUNT);
	}

	cl_uint Context::numDevices() const {
		return getInfo<cl_uint>(CL_CONTEXT_NUM_DEVICES);
	}

	std::vector<Device> Context::devices() const {
		auto device_ids = getInfoVector<cl_device_id>(CL_CONTEXT_DEVICES);
		auto devices = std::vector<Device>();	
		for (auto&& device_id : device_ids) {
			devices.push_back(Device(device_id));
		}
		return devices;
	}

	ContextProperties Context::properties() const {
		return {getInfo<cl_context_properties*>(CL_CONTEXT_PROPERTIES)};
	}
}
