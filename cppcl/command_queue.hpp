#ifndef __CL_COMMAND_QUEUE_HEADER
#define __CL_COMMAND_QUEUE_HEADER

#include "object.hpp"
#include "error_handler.hpp"
#include "command_queue_properties.hpp"
#include "event.hpp"
#include "buffer.hpp"
#include "image.hpp"
#include "map_buffer_flags.hpp"
#include "mapped_buffer.hpp"

#include <iterator>
#include <type_traits>
#include <cassert>
#include <memory>

namespace cl {
	class Context;
	class Device;
	class Event;

	struct CommandQueueFunctions final {
	private:
		static const error::ErrorMap error_map;

	public:
		static decltype(auto) release(cl_command_queue id) {
			error::handle<CommandQueueException>(clReleaseCommandQueue(id), error_map);
		}

		static decltype(auto) retain(cl_command_queue id) {
			error::handle<CommandQueueException>(clRetainCommandQueue(id), error_map);
		}

		static decltype(auto) get_info
		(
			cl_command_queue context,
			cl_command_queue_info param_name,
			size_t param_value_size,
			void *param_value,
			size_t *param_value_size_ret
		) {
			return clGetCommandQueueInfo(
				context, param_name, param_value_size, param_value, param_value_size_ret);
		}
	};

