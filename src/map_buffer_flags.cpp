#include "map_buffer_flags.hpp"

namespace cl {
	MapBufferFlags::MapBufferFlags(cl_map_flags mask) :
		m_mask{mask}
	{}

	MapBufferFlags::MapBufferFlags(cl_bool p_read, cl_bool p_write, cl_bool p_write_invalidate_region) {
		this->read(p_read);
		this->write(p_write);
		this->write_invalidate_region(p_write_invalidate_region);
	}

	cl_map_flags MapBufferFlags::mask() const {
		return m_mask;
	}

	MapBufferFlags & MapBufferFlags::read(cl_bool flag) {
		modifyMask<CL_MAP_READ>(flag);
		return * this;
	}

	MapBufferFlags & MapBufferFlags::write(cl_bool flag) {
		modifyMask<CL_MAP_WRITE>(flag);
		return * this;
	}

	MapBufferFlags & MapBufferFlags::write_invalidate_region(cl_bool flag) {
		modifyMask<CL_MAP_WRITE_INVALIDATE_REGION>(flag);
		return * this;
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
