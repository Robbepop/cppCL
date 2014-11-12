#ifndef __CL_WRAPPER_HEADER
#define __CL_WRAPPER_HEADER

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <string>

#define CL_VERSION_100
#define CL_VERSION_110
#define CL_VERSION_120
//#define CL_VERSION_200

#define CL_DEVICE_HALF_FP_CONFIG 37

namespace cl {
	enum class AddressingMode : cl_addressing_mode {
		none = CL_ADDRESS_NONE,
		clamp_to_edge = CL_ADDRESS_CLAMP_TO_EDGE,
		clamp = CL_ADDRESS_CLAMP,
		repeat = CL_ADDRESS_REPEAT
	};

	enum class BufferCreateType : cl_buffer_create_type {
		// not yet filled as of OpenCL v1.1
	};

	enum class BuildStatus : cl_build_status {
		error = CL_BUILD_ERROR,
		in_progress = CL_BUILD_IN_PROGRESS,
		none = CL_BUILD_NONE,
		success = CL_BUILD_SUCCESS
	};

	enum class CommandSync : cl_bool {
		blocking = CL_TRUE,
		async = CL_FALSE
	};

	enum class ChannelOrder : cl_channel_order {
		a = CL_A,
		r = CL_R,
		rg = CL_RG,
		ra = CL_RA,
		rgb = CL_RGB,
		rgba = CL_RGBA,
		bgra = CL_BGRA,
		argb = CL_ARGB,
		intensity = CL_INTENSITY,
		luminance = CL_LUMINANCE
	};

	enum class ChannelType : cl_channel_type {
		t_float = CL_FLOAT,
		t_half_float = CL_HALF_FLOAT,
		t_signed_int8 = CL_SIGNED_INT8,
		t_signed_int16 = CL_SIGNED_INT16,
		t_signed_int32 = CL_SIGNED_INT32,
		t_unsigned_int8 = CL_UNSIGNED_INT8,
		t_unsigned_int16 = CL_UNSIGNED_INT16,
		t_unsigned_int32 = CL_UNSIGNED_INT32,
		t_snorm_int8 = CL_SNORM_INT8,
		t_snorm_int16 = CL_SNORM_INT16,
		t_unorm_int8 = CL_UNORM_INT8,
		t_unorm_int16 = CL_UNORM_INT16,
		t_unorm_int101010 = CL_UNORM_INT_101010,
		t_unorm_short_555 = CL_UNORM_SHORT_555,
		t_unorm_short_565 = CL_UNORM_SHORT_565
	};

	enum class CommandType : cl_command_type {
		acquire_gl_objects = CL_COMMAND_ACQUIRE_GL_OBJECTS,
		copy_buffer = CL_COMMAND_COPY_BUFFER,
		copy_buffer_to_image = CL_COMMAND_COPY_BUFFER_TO_IMAGE,
		copy_image = CL_COMMAND_COPY_IMAGE,
		copy_image_to_buffer = CL_COMMAND_COPY_IMAGE_TO_BUFFER,
		map_buffer = CL_COMMAND_MAP_BUFFER,
		map_image = CL_COMMAND_MAP_IMAGE,
		marker = CL_COMMAND_MARKER,
		native_kernel = CL_COMMAND_NATIVE_KERNEL,
		nd_range_kernel = CL_COMMAND_NDRANGE_KERNEL,
		read_buffer = CL_COMMAND_READ_BUFFER,
		read_image = CL_COMMAND_READ_IMAGE,
		release_gl_objects = CL_COMMAND_RELEASE_GL_OBJECTS,
		task = CL_COMMAND_TASK,
		unmap_mem_object = CL_COMMAND_UNMAP_MEM_OBJECT,
		write_buffer = CL_COMMAND_WRITE_BUFFER,
		write_image = CL_COMMAND_WRITE_IMAGE
	};

	// cl_d3d10_device_set_khr - not implemented yet by intel sdk
	enum class Direct3D10_DeviceSetKHR {// : cl_d3d10_device_set_khr {
		// not yet filled as of OpenCL v1.1
	};

