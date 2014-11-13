#ifndef __CL_MEMORY_OBJECT_HEADER
#define __CL_MEMORY_OBJECT_HEADER

#include "wrapper.hpp"
#include "object.hpp"
#include "context.hpp"
#include "memory_flags.hpp"
#include "error_handler.hpp"

namespace cl {
	struct MemoryObjectFunctions final {
	private:
		static const error::ErrorMap error_map;

	public:
		static decltype(auto) release(cl_mem id) {
			error::handle<CommandQueueException>(clReleaseMemObject(id), error_map);
		}

		static decltype(auto) retain(cl_mem id) {
			error::handle<CommandQueueException>(clRetainMemObject(id), error_map);
		}

		static decltype(auto) get_info
		(
			cl_mem memory_object,
			cl_mem_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret
		) {
			return clGetMemObjectInfo(
				memory_object, param_name, param_value_size, param_value, param_value_size_ret);
		}
	};

	template <typename DataType>
	class MemoryObject : public Object<cl_mem, cl_mem_info, MemoryObjectFunctions, MemoryObjectException> {
	private:
		MemoryObject() = delete;

	public:
		MemoryObjectType type() const {
			return static_cast<MemoryObjectType>(getInfo<cl_mem_object_type>(CL_MEM_TYPE));
		}

		MemoryFlags flags() const {
			return MemoryFlags(getInfo<cl_mem_flags>(CL_MEM_FLAGS));
		}

		size_t size() const {
			return getInfo<size_t>(CL_MEM_SIZE);
		}

		cl_uint mapCount() const {
			return getInfo<cl_uint>(CL_MEM_MAP_COUNT);
		}

		cl_uint referenceCount() const {
			return getInfo<cl_uint>(CL_MEM_REFERENCE_COUNT);
		}

		Context const& context() const {
			return {getInfo<cl_context>(CL_MEM_CONTEXT)};
		}

		MemoryObject const& associatedMemoryObject() const {
			return {getInfo<cl_mem>(CL_MEM_ASSOCIATED_MEMOBJECT)};
		}

		size_t offset() const {
			return getInfo<size_t>(CL_MEM_OFFSET);
		}

		DataType* hostPointer() const {
			return reinterpret_cast<DataType*>(getInfo<void*>(CL_MEM_HOST_PTR));
		}
	};
}

#endif
