#ifndef CPPCL_COMMAND_QUEUE_HEADER
#define CPPCL_COMMAND_QUEUE_HEADER

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
#include <algorithm>

namespace cl {
	class Context;
	class Device;
	class Event;

	struct CommandQueueInfo final {
	private:
		static const error::ErrorMap error_map;

	public:
		using cl_type = cl_command_queue;
		using info_type = cl_command_queue_info;
		using exception_type = CommandQueueException;

		static decltype(auto) func_release(cl_command_queue id) {
			error::handle<CommandQueueException>(clReleaseCommandQueue(id), error_map);
		}

		static decltype(auto) func_retain(cl_command_queue id) {
			error::handle<CommandQueueException>(clRetainCommandQueue(id), error_map);
		}

		static decltype(auto) func_info
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
		public Object<CommandQueueInfo>
	{
	private:
		struct operation final {
			struct read final {
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
				static decltype(auto) func_rect(
					cl_command_queue command_queue, cl_mem buffer, cl_bool blocking,
					size_t const* buffer_origin, size_t const* host_origin, size_t const* region,
					size_t buffer_row_pitch, size_t buffer_slice_pitch,
					size_t host_row_pitch, size_t host_slice_pitch,
					void * ptr,
					cl_uint num_events_in_wait_list, cl_event const* event_wait_list,
					cl_event * event
				) {
					return clEnqueueReadBufferRect(
						command_queue, buffer, blocking,
						buffer_origin, host_origin, region,
						buffer_row_pitch, buffer_slice_pitch,
						host_row_pitch, host_slice_pitch,
						ptr,
						num_events_in_wait_list, event_wait_list,
						event
					);
				}
			};

			struct write final {
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
				static decltype(auto) func_rect(
					cl_command_queue command_queue, cl_mem buffer, cl_bool blocking,
					size_t const* buffer_origin, size_t const* host_origin, size_t const* region,
					size_t buffer_row_pitch, size_t buffer_slice_pitch,
					size_t host_row_pitch, size_t host_slice_pitch,
					void * ptr,
					cl_uint num_events_in_wait_list, cl_event const* event_wait_list,
					cl_event * event
				) {
					return clEnqueueWriteBufferRect(
						command_queue, buffer, blocking,
						buffer_origin, host_origin, region,
						buffer_row_pitch, buffer_slice_pitch,
						host_row_pitch, host_slice_pitch,
						ptr,
						num_events_in_wait_list, event_wait_list,
						event
					);
				}
			};
		};


		/////////////////////////////////////////////////////////////////////////
		/// READ/WRITE BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType, CommandSync Sync, typename Operation, typename Iterator>
		typename std::conditional<Sync == CommandSync::blocking, void, Event>::type
		enqueueReadWriteBuffer(
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
				"iterators have to iterate through the same data type as buffer stores."
			);
			static_assert(
				std::is_trivial<DataType>::value,
				"iterator's and buffer's data type has to be trivial."
			);
			static_assert(
				std::is_same<Operation, operation::read>::value ||
				std::is_same<Operation, operation::write>::value,
				"operation template parameter can only be of type operation::read or operation::write!"
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
			const auto num_events = (events_in_wait_list != nullptr) ? events_in_wait_list->size() : 0;
			const auto count_elements = static_cast<size_t>(std::distance(first, last));
			if (Sync == CommandSync::blocking) {
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
		/////////////////////////////////////////////////////////////////////////
		/// READ/WRITE BUFFER - BEGIN
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
		/////////////////////////////////////////////////////////////////////////
		/// COPY BUFFER - END
		/////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////
		/// MAP BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
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
		/////////////////////////////////////////////////////////////////////////
		/// MAP BUFFER - END
		/////////////////////////////////////////////////////////////////////////


#if defined(CPPCL_CL_VERSION_1_1_ENABLED)
		/////////////////////////////////////////////////////////////////////////
		/// READ/WRITE BUFFER RECT - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <
			typename DataType,
			typename Iterator,
			typename Operation,
			size_t N,
			CommandSync Sync
		> typename std::conditional<Sync == CommandSync::blocking, void, Event>::type
		enqueueReadWriteBufferRect(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch,
			size_t buffer_slice_pitch,
			size_t host_row_pitch,
			size_t host_slice_pitch,
			std::vector<Event> const* event_wait_list
		) {
			static_assert(
				std::is_same<
					typename std::iterator_traits<Iterator>::iterator_category,
					std::random_access_iterator_tag
				>::value,
				"first has to be a random access iterator."
			);
			static_assert(
				std::is_same<typename std::iterator_traits<Iterator>::value_type, DataType>::value,
				"the given iterator has to iterate over the same data type as buffer stores."
			);
			static_assert(
				std::is_trivial<DataType>::value,
				"iterator's and buffer's data type has to be trivial."
			);
			static_assert(
				std::is_same<Operation, operation::read>::value ||
				std::is_same<Operation, operation::write>::value,
				"operation template parameter can only be of type operation::read or operation::write!"
			);
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_command_queue, "the command queue object is invalid."},
				{ErrorCode::invalid_context, "the context associated with this command queue and the given buffer are not the same."},
				{ErrorCode::invalid_memory_object, "the given buffer is invalid."},
				{ErrorCode::invalid_value, "specified offsets and regions to access invalid data out of bounds of the buffer areas; OR if any element in region is null; OR if any given pitch value is invalid."},
				{ErrorCode::invalid_event_wait_list, "one or more event objects in the given event list are invalid."},
				{ErrorCode::misaligned_sub_buffer_offset, "the given sub-buffer and offset is not aligned to CL_DEVICE_MEM_BASE_ADDR_ALIGN value for the device associated with this command queue."},		
				{ErrorCode::execute_status_error_for_events_in_wait_list, "read and write operations are blocking and there is one or more event in the given event list with an invalid status."},
				{ErrorCode::memory_object_allocation_failure, "failed to allocate memory for data store associated with the given buffer."},
				{ErrorCode::invalid_operation, "can not read from buffer which has been created with write only or host-no-access attributes."}
			};
			const auto data_size = sizeof(DataType);
			auto buffer_off_cplt = std::array<size_t, 3>{ {0, 0, 0} };
			auto host_off_cplt   = std::array<size_t, 3>{ {0, 0, 0} };
			auto region_cplt     = std::array<size_t, 3>{ {1, 1, 1} };
			for (int n = 0; n < N; ++n) {
				buffer_off_cplt[n] = buffer_offset[n] * data_size;
				host_off_cplt[n]   = host_offset[n] * data_size;
				region_cplt[n]     = region[n];
			}
			const auto num_events = (event_wait_list != nullptr)
				? event_wait_list->size()
				: 0;
			auto events = (event_wait_list != nullptr)
				? reinterpret_cast<const cl_event*>(event_wait_list->data())
				: nullptr;
			auto first_addr = reinterpret_cast<void*>(std::addressof(*first));
			if (Sync == CommandSync::blocking) {
				const auto error = Operation::func_rect(
					m_id, buffer.id(), true,
					buffer_off_cplt.data(), host_off_cplt.data(), region_cplt.data(),
					buffer_row_pitch, buffer_slice_pitch,
					host_row_pitch, host_slice_pitch,
					first_addr,
					num_events, events,
					nullptr
				);
				error::handle<CommandQueueException>(error, error_map);
			} else {
				auto event_id = cl_event{};
				const auto error = Operation::func_rect(
					m_id, buffer.id(), false,
					buffer_off_cplt.data(), host_off_cplt.data(), region_cplt.data(),
					buffer_row_pitch, buffer_slice_pitch,
					host_row_pitch, host_slice_pitch,
					first_addr,
					num_events, events,
					std::addressof(event_id)
				);
				error::handle<CommandQueueException>(error, error_map);
				return {event_id};
			}
		}
		/////////////////////////////////////////////////////////////////////////
		/// READ/WRITE BUFFER RECT - END
		/////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////
		/// COPY BUFFER RECT - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType, size_t N>
		Event enqueueCopyBufferRect(
			Buffer<DataType> const& src,
			Buffer<DataType> const& dst,
			std::array<size_t, N> const& src_off,
			std::array<size_t, N> const& dst_off,
			std::array<size_t, N> const& regions,
			size_t src_row_pitch,
			size_t src_slice_pitch,
			size_t dst_row_pitch,
			size_t dst_slice_pitch,
			std::vector<Event> const* event_wait_list
		) {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_command_queue, "this command queue is invalid."},
				{ErrorCode::invalid_context, "the context of this command queue is invalid."},
				{ErrorCode::invalid_memory_object, "src and/or dst buffers are invalid."},
				{ErrorCode::invalid_value, "specified offsets and regions to access invalid data out of bounds of the buffer areas; OR if any element in region is null; OR if any given pitch value is invalid."},
				{ErrorCode::memory_copy_overlap, "regions specified are overlapping memory regions within the given buffer objects."},
				{ErrorCode::misaligned_sub_buffer_offset, "src or dst buffers are subbuffers with invalid alignment."},
				{ErrorCode::memory_object_allocation_failure, "failed to allocate memory for data store associated with src or dst."}
			};
			const auto data_size = sizeof(DataType);
			auto src_off_cplt = std::array<size_t, 3>{ {0, 0, 0} };
			auto dst_off_cplt = std::array<size_t, 3>{ {0, 0, 0} };
			auto regions_cplt = std::array<size_t, 3>{ {1, 1, 1} };
			/*
			std::copy_n(src_off.begin(), N, src_off_cplt);
			std::copy_n(dst_off.begin(), N, dst_off_cplt);
			std::copy_n(regions.begin(), N, regions_cplt);
			const auto elements_to_bytes = [data_size](size_t elements) { return elements * data_size; };
			std::transform(src_off_cplt.begin(), src_off_cplt.end(), src_off_cplt.begin(), elements_to_bytes);
			std::transform(dst_off_cplt.begin(), dst_off_cplt.end(), dst_off_cplt.begin(), elements_to_bytes);
			std::transform(regions_cplt.begin(), regions_cplt.end(), regions_cplt.begin(), elements_to_bytes);
			*/
			for (int n = 0; n < N; ++n) {
				src_off_cplt[n] = src_off[n] * data_size;
				dst_off_cplt[n] = dst_off[n] * data_size;
				regions_cplt[n] = regions[n];
			}
			const auto num_events = (event_wait_list != nullptr) ? event_wait_list->size() : 0;
			auto event_id = cl_event{0};
			auto error = clEnqueueCopyBufferRect(
				m_id, src.id(), dst.id(),
				src_off_cplt.data(), dst_off_cplt.data(), regions_cplt.data(),
				src_row_pitch, src_slice_pitch,
				dst_row_pitch, dst_slice_pitch,
				num_events,
				(num_events > 0) ? reinterpret_cast<const cl_event*>(event_wait_list->data()) : nullptr,
				std::addressof(event_id)
			);
			error::handle<CommandQueueException>(error, error_map);
			return {event_id};
		}
		/////////////////////////////////////////////////////////////////////////
		/// COPY BUFFER RECT - END
		/////////////////////////////////////////////////////////////////////////
#endif

#if defined(CPPCL_CL_VERSION_1_2_ENABLED)
		/////////////////////////////////////////////////////////////////////////
		/// FILL BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType>
		Event enqueueFillBuffer(
			Buffer<DataType> const& buffer,
			DataType const& value,
			size_t offset,
			size_t count_elements,
			std::vector<Event> const* events_in_wait_list
		) {
			static const auto error_map = error::ErrorMap{
				{ErrorCode::invalid_command_queue, "this command queue is invalid."},
				{ErrorCode::invalid_context, "the given context is invalid."},
				{ErrorCode::invalid_memory_object, "the given buffer is an invalid memory object."},
				{ErrorCode::invalid_value, "the specified offset and count_elements requires accessing data outside the given buffer."},
				{ErrorCode::invalid_event_wait_list, "one or more event objects in the given event list are invalid."},
				{ErrorCode::misaligned_sub_buffer_offset, "buffer is a subbuffer with a misaligned offset."},
				{ErrorCode::memory_object_allocation_failure, "failed to allocated memory for data store associated with this buffer operation."}
			};
			const auto byte_offest = offset * sizeof(DataType);
			const auto byte_size = count_elements * sizeof(DataType);
			const auto num_events = (events_in_wait_list != nullptr) ? events_in_wait_list->size() : 0;
			auto event_id = cl_event{0};
			auto error = clEnqueueFillBuffer(
				m_id,
				buffer.id(),
				reinterpret_cast<const void*>(std::addressof(value)),
				sizeof(DataType),
				byte_offest,
				byte_size,
				num_events,
				(num_events > 0) ? reinterpret_cast<const cl_event*>(events_in_wait_list->data()) : nullptr,
				std::addressof(event_id)
			);
			error::handle<CommandQueueException>(error, error_map);
			return {event_id};
		}
		/////////////////////////////////////////////////////////////////////////
		/// FILL BUFFER - END
		/////////////////////////////////////////////////////////////////////////
#endif

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
			enqueueReadWriteBuffer<DataType, CommandSync::blocking, operation::read, Iterator>(
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
			return enqueueReadWriteBuffer<DataType, CommandSync::async, operation::read, Iterator>(
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
			enqueueReadWriteBuffer<DataType, CommandSync::blocking, operation::read, Iterator>(
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
			return enqueueReadWriteBuffer<DataType, CommandSync::async, operation::read, Iterator>(
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
			enqueueReadWriteBuffer<DataType, CommandSync::blocking, operation::read>(
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
			return enqueueReadWriteBuffer<DataType, CommandSync::async, operation::read>(
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
			enqueueReadWriteBuffer<DataType, CommandSync::blocking, operation::read>(
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
			return enqueueReadWriteBuffer<DataType, CommandSync::async, operation::read>(
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
			enqueueReadWriteBuffer<DataType, CommandSync::blocking, operation::write, Iterator>(
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
			return enqueueReadWriteBuffer<DataType, CommandSync::async, operation::write, Iterator>(
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
			enqueueReadWriteBuffer<DataType, CommandSync::blocking, operation::write, Iterator>(
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
			return enqueueReadWriteBuffer<DataType, CommandSync::async, operation::write, Iterator>(
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
			enqueueReadWriteBuffer<DataType, CommandSync::blocking, operation::write>(
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
			return enqueueReadWriteBuffer<DataType, CommandSync::async, operation::write>(
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
			enqueueReadWriteBuffer<DataType, CommandSync::blocking, operation::write>(
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
			return enqueueReadWriteBuffer<DataType, CommandSync::async, operation::write>(
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

#if defined(CPPCL_CL_VERSION_1_1_ENABLED)
		/////////////////////////////////////////////////////////////////////////
		/// READ BUFFER RECT - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType, typename Iterator, size_t N>
		void enqueueReadBufferRect(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch,
			size_t buffer_slice_pitch,
			size_t host_row_pitch,
			size_t host_slice_pitch,
			std::vector<Event> const& event_wait_list
		) {
			enqueueReadWriteBufferRect<DataType, Iterator, operation::read, N, CommandSync::blocking>(
				buffer, first,
				buffer_offset, host_offset, region,
				buffer_row_pitch, buffer_slice_pitch,
				host_row_pitch, host_slice_pitch,
				std::addressof(event_wait_list)
			);
		}

		template <typename DataType, typename Iterator, size_t N>
		Event enqueueReadBufferRectAsync(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch,
			size_t buffer_slice_pitch,
			size_t host_row_pitch,
			size_t host_slice_pitch,
			std::vector<Event> const& event_wait_list
		) {
			return enqueueReadWriteBufferRect<DataType, Iterator, operation::read, N, CommandSync::async>(
				buffer, first,
				buffer_offset, host_offset, region,
				buffer_row_pitch, buffer_slice_pitch,
				host_row_pitch, host_slice_pitch,
				std::addressof(event_wait_list)
			);
		}

		template <typename DataType, typename Iterator, size_t N>
		void enqueueReadBufferRect(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch = 0,
			size_t buffer_slice_pitch = 0,
			size_t host_row_pitch = 0,
			size_t host_slice_pitch = 0
		) {
			enqueueReadWriteBufferRect<DataType, Iterator, operation::read, N, CommandSync::blocking>(
				buffer, first,
				buffer_offset, host_offset, region,
				buffer_row_pitch, buffer_slice_pitch,
				host_row_pitch, host_slice_pitch,
				nullptr
			);
		}

		template <typename DataType, typename Iterator, size_t N>
		Event enqueueReadBufferRectAsync(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch = 0,
			size_t buffer_slice_pitch = 0,
			size_t host_row_pitch = 0,
			size_t host_slice_pitch = 0
		) {
			return enqueueReadWriteBufferRect<DataType, Iterator, operation::read, N, CommandSync::async>(
				buffer, first,
				buffer_offset, host_offset, region,
				buffer_row_pitch, buffer_slice_pitch,
				host_row_pitch, host_slice_pitch,
				nullptr
			);
		}
		/////////////////////////////////////////////////////////////////////////
		/// READ BUFFER RECT - END
		/////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////
		/// WRITE BUFFER RECT - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType, typename Iterator, size_t N>
		void enqueueWriteBufferRect(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch,
			size_t buffer_slice_pitch,
			size_t host_row_pitch,
			size_t host_slice_pitch,
			std::vector<Event> const& event_wait_list
		) {
			enqueueReadWriteBufferRect<DataType, Iterator, operation::write, N, CommandSync::blocking>(
				buffer, first,
				buffer_offset, host_offset, region,
				buffer_row_pitch, buffer_slice_pitch,
				host_row_pitch, host_slice_pitch,
				std::addressof(event_wait_list)
			);
		}

		template <typename DataType, typename Iterator, size_t N>
		Event enqueueWriteBufferRectAsync(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch,
			size_t buffer_slice_pitch,
			size_t host_row_pitch,
			size_t host_slice_pitch,
			std::vector<Event> const& event_wait_list
		) {
			return enqueueReadWriteBufferRect<DataType, Iterator, operation::write, N, CommandSync::async>(
				buffer, first,
				buffer_offset, host_offset, region,
				buffer_row_pitch, buffer_slice_pitch,
				host_row_pitch, host_slice_pitch,
				std::addressof(event_wait_list)
			);
		}

		template <typename DataType, typename Iterator, size_t N>
		void enqueueWriteBufferRect(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch = 0,
			size_t buffer_slice_pitch = 0,
			size_t host_row_pitch = 0,
			size_t host_slice_pitch = 0
		) {
			enqueueReadWriteBufferRect<DataType, Iterator, operation::write, N, CommandSync::blocking>(
				buffer, first,
				buffer_offset, host_offset, region,
				buffer_row_pitch, buffer_slice_pitch,
				host_row_pitch, host_slice_pitch,
				nullptr
			);
		}

		template <typename DataType, typename Iterator, size_t N>
		Event enqueueWriteBufferRectAsync(
			Buffer<DataType> const& buffer,
			Iterator first,
			std::array<size_t, N> const& buffer_offset,
			std::array<size_t, N> const& host_offset,
			std::array<size_t, N> const& region,
			size_t buffer_row_pitch = 0,
			size_t buffer_slice_pitch = 0,
			size_t host_row_pitch = 0,
			size_t host_slice_pitch = 0
		) {
			return enqueueReadWriteBufferRect<DataType, Iterator, operation::write, N, CommandSync::async>(
				buffer, first,
				buffer_offset, host_offset, region,
				buffer_row_pitch, buffer_slice_pitch,
				host_row_pitch, host_slice_pitch,
				nullptr
			);
		}
		/////////////////////////////////////////////////////////////////////////
		/// WRITE BUFFER RECT - END
		/////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////
		/// COPY BUFFER RECT - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType, size_t N>
		Event enqueueCopyBufferRect(
			Buffer<DataType> const& src,
			Buffer<DataType> const& dst,
			std::array<size_t, N> const& src_offset,
			std::array<size_t, N> const& dst_offset,
			std::array<size_t, N> const& region,
			size_t src_row_pitch,
			size_t src_slice_pitch,
			size_t dst_row_pitch,
			size_t dst_slice_pitch,
			std::vector<Event> const& event_wait_list
		) {
			return enqueueCopyBufferRect<DataType, N>(
				src, dst,
				src_offset, dst_offset, region,
				src_row_pitch, src_slice_pitch,
				dst_row_pitch, dst_slice_pitch,
				std::addressof(event_wait_list)
			);
		}

		template <typename DataType, size_t N>
		Event enqueueCopyBufferRect(
			Buffer<DataType> const& src,
			Buffer<DataType> const& dst,
			std::array<size_t, N> const& src_offset,
			std::array<size_t, N> const& dst_offset,
			std::array<size_t, N> const& region,
			size_t src_row_pitch = 0,
			size_t src_slice_pitch = 0,
			size_t dst_row_pitch = 0,
			size_t dst_slice_pitch = 0
		) {
			return enqueueCopyBufferRect<DataType, N>(
				src, dst,
				src_offset, dst_offset, region,
				src_row_pitch, src_slice_pitch,
				dst_row_pitch, dst_slice_pitch,
				nullptr
			);
		}

		template <typename DataType, size_t N>
		Event enqueueCopyBufferRect(
			Buffer<DataType> const& src,
			Buffer<DataType> const& dst,
			std::array<size_t, N> const& src_offset,
			std::array<size_t, N> const& dst_offset,
			std::array<size_t, N> const& region,
			std::vector<Event> const& event_wait_list
		) {
			return enqueueCopyBufferRect<DataType, N>(
				src, dst,
				src_offset, dst_offset, region,
				0, 0, 0, 0,
				std::addressof(event_wait_list)
			);
		}
		/////////////////////////////////////////////////////////////////////////
		/// COPY BUFFER RECT - END
		/////////////////////////////////////////////////////////////////////////
#endif

#if defined(CPPCL_CL_VERSION_1_2_ENABLED)
		Event migrateMemoryObject(
			std::vector<MemoryObject> const& memory_objects,
			cl_mem_migration_flags flags,
			std::vector<Event> const& event_wait_list
		); // TODO

		/////////////////////////////////////////////////////////////////////////
		/// FILL BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
		template <typename DataType>
		Event enqueueFillBuffer(
			Buffer<DataType> buffer,
			DataType const& value,
			size_t offset,
			size_t count_elements,
			std::vector<Event> const& events_in_wait_list
		) {
			return enqueueFillBuffer<DataType>(
				buffer, value, offset, count_elements, std::addressof(events_in_wait_list)
			);
		}

		template <typename DataType>
		Event enqueueFillBuffer(
			Buffer<DataType> buffer,
			DataType const& value,
			size_t offset,
			size_t count_elements
		) {
			return enqueueFillBuffer<DataType>(
				buffer, value, offset, count_elements, nullptr
			);
		}

		template <typename DataType>
		Event enqueueFillBuffer(
			Buffer<DataType> buffer,
			DataType const& value,
			size_t offset = 0
		) {
			return enqueueFillBuffer<DataType>(
				buffer, value, offset, buffer.count_elements(), nullptr
			);
		}
		/////////////////////////////////////////////////////////////////////////
		/// FILL BUFFER - BEGIN
		/////////////////////////////////////////////////////////////////////////
#endif
	};
}

/*
clGetSupportedImageFormats (Context)

	clCreateImage (Image)
	clCreateBuffer (Buffer)
	clCreateSubBuffer (Buffer)	
	clGetMemObjectInfo (MemoryObject)

clGetImageInfo (Image)
clRetainMemObject (MemoryObject)
clReleaseMemObject (MemoryObject)
clSetMemObjectDestructorCallback (MemoryObject) (OpenCL 1.1)

	clEnqueueReadBuffer
	clEnqueueWriteBuffer
	clEnqueueReadBufferRect
	clEnqueueWriteBufferRect
	clEnqueueFillBuffer
	clEnqueueCopyBuffer
	clEnqueueCopyBufferRect
	clEnqueueMapBuffer

TODO: all image enqueue functions
clEnqueueReadImage
clEnqueueWriteImage
clEnqueueFillImage (OpenCL v1.2)
clEnqueueCopyImage
clEnqueueMapImage

clEnqueueCopyImageToBuffer (OpenCL 1.0) - TODO
clEnqueueCopyBufferToImage (OpenCL 1.0) - TODO
clEnqueueMigrateMemObjects (OpenCL 1.2) - TODO

	clEnqueueUnmapMemObject (not required)
*/

#endif
