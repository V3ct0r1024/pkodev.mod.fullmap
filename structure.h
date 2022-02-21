#pragma once

namespace pkodev
{
	// Map data structure
	struct map
	{
		// Map name
		std::string name;

		// Map size
		unsigned int width;
		unsigned int height;

		// Constructor
		map() :
			name(""),
			width(0),
			height(0)
		{

		}

		// Constructor
		map(const std::string name_,
			unsigned int width_, unsigned int height_) :
			name(name_),
			width(width_),
			height(height_)
		{

		}
	};
}