#ifndef CPPCL_PLATFORM_HEADER
#define CPPCL_PLATFORM_HEADER

#include "object.hpp"

#include <vector>
#include <string>

namespace cl {
	class Device;
	class PlatformException;

	struct PlatformInfo final {
	public:
		using cl_type = cl_platform_id;
		using info_type = cl_platform_info;
		using exception_type = PlatformException;

		static decltype(auto) func_release(cl_platform_id) { return; }
		static decltype(auto) func_retain(cl_platform_id) { return; }
		static decltype(auto) func_info
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

	class Platform final : public Object<PlatformInfo> {
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
