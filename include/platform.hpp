#ifndef __CL_PLATFORM_HEADER
#define __CL_PLATFORM_HEADER

#include "object.hpp"

#include <vector>
#include <string>

namespace cl {
	class Device;
	class PlatformException;

	struct PlatformFunctions final {
		static decltype(auto) release(cl_platform_id) { return; }
		static decltype(auto) retain(cl_platform_id) { return; }
		static decltype(auto) get_info
		(
			cl_platform_id platform,
			cl_platform_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret
		) {
			return clGetPlatformInfo(
				platform, param_name, param_value_size, param_value, param_value_size_ret);
		}
	};

	class Platform final : public Object<cl_platform_id, cl_platform_info, PlatformFunctions, PlatformException> {
	public:
		static std::vector<Platform> getPlatforms();

		Platform(cl_platform_id id);

		std::string profile() const;

		std::string version() const;

		std::string name() const;

		std::string vendor() const;

		std::vector<std::string> getExtensions() const;

		std::vector<Device> getDevices(DeviceType device_type) const;
	};
}

#endif