	class CommandQueue final :
		public Object<cl_command_queue, cl_command_queue_info, CommandQueueFunctions, CommandQueueException>
	{
	private:
		struct read_operation final {
			using convert_type = void*;
			static decltype(auto) func(
				cl_command_queue command_queue,
				cl_mem buffer,
				cl_bool blocking_read,
				size_t offset,
				size_t size,
				void *ptr,
				cl_uint num_events_in_wait_list,
				const cl_event *event_wait_list,
				cl_event *event
			) {
				return clEnqueueReadBuffer(
					command_queue, buffer, blocking_read, offset, size, ptr,
					num_events_in_wait_list, event_wait_list, event
				);
			}
		};

		struct write_operation final {
			using convert_type = const void*;
			static decltype(auto) func(
				cl_command_queue command_queue,
				cl_mem buffer,
				cl_bool blocking_write,
				size_t offset,
				size_t size,
				const void *ptr,
				cl_uint num_events_in_wait_list,
				const cl_event *event_wait_list,
				cl_event *event
			) {
				return clEnqueueWriteBuffer(
					command_queue, buffer, blocking_write, offset, size, ptr,
					num_events_in_wait_list, event_wait_list, event
				);
			}
		};

		template <typename DataType, CommandSync Sync, typename Operation, typename Iterator>
		typename std::conditional<static_cast<cl_bool>(Sync), void, Event>::type
		enqueueReadWrite(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset,
			std::vector<Event> const* events_in_wait_list
		) {
			static_assert(
				std::is_same<
					typename std::iterator_traits<Iterator>::iterator_category,
					std::random_access_iterator_tag
				>::value,
				"it_begin and it_end have to be random access iterators."
			);
			static_assert(
				std::is_same<typename std::iterator_traits<Iterator>::value_type, DataType>::value,
				"interators have to iterate through the same data type as buffer stores."
			);
			static_assert(
				std::is_trivial<DataType>::value,
				"iterator's and buffer's data type has to be trivial."
			);
			static_assert(
				std::is_same<Operation, read_operation>::value ||
				std::is_same<Operation, write_operation>::value,
				"Operation template parameter can only be of type read_operation or write_operation!"
			);
			assert(first <= last);
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_command_queue, "the command queue object is invalid."},
				{ErrorCode::invalid_context, "the context associated with this command queue and the given buffer are not the same."},
				{ErrorCode::invalid_memory_object, "the given buffer is invalid."},
				{ErrorCode::invalid_value, "the region being read defined by (offset, size) is out of bounds; OR data points to invalid data; OR size is null."},
				{ErrorCode::invalid_event_wait_list, "one or more event objects in the given event list are invalid."},
				{ErrorCode::misaligned_sub_buffer_offset, "the given sub-buffer and offset is not aligned to CL_DEVICE_MEM_BASE_ADDR_ALIGN value for the device associated with this command queue."},		
				{ErrorCode::execute_status_error_for_events_in_wait_list, "read and write operations are blocking and there is one or more event in the given event list with an invalid status."},
				{ErrorCode::memory_object_allocation_failure, "failed to allocate memory for data store associated with the given buffer."},
				{ErrorCode::invalid_operation, "can not read from buffer which has been created with write only or host-no-access attributes."}
			};
			if (Sync == CommandSync::blocking) {
				const auto num_events = (events_in_wait_list != nullptr) ? events_in_wait_list->size() : 0;
				const auto count_elements = static_cast<size_t>(std::distance(first, last));
				const auto error = Operation::func(
					m_id,
					buffer.id(),
					static_cast<cl_bool>(Sync),
					buffer_offset * sizeof(DataType),
					count_elements * sizeof(DataType),
					reinterpret_cast<typename Operation::convert_type>(std::addressof(*first)),
					num_events,
					(num_events > 0) ? reinterpret_cast<const cl_event*>(events_in_wait_list->data()) : nullptr,
					nullptr
				);
				error::handle<CommandQueueException>(error, error_map);
			} else {
				auto event_id = cl_event{};
				const auto num_events = (events_in_wait_list != nullptr) ? events_in_wait_list->size() : 0;
				const auto count_elements = static_cast<size_t>(std::distance(first, last));
				const auto error = Operation::func(
					m_id,
					buffer.id(),
					static_cast<cl_bool>(Sync),
					buffer_offset * sizeof(DataType),
					count_elements * sizeof(DataType),
					reinterpret_cast<typename Operation::convert_type>(std::addressof(*first)),
					num_events,
					(num_events > 0) ? reinterpret_cast<const cl_event*>(events_in_wait_list->data()) : nullptr,
					std::addressof(event_id)
				);
				error::handle<CommandQueueException>(error, error_map);
				return {event_id};
			}
		}

		template <typename DataType>
		Event enqueueCopyBuffer(
			Buffer<DataType> src,
			Buffer<DataType> dst,
			size_t offset_src,
			size_t offset_dst,
			size_t count_elements,
			std::vector<Event> const* events_in_wait_list
		) {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_command_queue, "this command queue is invalid."},
				{ErrorCode::invalid_context, "the context associated with this command queue, the events in the wait list, the source and the destination buffer is not the same and thus invalid.}"},
				{ErrorCode::invalid_memory_object, "source and destination buffers are invalid buffer objects."},
				{ErrorCode::invalid_value, "this call requires accessing elements outside of the boundaries of the given source and destination buffers; OR the given size of the copy are is equal to 0."},
				{ErrorCode::invalid_event_wait_list, "there are invalid events in the given event list."},
				{ErrorCode::misaligned_sub_buffer_offset, "the offset of the source or destination buffer is a misaligned sub-buffer for the device associated with this command queue."},
				{ErrorCode::memory_copy_overlap, "the memory areas of the given source and destination areas do overlap; OR both given buffers are the same."},
				{ErrorCode::memory_object_allocation_failure, "there was a failure to allocate memory for the data store associated with the given source or destination buffer."}
			};
			auto event_id = cl_event{};
			const auto num_events = (events_in_wait_list != nullptr) ? events_in_wait_list->size() : 0;
			auto error = clEnqueueCopyBuffer(m_id, src.id(), dst.id(),
					offset_src * sizeof(DataType),
					offset_dst * sizeof(DataType),
					count_elements * sizeof(DataType),
					num_events,
					(num_events > 0) ? reinterpret_cast<const cl_event*>(events_in_wait_list->data()) : nullptr,
					& event_id
			);
			error::handle<CommandQueueException>(error, error_map);
			return {event_id};
		}

		template <typename DataType>
		MappedBuffer<DataType> enqueueMapBuffer(
			Buffer<DataType> buffer,
			MapBufferFlags flags,
			CommandSync sync,
			size_t count_elems,
			size_t elems_offset,
			std::vector<Event> const* events_in_wait_list
		) {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_command_queue, "the command queue object is invalid."},
				{ErrorCode::invalid_context, "the context associated with this command queue and the given buffer are not the same."},
				{ErrorCode::invalid_memory_object, "the given buffer is invalid."},
				{ErrorCode::invalid_value, "the region being read defined by (offset, size) is out of bounds; OR data points to invalid data; OR size is null."},
				{ErrorCode::invalid_event_wait_list, "one or more event objects in the given event list are invalid."},
				{ErrorCode::misaligned_sub_buffer_offset, "the given sub-buffer and offset is not aligned to CL_DEVICE_MEM_BASE_ADDR_ALIGN value for the device associated with this command queue."},		
				{ErrorCode::map_failure, "failed to map the requested memory region into the host address space."},
				{ErrorCode::execute_status_error_for_events_in_wait_list, "read and write operations are blocking and there is one or more event in the given event list with an invalid status."},
				{ErrorCode::memory_object_allocation_failure, "failed to allocate memory for data store associated with the given buffer."},
				{ErrorCode::invalid_operation, "can not read from buffer which has been created with write only or host-no-access attributes."}
			};
			const auto byte_offset = elems_offset * sizeof(DataType);
			const auto count_bytes = count_elems * sizeof(DataType);
			const auto num_events = (events_in_wait_list != nullptr) ? events_in_wait_list->size() : 0;
			auto event_id = cl_event{0};
			auto error = cl_int{CL_INVALID_VALUE};
			auto ptr = reinterpret_cast<DataType*>(
				clEnqueueMapBuffer(
					m_id,
					buffer.id(),
					static_cast<std::underlying_type<CommandSync>::type>(sync),
					flags.mask(),
					byte_offset * sizeof(DataType),
					count_bytes * sizeof(DataType),
					num_events,
					(num_events > 0) ? reinterpret_cast<const cl_event*>(events_in_wait_list->data()) : nullptr,
					std::addressof(event_id),
					std::addressof(error)
				)
			);
			error::handle<CommandQueueException>(error, error_map);
			return MappedBuffer<DataType>(*this, buffer, Event(event_id), ptr, count_elems);
		}

	public:
		CommandQueue(cl_command_queue command_queue_id);
		CommandQueue(Context const& context, Device const& device, CommandQueueProperties const& properties);

		Context context() const;
		Device device() const;
		cl_uint referenceCount() const;
		CommandQueueProperties properties() const;

		/////////////////////////////////////////////////////////////////////////
		/// READ BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType, typename Iterator>
		void enqueueRead(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset,
			std::vector<Event> const& events_in_wait_list
		) {
			enqueueReadWrite<DataType, CommandSync::blocking, read_operation, Iterator>(
				buffer, first, last, buffer_offset, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType, typename Iterator>
		Event enqueueReadAsync(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset,
			std::vector<Event> const& events_in_wait_list
		) {
			return enqueueReadWrite<DataType, CommandSync::async, read_operation, Iterator>(
				buffer, first, last, buffer_offset, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType, typename Iterator>
		void enqueueRead(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset = 0
		) {
			enqueueReadWrite<DataType, CommandSync::blocking, read_operation, Iterator>(
				buffer, first, last, buffer_offset, nullptr
			);
		}

		template <typename DataType, typename Iterator>
		Event enqueueReadAsync(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset = 0
		) {
			return enqueueReadWrite<DataType, CommandSync::async, read_operation, Iterator>(
				buffer, first, last, buffer_offset, nullptr
			);
		}

		template <typename DataType>
		void enqueueRead(
			Buffer<DataType> const& buffer,
			DataType & element,
			size_t buffer_offset,
			std::vector<Event> const& events_in_wait_list
		) {
			const auto addr = std::addressof(element);
			enqueueReadWrite<DataType, CommandSync::blocking, read_operation>(
				buffer, addr, addr + 1, buffer_offset, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		Event enqueueRead(
			Buffer<DataType> const& buffer,
			DataType & element,
			size_t buffer_offset,
			std::vector<Event> const& events_in_wait_list
		) {
			const auto addr = std::addressof(element);
			return enqueueReadWrite<DataType, CommandSync::async, read_operation>(
				buffer, addr, addr + 1, buffer_offset, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		void enqueueRead(
			Buffer<DataType> const& buffer,
			DataType & element,
			size_t buffer_offset = 0
		) {
			const auto addr = std::addressof(element);
			enqueueReadWrite<DataType, CommandSync::blocking, read_operation>(
				buffer, addr, addr + 1, buffer_offset, nullptr
			);
		}

		template <typename DataType>
		Event enqueueRead(
			Buffer<DataType> const& buffer,
			DataType & element,
			size_t buffer_offset = 0
		) {
			const auto addr = std::addressof(element);
			return enqueueReadWrite<DataType, CommandSync::async, read_operation>(
				buffer, addr, addr + 1, buffer_offset, nullptr
			);
		}
		/////////////////////////////////////////////////////////////////////////
		/// READ BUFFER - END
		/////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////
		/// WRITE BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType, typename Iterator>
		void enqueueWrite(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset,
			std::vector<Event> const& events_in_wait_list
		) {
			enqueueReadWrite<DataType, CommandSync::blocking, write_operation, Iterator>(
				buffer, first, last, buffer_offset, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType, typename Iterator>
		Event enqueueWriteAsync(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset,
			std::vector<Event> const& events_in_wait_list
		) {
			return enqueueReadWrite<DataType, CommandSync::async, write_operation, Iterator>(
				buffer, first, last, buffer_offset, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType, typename Iterator>
		void enqueueWrite(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset = 0
		) {
			enqueueReadWrite<DataType, CommandSync::blocking, write_operation, Iterator>(
				buffer, first, last, buffer_offset, nullptr
			);
		}

		template <typename DataType, typename Iterator>
		Event enqueueWriteAsync(
			Buffer<DataType> const& buffer,
			Iterator first,
			Iterator last,
			size_t buffer_offset = 0
		) {
			return enqueueReadWrite<DataType, CommandSync::async, write_operation, Iterator>(
				buffer, first, last, buffer_offset, nullptr
			);
		}

		template <typename DataType>
		void enqueueWrite(
			Buffer<DataType> const& buffer,
			DataType & element,
			size_t buffer_offset,
			std::vector<Event> const& events_in_wait_list
		) {
			const auto addr = std::addressof(element);
			enqueueReadWrite<DataType, CommandSync::blocking, write_operation>(
				buffer, addr, addr + 1, buffer_offset, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		Event enqueueWriteAsync(
			Buffer<DataType> const& buffer,
			DataType & element,
			size_t buffer_offset,
			std::vector<Event> const& events_in_wait_list
		) {
			const auto addr = std::addressof(element);
			return enqueueReadWrite<DataType, CommandSync::async, write_operation>(
				buffer, addr, addr + 1, buffer_offset, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		void enqueueWrite(
			Buffer<DataType> const& buffer,
			DataType & element,
			size_t buffer_offset = 0
		) {
			const auto addr = std::addressof(element);
			enqueueReadWrite<DataType, CommandSync::blocking, write_operation>(
				buffer, addr, addr + 1, buffer_offset, nullptr
			);
		}

		template <typename DataType>
		Event enqueueWriteAsync(
			Buffer<DataType> const& buffer,
			DataType & element,
			size_t buffer_offset = 0
		) {
			const auto addr = std::addressof(element);
			return enqueueReadWrite<DataType, CommandSync::async, write_operation>(
				buffer, addr, addr + 1, buffer_offset, nullptr
			);
		}
		/////////////////////////////////////////////////////////////////////////
		/// WRITE BUFFER - END
		/////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////
		/// COPY BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType>
		Event enqueueCopyBuffer(
			Buffer<DataType> src,
			Buffer<DataType> dst,
			size_t offset_src,
			size_t offset_dst,
			size_t count_elements,
			std::vector<Event> const& events_in_wait_list
		) {
			return enqueueCopyBuffer<DataType>(
				src, dst, offset_src, offset_dst, count_elements, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		Event enqueueCopyBuffer(
			Buffer<DataType> src,
			Buffer<DataType> dst,
			size_t count_elements,
			std::vector<Event> const& events_in_wait_list
		) {
			return enqueueCopyBuffer<DataType>(
				src, dst, 0, 0, count_elements, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		Event enqueueCopyBuffer(Buffer<DataType> src, Buffer<DataType> dst, size_t count_elements) {
			return enqueueCopyBuffer<DataType>(src, dst, 0, 0, count_elements, nullptr);
		}

		template <typename DataType>
		Event enqueueCopyBuffer(Buffer<DataType> src, Buffer<DataType> dst) {
			return enqueueCopyBuffer<DataType>(src, dst, 0, 0, src.size(), nullptr);
		}
		/////////////////////////////////////////////////////////////////////////
		/// COPY BUFFER - END
		/////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////
		/// MAP BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType>
		MappedBuffer<DataType> enqueueMapBuffer(Buffer<DataType> buffer) {
			return enqueueMapBuffer<DataType>(
				buffer,
				CommandSync::blocking,
				MapBufferFlags{}.read(true).write(true).write_invalidate_region(false),
				buffer.size(),
				0,
				nullptr
			);
		}

		template <typename DataType>
		MappedBuffer<DataType> enqueueMapBuffer(Buffer<DataType> buffer, MapBufferFlags flags) {
			return enqueueMapBuffer<DataType>(buffer, CommandSync::blocking, flags, buffer.size(), 0, nullptr);
		}

		template <typename DataType>
		MappedBuffer<DataType> const& enqueueMapBuffer(
			Buffer<DataType> buffer,
			MapBufferFlags flags,
			CommandSync sync
		) {
			return enqueueMapBuffer<DataType>(buffer, sync, flags, buffer.size(), 0, nullptr);
		}

		template <typename DataType>
		MappedBuffer<DataType> enqueueMapBuffer(
			Buffer<DataType> buffer,
			MapBufferFlags flags,
			CommandSync sync,
			size_t size
		) {
			return enqueueMapBuffer<DataType>(buffer, sync, flags, size, 0, nullptr);
		}

		template <typename DataType>
		MappedBuffer<DataType> enqueueMapBuffer(
			Buffer<DataType> buffer,
			MapBufferFlags flags,
			CommandSync sync,
			size_t size,
			size_t offset
		) {
			return enqueueMapBuffer<DataType>(buffer, sync, flags, size, offset, nullptr);
		}

		template <typename DataType>
		MappedBuffer<DataType> enqueueMapBuffer(
			Buffer<DataType> buffer,
			MapBufferFlags flags,
			CommandSync sync,
			std::vector<Event> const& events_in_wait_list
		) {
			return enqueueMapBuffer<DataType>(
				buffer, sync, flags, buffer.size(), 0, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		MappedBuffer<DataType> enqueueMapBuffer(
			Buffer<DataType> buffer,
			MapBufferFlags flags,
			CommandSync sync,
			size_t size,
			std::vector<Event> const& events_in_wait_list
		) {
			return enqueueMapBuffer<DataType>(
				buffer, sync, flags, size, 0, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		MappedBuffer<DataType> enqueueMapBuffer(
			Buffer<DataType> buffer,
			MapBufferFlags flags,
			CommandSync sync,
			size_t size,
			size_t offset,
			std::vector<Event> const& events_in_wait_list
		) {
			return enqueueMapBuffer<DataType>(
				buffer, sync, flags, size, offset, std::addressof(events_in_wait_list)
			);
		}
		/////////////////////////////////////////////////////////////////////////
		/// MAP BUFFER - END
		/////////////////////////////////////////////////////////////////////////

#if defined(CL_VERSION_110)
		Event enqueueReadBufferRect();
		Event enqueueWriteBufferRect();
		Event enqueueCopyBufferRect();
#endif

#if defined(CL_VERSION_120)
		Event enqueueFillBuffer();
#endif
	};
}

#endif
