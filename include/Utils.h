#pragma once

#include <string>
#include <vector>

namespace Utils {

    std::string toLowerCase(const std::string& text);

    std::string removePunctuation(const std::string& text);

    std::string normalizeSpaces(const std::string& text);

    std::string cleanText(const std::string& text);

}
