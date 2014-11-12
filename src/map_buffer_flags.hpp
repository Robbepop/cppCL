#ifndef CL_MAP_BUFFER_FLAGS_HEADER
#define CL_MAP_BUFFER_FLAGS_HEADER

#include "wrapper.hpp"
#include "event.hpp"

#include <vector>

namespace cl {
	struct MapBufferFlags final {
	public:
		MapBufferFlags(cl_map_flags mask);
		MapBufferFlags(cl_bool read, cl_bool write, cl_bool write_invalidate_region);

		cl_map_flags mask() const;

		MapBufferFlags & read(cl_bool);
		MapBufferFlags & write(cl_bool);
		MapBufferFlags & write_invalidate_region(cl_bool);

		cl_bool read() const;
		cl_bool write() const;
		cl_bool write_invalidate_region() const;

	private:
		cl_map_flags m_mask;

		template <cl_map_flags Option>
		MapBufferFlags & modifyMask(cl_bool flag) {
			if (flag) m_mask |=  Option;
			else      m_mask &= ~Option;
			return * this;
		}

		template <cl_map_flags Option>
		cl_bool readMask() const {
			return !(m_mask & Option);
		}
	};
}

#endif
