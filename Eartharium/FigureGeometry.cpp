

#include "FigureGeometry.h"

//glm::vec3 NormalSphere::getLoc3D(const LLH loc, const float gpuRadius, const bool rad) {
//    double lat = loc.lat, lon = loc.lon;
//    if (!rad) {
//        lat *= deg2rad;
//        lon *= deg2rad;
//    }
//    double h = (double)gpuRadius + loc.dst; // Height above center in world coordinates
//    float w = (float)(cos(lat) * h);
//    float x = (float)(cos(lon) * w);
//    float y = (float)(sin(lon) * w);
//    float z = (float)(sin(lat) * h);
//    return glm::vec3(x, y, z);
//}