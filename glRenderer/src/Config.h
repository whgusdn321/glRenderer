#pragma once

#include <string>
#include <vector>

struct Config
{
	// configPanel -> write to this.
	// other draw processes -> read this.
	std::string modelName = "rodin";
	std::string skyboxName = "sky";

	unsigned int primitiveCnt;

	bool frustumCulling = true;
	bool highlightBoundary = false;
	bool loadSkybox = false;

};
