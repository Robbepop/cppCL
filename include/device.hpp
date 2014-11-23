#ifndef __CL_DEVICE_HEADER
#define __CL_DEVICE_HEADER

#include "object.hpp"
#include "error_handler.hpp"

#include <map>

namespace cl {
	class Platform;
	class FPConfig;
	class ExecutionCapabilities;
	class CommandQueueProperties;
	class DeviceException;

	struct DeviceInfo final {
	private:
		static const error::ErrorMap error_map;

	public:
		using cl_type = cl_device_id;
		using info_type = cl_device_info;
		using exception_type = DeviceException;

		static decltype(auto) func_release(cl_device_id id) {
#if defined(CPPCL_CL_VERSION_1_2_ENABLED)
			error::handle<DeviceException>(clReleaseDevice(id), error_map);
#endif
		}

		static decltype(auto) func_retain(cl_device_id id) {
#if defined(CPPCL_CL_VERSION_1_2_ENABLED)
			error::handle<DeviceException>(clRetainDevice(id), error_map);
#endif
		}

		static decltype(auto) func_info
		(
			cl_device_id device,
			cl_device_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret
		) {
			return clGetDeviceInfo(
				device, param_name, param_value_size, param_value, param_value_size_ret);
		}
	};

	class Device final : public Object<DeviceInfo> {
	public:
		Device(cl_device_id id);

#if defined(CPPCL_CL_VERSION_1_2_ENABLED)
	private:
		std::vector<Device> partition(std::vector<cl_device_partition_property> const& properties, error::ErrorMap error_map);
	public:
		std::vector<Device> partitionEqually(cl_uint num);
		std::vector<Device> partitionByCounts(std::vector<cl_uint> const& counts);
		std::vector<Device> partitionByAffinityDomain(DeviceAffinityDomain domain);
#endif

		cl_uint addressBits() const;

		cl_bool available() const;

		cl_bool compilerAvailable() const;

		FPConfig fpConfig(FloatingPointType type) const;

		cl_bool littleEndian() const;

		cl_bool errorCorrectionSupport() const;

		ExecutionCapabilities executionCapabilities() const;

		std::vector<std::string> getExtensions() const;

		cl_ulong globalMemoryCacheSize() const;

		DeviceMemoryCacheType globalMemoryCacheType() const;

		cl_uint globalMemoryCachelineSize() const;

		cl_ulong globalMemorySize() const;

		cl_bool hostUnifiedMemory() const;

		cl_bool imageSupport() const;

		size_t maxHeightImage2D() const;

		size_t maxWidthImage2D() const;

		size_t maxHeightImage3D() const;

		size_t maxWidthImage3D() const;

		size_t maxDepthImage3D() const;

		cl_ulong localMemorySize() const;

		DeviceLocalMemoryType localMemoryType() const;

		cl_uint maxClockFrequency() const;

		cl_uint maxComputeUnits() const;

		cl_uint maxConstantArguments() const;

		cl_ulong maxConstantBufferSize() const;

		cl_ulong maxMemoryAllocationSize() const;

		size_t maxParameterSize() const;

		cl_uint maxReadImageArguments() const;

		cl_uint maxSamplers() const;

		size_t maxWorkGroupSize() const;

		cl_uint maxWorkItemDimensions() const;

		std::vector<size_t> maxWorkItemSizes() const;

		cl_uint maxWriteImageArguments() const;

		cl_uint memoryBaseAddressAlign() const;

		cl_uint minDataTypeAlignSize() const;

		std::string name() const;

		cl_uint nativeVectorWidth(ScalarType type) const;

		std::string versionOpenCLC() const;

		Platform platform() const;

		cl_uint preferredVectorWidth(ScalarType type) const;

		std::string profile() const;

		size_t profilingTimerResolution() const;

		CommandQueueProperties queueProperties() const;

		DeviceType type() const;

		std::string vendor() const;

		cl_uint vendorID() const;

		std::string version() const;

		std::string driverVersion() const;
	};
}

#endif
