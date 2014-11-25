#include "image.hpp"

namespace cl {

}

/*
class MemoryObject

class Image extends MemoryObject

class Image1D extends Image (OpenCL 1.2)
    Image1D(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t width,
        void* host_ptr = NULL,
        cl_int* err = NULL
	)

class Image1DBuffer extends Image (OpenCL 1.2)
    Image1DBuffer(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t width,
        const Buffer &buffer,
        cl_int* err = NULL
	)

class Image1DArray extends Image (OpenCL 1.2)
    Image1DArray(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t arraySize,
        ::size_t width,
        ::size_t rowPitch,
        void* host_ptr = NULL,
        cl_int* err = NULL
	)

class Image2D extends Image (OpenCL 1.0)
    Image2D(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t width,
        ::size_t height,
        ::size_t row_pitch = 0,
        void* host_ptr = NULL,
        cl_int* err = NULL
	)

class Image2DGL extends Image (OpenCL <1.2 [deprecated for ImageGL])
    Image2DGL(
        const Context& context,
        cl_mem_flags flags,
        GLenum target,
        GLint  miplevel,
        GLuint texobj,
        cl_int * err = NULL
	)

class Image2DArray extends Image (OpenCL 1.2)
    Image2DArray(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t arraySize,
        ::size_t width,
        ::size_t height,
        ::size_t rowPitch,
        ::size_t slicePitch,
        void* host_ptr = NULL,
        cl_int* err = NULL
	)

class Image3D extends Image (OpenCL 1.0)
    Image3D(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t width,
        ::size_t height,
        ::size_t depth,
        ::size_t row_pitch = 0,
        ::size_t slice_pitch = 0,
        void* host_ptr = NULL,
        cl_int* err = NULL
	)

class Image3DGL extends Image (OpenCL v1.2)
    Image3DGL(
        const Context& context,
        cl_mem_flags flags,
        GLenum target,
        GLint  miplevel,
        GLuint texobj,
        cl_int * err = NULL
	)

class ImageGL extends Image (OpenCL v1.2)
	ImageGL(
		const Context& context,
		cl_mem_flags flags,
		GLenum target,
		GLint  miplevel,
		GLuint texobj,
		cl_int * err = NULL
	)
*/