	// cl_d3d10_device_source_khr - not implemented yet by intel sdk
	enum class Direct3D10_DeviceSourceKHR {// : cl_d3d10_device_source_khr {
		// not yet filled as of OpenCL v1.1
	};

	enum class DeviceLocalMemoryType : cl_device_local_mem_type {
		global = CL_GLOBAL,
		local = CL_LOCAL
	};

	enum class DeviceMemoryCacheType : cl_device_mem_cache_type {
		none = CL_NONE,
		read_only_cache = CL_READ_ONLY_CACHE,
		read_write_cache = CL_READ_WRITE_CACHE
	};

	enum class DeviceType : cl_device_type {
		default_type = CL_DEVICE_TYPE_DEFAULT,
		cpu = CL_DEVICE_TYPE_CPU,
		gpu = CL_DEVICE_TYPE_GPU,
		accelerator = CL_DEVICE_TYPE_ACCELERATOR,
		all = CL_DEVICE_TYPE_ALL
	};

	enum class FilterMode : cl_filter_mode {
		nearest = CL_FILTER_NEAREST,
		linear = CL_FILTER_LINEAR
	};

	// not implemented by the intel sdk
	/*
	enum class GLObjectType : cl_gl_object_type {
		buffer = CL_GL_OBJECT_BUFFER,
		texture2d = CL_GL_OBJECT_TEXTURE2D,
		texture3d = CL_GL_OBJECT_TEXTURE3D,
		render_buffer = CL_GL_OBJECT_RENDERBUFFER
	};
	*/

	enum class MapFlags : cl_map_flags {
		read = CL_MAP_READ,
		write = CL_MAP_WRITE
	};

	// cl_mem_fence_flags - not implemented yet by intel sdk
	enum class MemoryFenceFlags {// : cl_mem_fence_flags {
		// not yet filled as of OpenCL v1.1
	};

	/*
	enum class MemoryFlags : cl_mem_flags {
		read_write = CL_MEM_READ_WRITE,
		write_only = CL_MEM_WRITE_ONLY,
		read_only = CL_MEM_READ_ONLY,
		use_host_ptr = CL_MEM_USE_HOST_PTR,
		alloc_host_ptr = CL_MEM_ALLOC_HOST_PTR,
		copy_host_ptr = CL_MEM_COPY_HOST_PTR
	};
	*/

	enum class MemoryObjectType : cl_mem_object_type {
		buffer = CL_MEM_OBJECT_BUFFER,
		image2d = CL_MEM_OBJECT_IMAGE2D,
		image3d = CL_MEM_OBJECT_IMAGE3D
	};

	enum class FloatingPointType : cl_device_info {
		half_t = CL_DEVICE_HALF_FP_CONFIG,
		single_t = CL_DEVICE_SINGLE_FP_CONFIG,
		double_t = CL_DEVICE_DOUBLE_FP_CONFIG
	};

	enum class ScalarType {
		char_type,
		short_type,
		int_type,
		long_type,
		float_type,
		double_type,
		half_type
	};

	enum class ErrorCode : cl_int {
		success                                         = CL_SUCCESS,

		device_not_found                                = CL_DEVICE_NOT_FOUND,
		device_not_available                            = CL_DEVICE_NOT_AVAILABLE,
		compiler_not_available                          = CL_COMPILER_NOT_AVAILABLE,
		memory_object_allocation_failure                = CL_MEM_OBJECT_ALLOCATION_FAILURE,
		out_of_resources                                = CL_OUT_OF_RESOURCES,
		out_of_host_memory                              = CL_OUT_OF_HOST_MEMORY,
		profiling_info_not_available                    = CL_PROFILING_INFO_NOT_AVAILABLE,
		memory_copy_overlap                             = CL_MEM_COPY_OVERLAP,
		image_format_mismatch                           = CL_IMAGE_FORMAT_MISMATCH,
		image_format_not_supported                      = CL_IMAGE_FORMAT_NOT_SUPPORTED,
		build_program_failure                           = CL_BUILD_PROGRAM_FAILURE,
		map_failure                                     = CL_MAP_FAILURE,
		misaligned_sub_buffer_offset                    = CL_MISALIGNED_SUB_BUFFER_OFFSET,
		execute_status_error_for_events_in_wait_list    = CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST,
		compile_program_failure                         = CL_COMPILE_PROGRAM_FAILURE,
		linker_not_available                            = CL_LINKER_NOT_AVAILABLE,
		link_program_failure                            = CL_LINK_PROGRAM_FAILURE,
		device_partition_failed                         = CL_DEVICE_PARTITION_FAILED,
		kernel_argument_info_not_available              = CL_KERNEL_ARG_INFO_NOT_AVAILABLE,

