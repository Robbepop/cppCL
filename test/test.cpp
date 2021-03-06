#include <iostream>
#include <string>

#include "cppcl.hpp"

#include <type_traits>
#include <unordered_map>
#include <map>

int main(int, const char**) {
	auto platforms = cl::Platform::getPlatforms();
	std::cout << "Count platforms: " << platforms.size() << "\n";
	auto platform = platforms[0];

	std::cout << "Platform ...\n"
			  << "\tProfile: " << platform.profile() << "\n"
			  << "\tName: " << platform.name() << "\n"
			  << "\tVendor: " << platform.vendor() << "\n"
			  << "\tVersion: " << platform.version() << "\n"
			  << "\tExtensions: \n";
	for (auto&& ext : platform.getExtensions()) {
		std::cout << "\t\t" << ext << '\n';
	}

	auto devices = platform.getDevices(cl::DeviceType::all);
	std::cout << "Count devices: " << devices.size() << '\n';
	auto device = devices[0];
	std::cout << "Devices ...\n"
			  << "\tName: " << device.name() << '\n'
			  << "\tAvailable: " << device.available() << '\n'
			  << "\tCompiler available: " << device.compilerAvailable() << '\n'
			  << "\tAddress bits: " << device.addressBits() << '\n'
			  << "\tAvailable: " << ((device.available()) ? "true" : "false") << '\n';
	for (auto&& size : device.maxWorkItemSizes()) {
		std::cout << "\t\t" << size << "\n";
	}

	auto usr_data = int{5};
	auto properties = cl::ContextProperties().setPlatform(platform);
	//auto context_id = cl::Context(properties, devices);
	auto context = cl::Context(
		properties,
		devices,
		[](std::string const& error_info, std::vector<uint8_t> const& private_info, int user_data) {
			std::cout << "error_info: " << error_info << '\n'
					  << "private_info: " << '\n';
			for (auto&& t : private_info) std::cout << '\t' << t << '\n';
			std::cout << "user_data: " << user_data << '\n';		
		},
		usr_data
	);

	std::cout << "Context created successfully!\n";

	std::ignore = context;
	//std::cout << "Context information ...\n";
	//std::cout << "\tReference Count = " << context.referenceCount() << '\n';

	/*
	//auto usr_data = int{5};
	std::cout << "Properties:\n";
	for (auto&& p : properties.get()) {
		std::cout << p << '\n';
	}
	std::cout << "CL_CONTEXT_PLATFORM = " << CL_CONTEXT_PLATFORM << '\n';
	std::cout << "static_cast<size_t>(platform) = " << ((size_t)(platform.id())) << '\n';
	*/

	/*
	auto context = cl::Context(
		properties,
		devices,
		[](std::string const& error_info, std::vector<uint8_t> const& private_info, int user_data) {
			std::cout << "error_info: " << error_info << '\n'
					  << "private_info: " << '\n';
			for (auto&& t : private_info) std::cout << '\t' << t << '\n';
			std::cout << "user_data: " << user_data << '\n';		
		},
		usr_data
	);
	*/
	//auto context = cl::Context(properties, devices);
	/*
	try {
		auto error = cl_int{CL_SUCCESS};
		auto context_id = clCreateContext(
			properties.get().data(),
			devices.size(),
			reinterpret_cast<const cl_device_id*>(devices.data()),
			nullptr,
			nullptr,
			& error
		);
		cl::error::handle<cl::Exception>(error);
		std::ignore = context_id;
	} catch (cl::Exception const& e) {
		std::cout << "cl::Exception caught!\n"
				  << "\twhat() = " << e.what()
				  << "\n\tcode() = " << static_cast<std::underlying_type<cl::ErrorCode>::type>(e.code()) << '\n';
	}
	*/

	//auto tmp = context;
	//std::ignore = tmp;

	//std::ignore = context_id;

	return 0;
}
