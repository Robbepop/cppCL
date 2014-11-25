#ifndef CPPCL_CONTEXT_PROPERTIES_HEADER
#define CPPCL_CONTEXT_PROPERTIES_HEADER

#include "wrapper.hpp"
#include "platform.hpp"

#include <unordered_map>
#include <vector>

namespace cl {
	class Platform;

	class ContextProperties final {
	public:
		ContextProperties() = default;

		ContextProperties(cl_context_properties * properties);

		ContextProperties& setPlatform(Platform platform);

		std::vector<cl_context_properties> get() const;

	private:
		std::unordered_map<cl_context_properties, cl_context_properties> m_options;
	};
}

#endif
