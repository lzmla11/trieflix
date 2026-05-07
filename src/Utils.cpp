#include "../include/Utils.h"
#include <cctype>

namespace Utils {

    	std::string toLowerCase(const std::string& text) {
		std::string result;

        	for (char c : text) {
        		result += std::tolower(static_cast<unsigned char>(c));
        	}

        	return result;
	}
	
    	std::string removePunctuation(const std::string& text) {
    		std::string result;

    		for (char c : text) {

        		if (std::isalnum(static_cast<unsigned char>(c)) ||
            	    	std::isspace(static_cast<unsigned char>(c))) {

            			result += c;

        		} else {

            			result += ' ';
        		}
    		}
    		return result;
    	}

	std::string normalizeSpaces(const std::string& text) {

		std::string result;

	    	bool previousWasSpace = false;

	    	for (char c : text) {

			if (std::isspace(static_cast<unsigned char>(c))) {

		    	if (!previousWasSpace) {
				result += ' ';
				previousWasSpace = true;
		    	}

			} else {

		    		result += c;
		    		previousWasSpace = false;
			}
	    	}

	    	// Eliminar espacio al inicio
	    	if (!result.empty() && result.front() == ' ') {
			result.erase(result.begin());
	    	}

	    	// Eliminar espacio al final
	    	if (!result.empty() && result.back() == ' ') {
			result.pop_back();
	    	}

	    	return result;
	}

	std::string cleanText(const std::string& text) {

    		std::string cleaned = toLowerCase(text);

    		cleaned = removePunctuation(cleaned);

    		cleaned = normalizeSpaces(cleaned);

    		return cleaned;
	}

}


