#ifndef __CL_MEMORY_OBJECT_HEADER
#define __CL_MEMORY_OBJECT_HEADER

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

	template <typename T>
	class MemoryObject : public Object<cl_mem, cl_mem_info, MemoryObjectFunctions, MemoryObjectException> {
		MemoryObject() = delete;

	public:
		MemoryObjectType type() const;
		MemoryFlags flags() const;
		size_t size() const;

		T* hostPointer() const {
			return reinterpret_cast<T*>(getInfo<void*>(CL_MEM_HOST_PTR));
		}

		cl_uint mapCount() const;
		cl_uint referenceCount() const;
		Context const& context() const;
		MemoryObject const& associatedMemoryObject() const;
		size_t offset() const;
	};
}

#endif
