#ifndef __CL_BUFFER_HEADER
#define __CL_BUFFER_HEADER

#include "memory_object.hpp"
#include "error_handler.hpp"

#include <memory>

namespace cl {
	template <typename DataType>
	class Buffer final : public MemoryObject {
	private:
	public:
		Buffer<DataType>(cl_mem mem) :
			MemoryObject{mem}
		{}

		Buffer<DataType>(
			Context const& context,
			MemoryFlags const& flags,
			size_t count_elements,
			DataType* host_ptr = nullptr
		) :
			MemoryObject{0}
		{
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_context, "the given context is invalid."},
				{ErrorCode::invalid_buffer_size, "buffer size must be greater than zero and less than or equal to the maximum buffer size for all devices of the given conetxt."},
				{ErrorCode::invalid_host_ptr, "invalid use of host pointer parameter and associated map flags."},
				{ErrorCode::memory_object_allocation_failure, "there was a failure to allocate memory for the buffer object."}
			};
			auto error = cl_int{CL_SUCCESS};
			auto buffer_id = clCreateBuffer(
				context.id(),
				flags.mask(),
				count_elements * sizeof(DataType),
				reinterpret_cast<void*>(host_ptr),
				std::addressof(error)
			);
			if (error::handle<MemoryObjectException>(error, error_map)) m_id = buffer_id;
		}

		Buffer<DataType> createSubBuffer(
			MemoryFlags const& flags,
			size_t offset,
			size_t count_elements
		) {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_memory_object, "this buffer object is invalid."},
				{ErrorCode::invalid_value, "given flags for subbuffer and flags of this buffer do match an invalid pattern."},
				{ErrorCode::invalid_buffer_size, "buffer size must be greater than zero."},
				{ErrorCode::misaligned_sub_buffer_offset, "the given offset is no valid alignment for any of the devices of this buffer's context."},
			};
			auto error = cl_int{CL_SUCCESS};
			auto buffer_region = cl_buffer_region{};
			buffer_region.origin = offset * sizeof(DataType);
			buffer_region.size = count_elements * sizeof(DataType);
			auto subbuffer_id = clCreateSubBuffer(
				m_id,
				flags.mask(),
				CL_BUFFER_CREATE_TYPE_REGION,
				reinterpret_cast<const void*>(std::addressof(buffer_region)),
				std::addressof(error)
			);
			error::handle<MemoryObjectException>(error, error_map);
			return {subbuffer_id};
		}

		Buffer<DataType> associatedMemoryObject() const {
			return {MemoryObject::getInfo<cl_mem>(CL_MEM_ASSOCIATED_MEMOBJECT)};
		}

		size_t offset() const {
			return MemoryObject::getInfo<size_t>(CL_MEM_OFFSET);
		}

		size_t count_elements() const {
			return MemoryObject::size() / sizeof(DataType);
		}
	};

}

#endif
