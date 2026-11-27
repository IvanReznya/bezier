#pragma once

// Библиотека geometry: только стандартный C++, никакого Qt.

namespace geometry {

struct Point {
    double x = 0.0;
    double y = 0.0;
};

// Расстояние между двумя точками.
double distance(const Point &a, const Point &b);

} // namespace geometry
