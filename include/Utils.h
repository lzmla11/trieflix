#pragma once

#include <string>
#include <vector>

using namespace std;

namespace Utils {
    string toLowerCase(const string& text);
    string removePunctuation(const string& text);
    string normalizeSpaces(const string& text);
    string cleanText(const string& text);
}