		invalid_value									= CL_INVALID_VALUE,
		invalid_device_type								= CL_INVALID_DEVICE_TYPE,
		invalid_platform								= CL_INVALID_PLATFORM,
		invalid_device									= CL_INVALID_DEVICE,
		invalid_context									= CL_INVALID_CONTEXT,
		invalid_queue_properties						= CL_INVALID_QUEUE_PROPERTIES,
		invalid_command_queue							= CL_INVALID_COMMAND_QUEUE,
		invalid_host_ptr								= CL_INVALID_HOST_PTR,
		invalid_memory_object							= CL_INVALID_MEM_OBJECT,
		invalid_image_format_descriptor					= CL_INVALID_IMAGE_FORMAT_DESCRIPTOR,
		invalid_image_size								= CL_INVALID_IMAGE_SIZE,
		invalid_sampler									= CL_INVALID_SAMPLER,
		invalid_binary									= CL_INVALID_BINARY,
		invalid_build_options							= CL_INVALID_BUILD_OPTIONS,
		invalid_program									= CL_INVALID_PROGRAM,
		invalid_program_executable						= CL_INVALID_PROGRAM_EXECUTABLE,
		invalid_kernel_name								= CL_INVALID_KERNEL_NAME,
		invalid_kernel_definition						= CL_INVALID_KERNEL_DEFINITION,
		invalid_kernel									= CL_INVALID_KERNEL,
		invalid_argument_index							= CL_INVALID_ARG_INDEX,
		invalid_argument_value							= CL_INVALID_ARG_VALUE,
		invalid_argument_size							= CL_INVALID_ARG_SIZE,
		invalid_kernel_arguments						= CL_INVALID_KERNEL_ARGS,
		invalid_work_dimension							= CL_INVALID_WORK_DIMENSION,
		invalid_work_group_size							= CL_INVALID_WORK_GROUP_SIZE,
		invalid_work_item_size							= CL_INVALID_WORK_ITEM_SIZE,
		invalid_global_offset							= CL_INVALID_GLOBAL_OFFSET,
		invalid_event_wait_list							= CL_INVALID_EVENT_WAIT_LIST,
		invalid_event									= CL_INVALID_EVENT,
		invalid_operation								= CL_INVALID_OPERATION,
		invalid_gl_object								= CL_INVALID_GL_OBJECT,
		invalid_buffer_size								= CL_INVALID_BUFFER_SIZE,
		invalid_mip_level								= CL_INVALID_MIP_LEVEL,
		invalid_global_work_size						= CL_INVALID_GLOBAL_WORK_SIZE,
		invalid_property								= CL_INVALID_PROPERTY,
		invalid_image_descriptor						= CL_INVALID_IMAGE_DESCRIPTOR,
		invalid_compiler_options						= CL_INVALID_COMPILER_OPTIONS,
		invalid_linker_options							= CL_INVALID_LINKER_OPTIONS,
		invalid_device_partition_count					= CL_INVALID_DEVICE_PARTITION_COUNT
	};

	enum class DeviceAffinityDomain : cl_device_affinity_domain {
		numa = CL_DEVICE_AFFINITY_DOMAIN_NUMA,
		l4_cache           = CL_DEVICE_AFFINITY_DOMAIN_L4_CACHE,
		l3_cache           = CL_DEVICE_AFFINITY_DOMAIN_L3_CACHE,
		l2_cache           = CL_DEVICE_AFFINITY_DOMAIN_L2_CACHE,
		l1_cache           = CL_DEVICE_AFFINITY_DOMAIN_L1_CACHE,
		next_partitionable = CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE
	};
}

#endif















