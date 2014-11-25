#ifndef CPPCL_MEMORY_OBJECT_HEADER
#define CPPCL_MEMORY_OBJECT_HEADER

#include "wrapper.hpp"
#include "object.hpp"
#include "context.hpp"
#include "memory_flags.hpp"
#include "error_handler.hpp"

namespace cl {
	struct MemoryObjectInfo final {
	private:
		static const error::ErrorMap error_map;

	public:
		using cl_type = cl_mem;
		using info_type = cl_mem_info;
		using exception_type = MemoryObjectException;

		static decltype(auto) func_release(cl_mem id) {
			error::handle<CommandQueueException>(clReleaseMemObject(id), error_map);
		}

		static decltype(auto) func_retain(cl_mem id) {
			error::handle<CommandQueueException>(clRetainMemObject(id), error_map);
		}

		static decltype(auto) func_info
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

	//class MemoryObject : public Object<cl_mem, cl_mem_info, MemoryObjectFunctions, MemoryObjectException> {
	class MemoryObject : public Object<MemoryObjectInfo> {
	private:
		MemoryObject() = delete;

	protected:
		using Object::Object;

		template <typename DataType>
		DataType* hostPointer() const {
			return reinterpret_cast<DataType*>(getInfo<void*>(CL_MEM_HOST_PTR));
		}

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

		Context context() const {
			return {getInfo<cl_context>(CL_MEM_CONTEXT)};
		}
	};
}

#endif
