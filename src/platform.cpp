#include <iostream>

#include "platform.hpp"
#include "common.hpp"
#include "device.hpp"

namespace cl {

	std::vector<Platform> Platform::getPlatforms() {
		auto error = cl_int{CL_INVALID_VALUE};
		auto num_platforms = cl_uint{0};
		error = clGetPlatformIDs(0, nullptr, std::addressof(num_platforms));
		if (error != CL_SUCCESS) return {};
		auto platform_ids = std::vector<cl_platform_id>{num_platforms};
		error = clGetPlatformIDs(num_platforms, platform_ids.data(), nullptr);
		if (error != CL_SUCCESS) return {};
		auto platforms = std::vector<Platform>{};
		for (auto&& platform_id : platform_ids) {
			platforms.emplace_back(platform_id);
		}
		return platforms;
	}

	Platform::Platform(cl_platform_id id) :
		Object{id}
	{}

	std::string Platform::profile() const {
		return getInfoString(CL_PLATFORM_PROFILE);
	}

	std::string Platform::version() const {
		return getInfoString(CL_PLATFORM_VERSION);
	}

	std::string Platform::name() const {
		return getInfoString(CL_PLATFORM_NAME);
	}

	std::string Platform::vendor() const {
		return getInfoString(CL_PLATFORM_VENDOR);
	}

	std::vector<std::string> Platform::getExtensions() const {
		return common::split(getInfoString(CL_PLATFORM_EXTENSIONS), " ");
	}

	std::vector<Device> Platform::getDevices(DeviceType device_type) const {
		auto device_ids = common::getInfoVector<cl_device_id>
			(m_id, static_cast<cl_device_type>(device_type), clGetDeviceIDs, true);
		auto devices = std::vector<Device>();	
		for (auto&& device_id : device_ids) {
			devices.emplace_back(device_id);
		}
		return devices;
	}
}
