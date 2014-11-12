#include "common.hpp"

#include <vector>
#include <string>
#include <algorithm>

namespace cl {
	namespace common {
		std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty) {
			auto result = std::vector<std::string>();
			if (delim.empty()) {
				result.push_back(s);
			} else {
				std::string::const_iterator substart = s.begin(), subend;
				while (true) {
					subend = search(substart, s.end(), delim.begin(), delim.end());
					std::string temp(substart, subend);
					if (keep_empty || !temp.empty()) {
						result.push_back(temp);
					}
					if (subend == s.end()) {
						break;
					}
					substart = subend + delim.size();
				}
			}
			return result;
		}
	}
}
