#include "context_properties.hpp"

namespace cl {
	class Platform;

	ContextProperties::ContextProperties(cl_context_properties * properties) {
		for (auto i = 0; properties[i] != 0; i += 2) {
			m_options[properties[i]] = properties[i+1];
		}
	}

	ContextProperties& ContextProperties::setPlatform(Platform platform) {
		m_options[CL_CONTEXT_PLATFORM] = reinterpret_cast<cl_context_properties>(platform.id());
		return * this;
	}

	std::vector<cl_context_properties> ContextProperties::get() const {
		auto list = std::vector<cl_context_properties>{};
		for (auto&& option : m_options) {
			list.push_back(option.first);
			list.push_back(option.second);
		}
		list.push_back(static_cast<cl_context_properties>(0));
		return list;
	}
}
