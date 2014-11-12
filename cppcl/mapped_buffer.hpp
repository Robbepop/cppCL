#ifndef __CL_MAPPED_BUFFER_HEADER
#define __CL_MAPPED_BUFFER_HEADER

#include "command_queue.hpp"
#include "memory_object.hpp"
#include "event.hpp"

namespace cl {
	template <typename T>
	class MappedBuffer final {
		class CommandQueue;
		class MemoryObject;
		class Event;

	private:
		CommandQueue const& m_cmd_queue;
		MemoryObject const& m_mem_object;

		Event const& m_event;
		T* m_mapped_ptr;
		size_t m_count_elements;

	public:
		MappedBuffer(
			CommandQueue cmd_queue,
			MemoryObject mem_object,
			Event const& event,
			T* mapped_ptr,
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

		T const* cbegin() const {
			return reinterpret_cast<T const*>(m_mapped_ptr);
		}

		T const* cend() const {
			return reinterpret_cast<T const*>(m_mapped_ptr + m_count_elements);
		}

		T* begin() {
			return m_mapped_ptr;
		}

		T* end() {
			return m_mapped_ptr + m_count_elements;
		}

		size_t size() {
			return m_count_elements;
		}
	};
}

#endif
