#pragma once

#include <vector>

#include "primitives.h"

class SimplePoint {
	glm::vec3 position{ 0.0f,0.0f,0.0f };
	glm::vec4 color{ NO_COLOR };
	float size{ 0.0f };
	
	glm::vec3* posptr{ nullptr };

	void setPosPtr(glm::vec3* PosPtr) {
		posptr = PosPtr;
	}
	void update() {
		position = *posptr;
	}
};

class SimplePointFac {
	std::vector<SimplePoint> objects;
	SimplePoint* addSimplePoint() {
		objects.emplace_back(SimplePoint{});
	}
	
};
