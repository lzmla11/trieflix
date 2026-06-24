#include "../include/Utils.h"
#include <cctype>
#include <sstream>

using namespace std; 

namespace Utils {

    	string toLowerCase(const string& text) {
		string result;

        	for (char c : text) {
        		result += std::tolower(static_cast<unsigned char>(c));
        	}

        	return result;
	}
	
    	string removePunctuation(const string& text) {
    		string result;

    		for (char c : text) {

        		if (isalnum(static_cast<unsigned char>(c)) ||
            	    	isspace(static_cast<unsigned char>(c))) {

            			result += c;

        		} else {

            			result += ' ';
        		}
    		}
    		return result;
    	}

	string normalizeSpaces(const string& text) {

		string result;

	    	bool previousWasSpace = false;

	    	for (char c : text) {

			if (isspace(static_cast<unsigned char>(c))) {

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

	string cleanText(const string& text) {

    		string cleaned = toLowerCase(text);

    		cleaned = removePunctuation(cleaned);

    		cleaned = normalizeSpaces(cleaned);

    		return cleaned;
	}
    
    vector<string> tokenize(const string& text) {
        vector<string> tokens;  
        
        istringstream ss(text); 

        string word; 

        while (ss >> word) {
            if (word.size() >= 3) {
                tokens.push_back(word); 
            }
        }

        return tokens; 
    }
}


