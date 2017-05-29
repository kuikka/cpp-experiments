#include <iostream>
#include <vector>
#include <string>
#include <cassert>

template<typename T>
std::vector< std::string > tokenize_string(
		const std::string& s,
		const T& delimeter)

{
	std::vector< std::string > ret;

	std::string::size_type pos = 0;

	while( pos != std::string::npos )
	{
		// Find beginning of word (ignore leading delimeters)
		auto bi = s.find_first_not_of( delimeter, pos );
		if ( bi != std::string::npos )
		{
			// Find the end of this word
			auto ei = s.find_first_of( delimeter, bi );
			if ( ei == std::string::npos )
			{
				// Last word
				ret.push_back( s.substr( bi ) );
			}
			else
			{
				ret.push_back( s.substr( bi, ei - bi ) );
			}
			pos = ei;
		}
		else
			pos = bi; // end
	}
	return ret;
}

int main(int argc, char *argv[])
{
	std::vector< std::string > tokens = tokenize_string(
			"../a/./b/../../c/",
			"/"
			);

	assert(tokens.size() == 7);
	assert(tokens[0] == "..");
	assert(tokens[1] == "a");
	assert(tokens[2] == ".");
	assert(tokens[3] == "b");
	assert(tokens[4] == "..");
	assert(tokens[5] == "..");
	assert(tokens[6] == "c");

	tokens = tokenize_string(
			"//",
			'/'
			);

	assert(tokens.size() == 0);

	return 0;
}
