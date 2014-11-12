OBJS = main.o common.o wrapper.o object.o platform.o device.o fpconfig.o \
	   execution_capabilities.o command_queue_properties.o context_properties.o context.o \
	   exception.o error_handler.o command_queue.o event.o memory_object.o buffer.o image.o \
	   mapped_buffer.o memory_flags.o
CC = g++
DEBUG = -g
#INC = -I/opt/intel/opencl-1.2-4.4.0.117/include
#LIB = -L/opt/intel/opencl-1.2-4.4.0.117/lib64 -lOpenCL
INC = -I/opt/AMDAPP/SDK/include
LIB = -L/opt/AMDAPP/SDK/lib -lOpenCL
GFLAGS = -Wall -Wextra -pedantic -std=c++14 -fdiagnostics-color=auto
CFLAGS = ${GFLAGS} ${LIB} ${INC} -c ${DEBUG}
LFLAGS = ${GFLAGS} ${LIB} ${INC} ${DEBUG}

main : ${OBJS}
	$(CC) $(LFLAGS) $(OBJS) -o main

main.o : main.cpp cppcl/wrapper.hpp cppcl/error_handler.hpp cppcl/exception.hpp
	$(CC) $(CFLAGS) main.cpp

common.o : cppcl/common.hpp cppcl/common.cpp
	$(CC) $(CFLAGS) cppcl/common.cpp

wrapper.o : cppcl/wrapper.hpp cppcl/wrapper.cpp
	$(CC) $(CFLAGS) cppcl/wrapper.cpp

object.o : cppcl/object.hpp cppcl/object.cpp cppcl/wrapper.hpp 
	$(CC) $(CFLAGS) cppcl/object.cpp

platform.o : cppcl/platform.hpp cppcl/platform.cpp cppcl/object.hpp cppcl/common.hpp
	$(CC) $(CFLAGS) cppcl/platform.cpp

device.o : cppcl/device.hpp cppcl/device.cpp cppcl/object.hpp cppcl/fpconfig.hpp \
		   cppcl/execution_capabilities.hpp cppcl/command_queue_properties.hpp
	$(CC) $(CFLAGS) cppcl/device.cpp

context.o : cppcl/context.hpp cppcl/context.cpp cppcl/object.hpp cppcl/context_properties.hpp
	$(CC) $(CFLAGS) cppcl/context.cpp

fpconfig.o : cppcl/fpconfig.hpp cppcl/fpconfig.cpp
	$(CC) $(CFLAGS) cppcl/fpconfig.cpp

execution_capabilities.o : cppcl/execution_capabilities.hpp cppcl/execution_capabilities.cpp
	$(CC) $(CFLAGS) cppcl/execution_capabilities.cpp

command_queue_properties.o : cppcl/command_queue_properties.hpp cppcl/command_queue_properties.cpp
	$(CC) $(CFLAGS) cppcl/command_queue_properties.cpp

context_properties.o : cppcl/context_properties.hpp cppcl/context_properties.cpp
	$(CC) $(CFLAGS) cppcl/context_properties.cpp

exception.o : cppcl/exception.hpp cppcl/exception.cpp
	$(CC) $(CFLAGS) cppcl/exception.cpp

error_handler.o : cppcl/error_handler.hpp cppcl/error_handler.cpp cppcl/exception.hpp
	$(CC) $(CFLAGS) cppcl/error_handler.cpp

command_queue.o : cppcl/command_queue.hpp cppcl/command_queue.cpp cppcl/object.hpp cppcl/error_handler.hpp \
				  cppcl/command_queue_properties.hpp cppcl/event.hpp cppcl/buffer.hpp cppcl/image.hpp \
				  cppcl/map_buffer_flags.hpp cppcl/mapped_buffer.hpp
	$(CC) $(CFLAGS) cppcl/command_queue.cpp

event.o : cppcl/event.hpp cppcl/event.cpp cppcl/error_handler.hpp cppcl/command_queue.hpp
	$(CC) $(CFLAGS) cppcl/event.cpp

memory_object.o : cppcl/memory_object.hpp cppcl/memory_object.cpp cppcl/memory_flags.hpp
	$(CC) $(CFLAGS) cppcl/memory_object.cpp

buffer.o : cppcl/buffer.hpp cppcl/buffer.cpp cppcl/memory_object.hpp
	$(CC) $(CFLAGS) cppcl/buffer.cpp

image.o : cppcl/image.hpp cppcl/image.cpp cppcl/memory_object.hpp
	$(CC) $(CFLAGS) cppcl/image.cpp

mapped_buffer.o : cppcl/mapped_buffer.hpp cppcl/mapped_buffer.cpp cppcl/command_queue.hpp \
				  cppcl/memory_object.hpp cppcl/event.hpp
	$(CC) $(CFLAGS) cppcl/mapped_buffer.cpp

memory_flags.o : cppcl/memory_flags.hpp cppcl/memory_flags.cpp cppcl/wrapper.hpp
	$(CC) $(CFLAGS) cppcl/memory_flags.cpp

clean:
	\rm *.o *~ cppcl/*~ main
