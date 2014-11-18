Welcome to cppCL (C++ OpenCL)
=============================

**cppCL** is an object-oriented wrapper for the [OpenCL](http://www.khronos.org/opencl/) C API written in modern C++.

You're welcome to contribute to the project:
* test
* file issues about bugs
* send patches
* whatever else

License
=======

The code is licensed under the terms of GNU General Public License v3.0 (GNU GPLv3).

Build Instructions
==================

Since this project is still under heavy development there are no fully working builds yet.
This section will update as soon as there is a stable working version supporting at least OpenCL v1.0.

There won't be dependencies to other libraries than OpenCL itself and the C++ standard library.

Philosophie
===========

The philosophie behind this project is to create an OpenCL wrapper with a well designed and especially modern C++ interface as the official OpenCL C++ bindings are only a very thin wrapper around the OpenCL C API and thus do not match the requirements of a modern interface.

Error return codes are completely replaced by proper exception handling mechanisms.
Each OpenCL wrapper class has its own exception type to make it possible to selectively catch them as needed.
However, for compatibility reasons every thrown exception is equipped with its error return code from OpenCL.

This wrapper supports RAII in all its wrapper objects and takes away the manual reference counting of OpenCL objects and sub objects. Type safety especially of buffers is guaranteed via template objects and functions.

Wrapper objects provide helper functions to access the objects properties such as device's name() method so that the users of this wrapper do not have to use the standart OpenCL routine via info function and a MACRO parameter indicating the required information.

Documentation
=============

The documentation of the whole wrapper is planned as a separate .pdf file documenting all functions, enums, classes, structures etc. and their special behaviours. Documentation within the codes is not planned at the moment.

Design Goals
============

* modern C++
* exceptions > return codes
* nice interface > thin layer
* raii everywhere
* pointers are evil
* stick to OpenCL structure as close as possible but as far as a nice interface requires it


Coding Guidelines
=================

* indentation via TAB; filling with spaces
* one TAB indentation is equal to 4 spaces
* avoid MACROs
* prefer to stick to standard library
* use RAII everywhere where it is possible
* const correctness; especially for interfaces


Exception-Safety Guidelines
===========================

* Throw by value. Catch by reference.
* No dynamic exception specifications. Use noexcept.
* Destructors that throw are evil.
* Use RAII (Every responsibility is an object. One per.)
* All cleanup code called from a destructor.
* Support swapperator (With No-Throw Guarantee)
* Draw "Critical Lines" for the Strong Guarantee.
* Know where to catch (Switch/Strategy/Some Success)
* Prefer exceptions to error codes.
