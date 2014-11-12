#include "memory_flags.hpp"

namespace cl {
		MemoryFlags::MemoryFlags(cl_map_flags mask) :
			m_mask{mask}
		{}

		MemoryFlags::MemoryFlags() :
			m_mask{CL_MEM_READ_WRITE}
		{}

		cl_mem_flags MemoryFlags::mask() const {
			return m_mask;
		}

		MemoryFlags & MemoryFlags::readWrite(cl_bool flag) {
			return modifyMask<CL_MEM_READ_WRITE>(flag);
		}

		MemoryFlags & MemoryFlags::writeOnly(cl_bool flag) {
			return modifyMask<CL_MEM_WRITE_ONLY>(flag);
		}

		MemoryFlags & MemoryFlags::readOnly(cl_bool flag) {
			return modifyMask<CL_MEM_READ_ONLY>(flag);
		}

		MemoryFlags & MemoryFlags::useHostPtr(cl_bool flag) {
			return modifyMask<CL_MEM_USE_HOST_PTR>(flag);
		}

		MemoryFlags & MemoryFlags::allocHostPtr(cl_bool flag) {
			return modifyMask<CL_MEM_ALLOC_HOST_PTR>(flag);
		}

		MemoryFlags & MemoryFlags::copyHostPtr(cl_bool flag) {
			return modifyMask<CL_MEM_COPY_HOST_PTR>(flag);
		}

		MemoryFlags & MemoryFlags::hostWriteOnly(cl_bool flag) {
			return modifyMask<CL_MEM_HOST_WRITE_ONLY>(flag);
		}

		MemoryFlags & MemoryFlags::hostReadOnly(cl_bool flag) {
			return modifyMask<CL_MEM_HOST_READ_ONLY>(flag);
		}

		MemoryFlags & MemoryFlags::hostNoAccess(cl_bool flag) {
			return modifyMask<CL_MEM_HOST_NO_ACCESS>(flag);
		}

		cl_bool MemoryFlags::readWrite() const {
			return readMask<CL_MEM_READ_WRITE>();
		}

		cl_bool MemoryFlags::writeOnly() const {
			return readMask<CL_MEM_WRITE_ONLY>();
		}

		cl_bool MemoryFlags::readOnly() const {
			return readMask<CL_MEM_READ_ONLY>();
		}

		cl_bool MemoryFlags::useHostPtr() const {
			return readMask<CL_MEM_USE_HOST_PTR>();
		}

		cl_bool MemoryFlags::allocHostPtr() const {
			return readMask<CL_MEM_ALLOC_HOST_PTR>();
		}

		cl_bool MemoryFlags::copyHostPtr() const {
			return readMask<CL_MEM_COPY_HOST_PTR>();
		}

		cl_bool MemoryFlags::hostWriteOnly() const {
			return readMask<CL_MEM_HOST_WRITE_ONLY>();
		}

		cl_bool MemoryFlags::hostReadOnly() const {
			return readMask<CL_MEM_HOST_READ_ONLY>();
		}

		cl_bool MemoryFlags::hostNoAccess() const {
			return readMask<CL_MEM_HOST_NO_ACCESS>();
		}
}
