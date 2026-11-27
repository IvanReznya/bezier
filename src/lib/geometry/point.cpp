#include "geometry/point.h"

#include <cmath>

namespace geometry {

double distance(const Point &a, const Point &b) {
    return std::hypot(b.x - a.x, b.y - a.y);
}

} // namespace geometry
