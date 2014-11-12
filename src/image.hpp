#ifndef __CL_IMAGE_HEADER
#define __CL_IMAGE_HEADER

#include "memory_object.hpp"

namespace cl {

	template <typename T>
	class Image final : public MemoryObject<T> {

	};

}

#endif
