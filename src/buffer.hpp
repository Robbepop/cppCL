#ifndef __CL_BUFFER_HEADER
#define __CL_BUFFER_HEADER

#include "memory_object.hpp"

namespace cl {

	template <typename T>
	class Buffer final : public MemoryObject<T> {

	};

}

#endif
