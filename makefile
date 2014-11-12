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

main.o : main.cpp src/wrapper.hpp src/error_handler.hpp src/exception.hpp
	$(CC) $(CFLAGS) main.cpp

common.o : src/common.hpp src/common.cpp
	$(CC) $(CFLAGS) src/common.cpp

wrapper.o : src/wrapper.hpp src/wrapper.cpp
	$(CC) $(CFLAGS) src/wrapper.cpp

object.o : src/object.hpp src/object.cpp src/wrapper.hpp 
	$(CC) $(CFLAGS) src/object.cpp

platform.o : src/platform.hpp src/platform.cpp src/object.hpp src/common.hpp
	$(CC) $(CFLAGS) src/platform.cpp

device.o : src/device.hpp src/device.cpp src/object.hpp src/fpconfig.hpp \
		   src/execution_capabilities.hpp src/command_queue_properties.hpp
	$(CC) $(CFLAGS) src/device.cpp

context.o : src/context.hpp src/context.cpp src/object.hpp src/context_properties.hpp
	$(CC) $(CFLAGS) src/context.cpp

fpconfig.o : src/fpconfig.hpp src/fpconfig.cpp
	$(CC) $(CFLAGS) src/fpconfig.cpp

execution_capabilities.o : src/execution_capabilities.hpp src/execution_capabilities.cpp
	$(CC) $(CFLAGS) src/execution_capabilities.cpp

command_queue_properties.o : src/command_queue_properties.hpp src/command_queue_properties.cpp
	$(CC) $(CFLAGS) src/command_queue_properties.cpp

context_properties.o : src/context_properties.hpp src/context_properties.cpp
	$(CC) $(CFLAGS) src/context_properties.cpp

exception.o : src/exception.hpp src/exception.cpp
	$(CC) $(CFLAGS) src/exception.cpp

error_handler.o : src/error_handler.hpp src/error_handler.cpp src/exception.hpp
	$(CC) $(CFLAGS) src/error_handler.cpp

command_queue.o : src/command_queue.hpp src/command_queue.cpp src/object.hpp src/error_handler.hpp \
				  src/command_queue_properties.hpp src/event.hpp src/buffer.hpp src/image.hpp \
				  src/map_buffer_flags.hpp src/mapped_buffer.hpp
	$(CC) $(CFLAGS) src/command_queue.cpp

event.o : src/event.hpp src/event.cpp src/error_handler.hpp src/command_queue.hpp
	$(CC) $(CFLAGS) src/event.cpp

memory_object.o : src/memory_object.hpp src/memory_object.cpp src/memory_flags.hpp
	$(CC) $(CFLAGS) src/memory_object.cpp

buffer.o : src/buffer.hpp src/buffer.cpp src/memory_object.hpp
	$(CC) $(CFLAGS) src/buffer.cpp

image.o : src/image.hpp src/image.cpp src/memory_object.hpp
	$(CC) $(CFLAGS) src/image.cpp

mapped_buffer.o : src/mapped_buffer.hpp src/mapped_buffer.cpp src/command_queue.hpp \
				  src/memory_object.hpp src/event.hpp
	$(CC) $(CFLAGS) src/mapped_buffer.cpp

memory_flags.o : src/memory_flags.hpp src/memory_flags.cpp src/wrapper.hpp
	$(CC) $(CFLAGS) src/memory_flags.cpp

clean:
	\rm *.o *~ src/*~ main
