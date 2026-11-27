#pragma once
#include "geometry/point.h"
#include <vector>
namespace geometry {
std::vector<std::vector<Point>> deCasteljauLevels(const std::vector<Point> &control, double t);
Point bezierPoint(const std::vector<Point> &control, double t);
std::vector<Point> sampleBezier(const std::vector<Point> &control, int segments = 300);
} // namespace geometry
