#ifndef __CL_CONTEXT_HEADER
#define __CL_CONTEXT_HEADER

#include "object.hpp"
#include "device.hpp"
#include "context_properties.hpp"
#include "error_handler.hpp"

#include <cstdint>
#include <experimental/string_view>

namespace cl {
	class ContextProperties;
	class ContextException;

	struct ContextFunctions final {
	private:
		static const error::ErrorMap error_map;

	public:
		static decltype(auto) release(cl_context id) {
			error::handle<ContextException>(clReleaseContext(id), error_map);
		}

		static decltype(auto) retain(cl_context id) {
			error::handle<ContextException>(clRetainContext(id), error_map);
		}

		static decltype(auto) get_info
		(
			cl_context context,
			cl_context_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret
		) {
			return clGetContextInfo(
				context, param_name, param_value_size, param_value, param_value_size_ret);
		}
	};

	class Context final : public Object<cl_context, cl_context_info, ContextFunctions, ContextException> {
	public:
		Context(cl_context context_id);

		Context(
			const ContextProperties & properties,
			const std::vector<Device> & devices,
			void (*pfn_notify) (
				const char * error_info,
				const void * private_info, size_t cb,
				void * user_data) = nullptr,
			void * user_data = nullptr
		);

		template <typename Function, typename T>
		Context(
			ContextProperties const& properties,
			std::vector<Device> const& devices,
			Function callback,
			T&& user_data
		) :
			Object{0}
		{
			/*
			static const auto error_lambda = [](ErrorCode error_code) -> std::experimental::string_view {
				switch (error_code) {
					case ErrorCode::invalid_platform    : return "no valid platform was found or specified.";
					case ErrorCode::invalid_property    : return "context property name in properties is not a supported property name, the value specified for a supported property name is not valid, OR the same property name is specified more than once.";
					case ErrorCode::invalid_value       : return "user_data and pfn_notify parameters are unset inequally.";
					case ErrorCode::invalid_device      : return "devices contained an invalid device.";
					case ErrorCode::invalid_operation   : return "two or more different graphics APIs are set to interoperate in this context.";
					case ErrorCode::device_not_available: return "a device in devices is not available.";
					default                             : return "";
				}
			};
			*/
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_platform,     "no valid platform was found or specified."},
				{ErrorCode::invalid_property,     "context property name in properties is not a supported property name, the value specified for a supported property name is not valid, OR the same property name is specified more than once."},
				{ErrorCode::invalid_value,        "user_data and pfn_notify parameters are unset inequally."},
				{ErrorCode::invalid_device,       "devices contained an invalid device."},
				{ErrorCode::invalid_operation,    "two or more different graphics APIs are set to interoperate in this context."},
				{ErrorCode::device_not_available, "a device in devices is not available."}
			};
			struct callback_data {
				Function callback;
				T&& user_data;
			};
			auto error = cl_int{CL_INVALID_VALUE};
			auto callback_wrapper = callback_data{callback, std::forward<T>(user_data)};
			auto new_id = clCreateContext(
				properties.get().data(),
				devices.size(),
				reinterpret_cast<const cl_device_id*>(devices.data()),
				[](
					const char * error_info,
					const void * private_info,
					size_t cb,
					void * user_data
				) {
					auto temp = reinterpret_cast<callback_data*>(user_data);
					temp->callback(
						std::string{error_info},
						std::vector<uint8_t>(
							reinterpret_cast<const uint8_t*>(private_info),
							reinterpret_cast<const uint8_t*>(private_info) + cb),
						temp->user_data
					);
				},
				& callback_wrapper,
				& error
			);
			std::cout << "context.hpp: Context::Context: ErrorCode = " << error << "\n";
			if (error::handle<ContextException>(error, error_map)) m_id = new_id;
			//if (error::handle<ContextException, error_lambda>(error)) m_id = new_id;
		}

		Context(
			const ContextProperties & properties,
			DeviceType type,
			void (*pfn_notify) (
				const char * error_info,
				const void * private_info, size_t cb,
				void * user_data) = nullptr,
			void * user_data = nullptr
		);

		cl_uint referenceCount() const;
		cl_uint numDevices() const;
		std::vector<Device> devices() const;
		ContextProperties properties() const;
	};
}

#endif
