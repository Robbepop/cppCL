#include "device.hpp"
#include "platform.hpp"
#include "fpconfig.hpp"
#include "execution_capabilities.hpp"
#include "command_queue_properties.hpp"
#include "common.hpp"

#include <iostream>
#include <vector>

namespace cl {
	const error::ErrorMap DeviceFunctions::error_map = {
		{ErrorCode::invalid_device, "device is not a valid sub-device."}
	};

	Device::Device(cl_device_id id) :
		Object(id)
	{}

	std::vector<Device> Device::partition(
		std::vector<cl_device_partition_property> const& properties,
		error::ErrorMap error_map
	) {
		auto error = cl_int{CL_INVALID_VALUE};
		auto count_elems = cl_uint{0};
		error = clCreateSubDevices(m_id, properties.data(), 0, nullptr, std::addressof(count_elems));
		error::handle<DeviceException>(error, error_map);
		auto subdevices = std::vector<Device>(count_elems, m_id);
		error = clCreateSubDevices(
			m_id, properties.data(), count_elems, reinterpret_cast<cl_device_id*>(subdevices.data()), nullptr
		);
		error::handle<DeviceException>(error, error_map);
		return std::move(subdevices);
	}

	std::vector<Device> Device::partitionEqually(cl_uint num) {
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_device, "this device is invalid."},
			{ErrorCode::invalid_value, "values specified are not supported by the device."},
			{ErrorCode::device_partition_failed, "the device could not be further partitioned."}
		};
		const auto properties = std::vector<cl_device_partition_property> {
			{CL_DEVICE_PARTITION_EQUALLY, num, 0}
		};
		return partition(properties, error_map);
	}

	std::vector<Device> Device::partitionByCounts(std::vector<cl_uint> const& counts) {
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_device, "this device is invalid."},
			{ErrorCode::invalid_value, "values specified are not supported by the device."},
			{ErrorCode::device_partition_failed, "the device could not be further partitioned."},
			{ErrorCode::invalid_device_partition_count, "the number of specified subdevices exceeds maximum subdevices count; OR the number of specified compute units exceeds maximum compute units number."}
		};
		auto properties = std::vector<cl_device_partition_property>{CL_DEVICE_PARTITION_BY_COUNTS};
		properties.insert(properties.end(), counts.begin(), counts.end());
		properties.push_back(CL_DEVICE_PARTITION_BY_COUNTS_LIST_END);
		return partition(properties, error_map);
	}

	std::vector<Device> Device::partitionByAffinityDomain(DeviceAffinityDomain domain) {
		static const auto error_map = error::ErrorMap{
			{ErrorCode::invalid_device, "this device is invalid."},
			{ErrorCode::invalid_value, "values specified are not supported by the device."},
			{ErrorCode::device_partition_failed, "the device could not be further partitioned."}
		};
		const auto properties = std::vector<cl_device_partition_property> {
			{
				CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN,
			 	static_cast<cl_device_partition_property>(domain),
			 	0
			}
		};
		return partition(properties, error_map);
	}

	cl_uint Device::addressBits() const {
		return getInfo<cl_uint>(CL_DEVICE_ADDRESS_BITS);
	}

	cl_bool Device::available() const {
		return getInfo<cl_bool>(CL_DEVICE_AVAILABLE);
	}

	cl_bool Device::compilerAvailable() const {
		return getInfo<cl_bool>(CL_DEVICE_COMPILER_AVAILABLE);
	}

	FPConfig Device::fpConfig(FloatingPointType type) const {
		return {getInfo<cl_device_fp_config>(static_cast<cl_device_info>(type))};
	}

	cl_bool Device::littleEndian() const {
		return getInfo<cl_bool>(CL_DEVICE_ENDIAN_LITTLE);
	}

	cl_bool Device::errorCorrectionSupport() const {
		return getInfo<cl_bool>(CL_DEVICE_ERROR_CORRECTION_SUPPORT);
	}

	ExecutionCapabilities Device::executionCapabilities() const {
		return {getInfo<cl_device_exec_capabilities>(CL_DEVICE_EXECUTION_CAPABILITIES)};
	}

	std::vector<std::string> Device::getExtensions() const {
		return common::split(getInfoString(CL_DEVICE_EXTENSIONS), " ");
	}

	cl_ulong Device::globalMemoryCacheSize() const {
		return getInfo<cl_ulong>(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
	}

	DeviceMemoryCacheType Device::globalMemoryCacheType() const {
		return static_cast<DeviceMemoryCacheType>(
			getInfo<cl_device_mem_cache_type>(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE));
	}

	cl_uint Device::globalMemoryCachelineSize() const {
		return getInfo<cl_uint>(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
	}

	cl_ulong Device::globalMemorySize() const {
		return getInfo<cl_ulong>(CL_DEVICE_GLOBAL_MEM_SIZE);
	}

	cl_bool Device::hostUnifiedMemory() const {
		return getInfo<cl_bool>(CL_DEVICE_HOST_UNIFIED_MEMORY);
	}

	cl_bool Device::imageSupport() const {
		return getInfo<cl_bool>(CL_DEVICE_IMAGE_SUPPORT);
	}

	size_t Device::maxHeightImage2D() const {
		return getInfo<size_t>(CL_DEVICE_IMAGE2D_MAX_HEIGHT);
	}

	size_t Device::maxWidthImage2D() const {
		return getInfo<size_t>(CL_DEVICE_IMAGE2D_MAX_WIDTH);
	}

	size_t Device::maxHeightImage3D() const {
		return getInfo<size_t>(CL_DEVICE_IMAGE3D_MAX_HEIGHT);
	}

	size_t Device::maxWidthImage3D() const {
		return getInfo<size_t>(CL_DEVICE_IMAGE3D_MAX_WIDTH);
	}

	size_t Device::maxDepthImage3D() const {
		return getInfo<size_t>(CL_DEVICE_IMAGE3D_MAX_DEPTH);
	}

	cl_ulong Device::localMemorySize() const {
		return getInfo<cl_ulong>(CL_DEVICE_LOCAL_MEM_SIZE);
	}

	DeviceLocalMemoryType Device::localMemoryType() const {
		return static_cast<DeviceLocalMemoryType>(
			getInfo<cl_device_local_mem_type>(CL_DEVICE_LOCAL_MEM_TYPE));
	}

	cl_uint Device::maxClockFrequency() const {
		return getInfo<cl_uint>(CL_DEVICE_MAX_CLOCK_FREQUENCY);
	}

	cl_uint Device::maxComputeUnits() const {
		return getInfo<cl_uint>(CL_DEVICE_MAX_COMPUTE_UNITS);
	}

	cl_uint Device::maxConstantArguments() const {
		return getInfo<cl_uint>(CL_DEVICE_MAX_CONSTANT_ARGS);
	}

	cl_ulong Device::maxConstantBufferSize() const {
		return getInfo<cl_ulong>(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
	}

	cl_ulong Device::maxMemoryAllocationSize() const {
		return getInfo<cl_ulong>(CL_DEVICE_MAX_MEM_ALLOC_SIZE);
	}

	size_t Device::maxParameterSize() const {
		return getInfo<size_t>(CL_DEVICE_MAX_PARAMETER_SIZE);
	}

	cl_uint Device::maxReadImageArguments() const {
		return getInfo<cl_uint>(CL_DEVICE_MAX_READ_IMAGE_ARGS);
	}

	cl_uint Device::maxSamplers() const {
		return getInfo<cl_uint>(CL_DEVICE_MAX_SAMPLERS);
	}

	size_t Device::maxWorkGroupSize() const {
		return getInfo<size_t>(CL_DEVICE_MAX_WORK_GROUP_SIZE);
	}

	cl_uint Device::maxWorkItemDimensions() const {
		return getInfo<cl_uint>(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
	}

	std::vector<size_t> Device::maxWorkItemSizes() const {
		return getInfoVector<size_t>(CL_DEVICE_MAX_WORK_ITEM_SIZES);
	}

	cl_uint Device::maxWriteImageArguments() const {
		return getInfo<cl_uint>(CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
	}

	cl_uint Device::memoryBaseAddressAlign() const {
		return getInfo<cl_uint>(CL_DEVICE_MEM_BASE_ADDR_ALIGN);
	}

	cl_uint Device::minDataTypeAlignSize() const {
		return getInfo<cl_uint>(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
	}

	std::string Device::name() const {
		return getInfoString(CL_DEVICE_NAME);
	}

	cl_uint Device::nativeVectorWidth(ScalarType type) const {
		auto info_id = cl_device_info{};
		switch (type) {
			case ScalarType::char_type:   info_id = CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR;   break;
			case ScalarType::short_type:  info_id = CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT;  break;
			case ScalarType::int_type:    info_id = CL_DEVICE_NATIVE_VECTOR_WIDTH_INT;    break;
			case ScalarType::long_type:   info_id = CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG;   break;
			case ScalarType::float_type:  info_id = CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT;  break;
			case ScalarType::double_type: info_id = CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE; break;
			case ScalarType::half_type:   info_id = CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF;   break;
		}
		return getInfo<cl_uint>(info_id);
	}

	std::string Device::versionOpenCLC() const {
		return getInfoString(CL_DEVICE_OPENCL_C_VERSION);
	}

	Platform Device::platform() const {
		return {getInfo<cl_platform_id>(CL_DEVICE_PLATFORM)};
	}

	cl_uint Device::preferredVectorWidth(ScalarType type) const {
		auto info_id = cl_device_info{};
		switch (type) {
			case ScalarType::char_type: info_id   = CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR; break;
			case ScalarType::short_type: info_id  = CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT; break;
			case ScalarType::int_type: info_id    = CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT; break;
			case ScalarType::long_type: info_id   = CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG; break;
			case ScalarType::float_type: info_id  = CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT; break;
			case ScalarType::double_type: info_id = CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE; break;
			case ScalarType::half_type: info_id   = CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF; break;
		}
		return getInfo<cl_uint>(info_id);
	}

	std::string Device::profile() const {
		return getInfoString(CL_DEVICE_PROFILE);
	}

	size_t Device::profilingTimerResolution() const {
		return getInfo<size_t>(CL_DEVICE_PROFILING_TIMER_RESOLUTION);
	}

	CommandQueueProperties Device::queueProperties() const {
		return {getInfo<cl_command_queue_properties>(CL_DEVICE_QUEUE_PROPERTIES)};
	}

	DeviceType Device::type() const {
		return static_cast<DeviceType>(getInfo<cl_device_type>(CL_DEVICE_TYPE));
	}

	std::string Device::vendor() const {
		return getInfoString(CL_DEVICE_VENDOR);
	}

	cl_uint Device::vendorID() const {
		return getInfo<cl_uint>(CL_DEVICE_VENDOR_ID);
	}

	std::string Device::version() const {
		return getInfoString(CL_DEVICE_VERSION);
	}

	std::string Device::driverVersion() const {
		return getInfoString(CL_DRIVER_VERSION);
	}
}
