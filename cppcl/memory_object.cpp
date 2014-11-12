#include "memory_object.hpp"

namespace cl {
	MemoryObjectType MemoryObject::type() const {
		return static_cast<MemoryObjectType>(getInfo<cl_mem_object_type>(CL_MEM_TYPE));
	}

	MemoryFlags MemoryObject::flags() const {
		return MemoryFlags(getInfo<cl_mem_flags>(CL_MEM_FLAGS));
	}

	size_t MemoryObject::size() const {
		return getInfo<size_t>(CL_MEM_SIZE);
	}

	cl_uint MemoryObject::mapCount() const {
		return getInfo<cl_uint>(CL_MEM_MAP_COUNT);
	}

	cl_uint MemoryObject::referenceCount() const {
		return getInfo<cl_uint>(CL_MEM_REFERENCE_COUNT);
	}

	Context const& MemoryObject::context() const {
		return {getInfo<cl_context>(CL_MEM_CONTEXT)};
	}

	MemoryObject const& MemoryObject::associatedMemoryObject() const {
		return {getInfo<cl_mem_object>(CL_MEM_ASSOCIATED_MEMOBJECT)};
	}

	size_t MemoryObject::offset() const {
		return getInfo<size_t>(CL_MEM_OFFSET);
	}
}
