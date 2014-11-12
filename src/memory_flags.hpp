#ifndef __CL_MEMORY_FLAGS_HEADER
#define __CL_MEMORY_FLAGS_HEADER

#include "wrapper.hpp"

namespace cl {
	struct MemoryFlags final {
	public:
		MemoryFlags(cl_map_flags mask);
		MemoryFlags();

		cl_mem_flags mask() const;

		MemoryFlags & readWrite(cl_bool);
		MemoryFlags & writeOnly(cl_bool);
		MemoryFlags & readOnly(cl_bool);
		MemoryFlags & useHostPtr(cl_bool);
		MemoryFlags & allocHostPtr(cl_bool);
		MemoryFlags & copyHostPtr(cl_bool);
		MemoryFlags & hostWriteOnly(cl_bool);
		MemoryFlags & hostReadOnly(cl_bool);
		MemoryFlags & hostNoAccess(cl_bool);

		cl_bool readWrite() const;
		cl_bool writeOnly() const;
		cl_bool readOnly() const;
		cl_bool useHostPtr() const;
		cl_bool allocHostPtr() const;
		cl_bool copyHostPtr() const;
		cl_bool hostWriteOnly() const;
		cl_bool hostReadOnly() const;
		cl_bool hostNoAccess() const;

	private:
		cl_mem_flags m_mask;

		template <cl_mem_flags Option>
		MemoryFlags & modifyMask(cl_bool flag) {
			if (flag) m_mask |=  Option;
			else      m_mask &= ~Option;
			return * this;
		}

		template <cl_mem_flags Option>
		cl_bool readMask() const {
			return !(m_mask & Option);
		}
	};
}

#endif
