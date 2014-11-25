#ifndef CPPCL_MAPPED_BUFFER_HEADER
#define CPPCL_MAPPED_BUFFER_HEADER

#include "command_queue.hpp"
#include "memory_object.hpp"
#include "event.hpp"

namespace cl {
	template <typename DataType>
	class MappedBuffer final {
		class CommandQueue;
		class MemoryObject;
		class Event;

	private:
		CommandQueue const& m_cmd_queue;
		MemoryObject const& m_mem_object;

		Event const& m_event;
		DataType* m_mapped_ptr;
		size_t m_count_elements;

	public:
		MappedBuffer(
			CommandQueue cmd_queue,
			MemoryObject mem_object,
			Event const& event,
			DataType* mapped_ptr,
			size_t count_elements
		) :
			m_cmd_queue{cmd_queue},
			m_mem_object{mem_object},
			m_event{event},
			m_mapped_ptr{mapped_ptr},
			m_count_elements{count_elements}
		{}

		~MappedBuffer() {
			auto error = clEnqeueUnmapMemObject(
				m_cmd_queue.id(),
				m_mem_object.id(),
				reinterpret_cast<void*>(m_mapped_ptr),
				0,
				nullptr,
				nullptr
			);
		}

		Event const& event() const {
			return m_event;
		}

		DataType const* cbegin() const {
			return reinterpret_cast<DataType const*>(m_mapped_ptr);
		}

		DataType const* cend() const {
			return reinterpret_cast<DataType const*>(m_mapped_ptr + m_count_elements);
		}

		DataType* begin() {
			return m_mapped_ptr;
		}

		DataType* end() {
			return m_mapped_ptr + m_count_elements;
		}

		size_t size() {
			return m_count_elements;
		}
	};
}

#endif
