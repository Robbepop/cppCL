#include "map_buffer_flags.hpp"

namespace cl {
	MapBufferFlags::MapFlags(cl_map_flags mask) :
		m_mask{mask}
	{}

	MapBufferFlags::MapFlags(cl_bool p_read, cl_bool p_write, cl_bool p_write_invalidate_region) {
		this->read(p_read);
		this->write(p_write);
		this->write_invalidate_region(p_write_invalidate_region);
	}

	cl_map_flags MapBufferFlags::mask() const {
		return m_mask;
	}

	MapFlags & MapBufferFlags::read(cl_bool flag) {
		modifyMask<CL_MAP_READ>(flag);
	}

	MapFlags & MapBufferFlags::write(cl_bool flag) {
		modifyMask<CL_MAP_WRITE>(flag);
	}

	MapFlags & MapBufferFlags::write_invalidate_region(cl_bool flag) {
		modifyMask<CL_MAP_WRITE_INVALIDATE_REGION>(flag);
	}

	cl_bool MapBufferFlags::read() const {
		return readMask<CL_MAP_READ>();
	}

	cl_bool MapBufferFlags::write() const {
		return readMask<CL_MAP_WRITE>();
	}

	cl_bool MapBufferFlags::write_invalidate_region() const {
		return readMask<CL_MAP_WRITE_INVALIDATE_REGION>();
	}
}
