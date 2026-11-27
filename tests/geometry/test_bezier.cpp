#include "check.h"
#include "geometry/bezier.h"
#include <cmath>
using namespace geometry;
int main() {
    std::vector<Point> q{{0, 0}, {1, 2}, {2, 0}}, c{{0, 0}, {0, 2}, {2, 2}, {2, 0}};
    auto mid = bezierPoint(q, .5);
    check(mid.x == 1 && mid.y == 1, "quadratic midpoint");
    mid = bezierPoint(c, .5);
    check(mid.x == 1 && mid.y == 1.5, "cubic midpoint");
    check(bezierPoint(q, 0).x == 0 && bezierPoint(q, 1).x == 2, "endpoints");
    auto levels = deCasteljauLevels(c, .3);
    check(levels.size() == 4, "all construction levels");
    for (size_t i = 0; i < levels.size(); ++i)
        check(levels[i].size() == 4 - i, "construction row lengths");
    auto curve = sampleBezier(c, 100);
    check(curve.size() == 101, "sample count");
    for (auto p : curve)
        check(p.x >= 0 && p.x <= 2 && p.y >= 0 && p.y <= 2, "convex hull bound");
    auto shifted = c;
    for (auto &p : shifted) {
        p.x = 3 * p.x + 100;
        p.y = 3 * p.y - 50;
    }
    auto original = bezierPoint(c, .37), transformed = bezierPoint(shifted, .37);
    check(std::abs(transformed.x - (3 * original.x + 100)) < 1e-12 &&
              std::abs(transformed.y - (3 * original.y - 50)) < 1e-12,
          "affine invariance");
    check(bezierPoint({{7, 9}}, .6).y == 9, "single point");
    for (double t : {-0.1, 1.1}) {
        bool caught = false;
        try {
            bezierPoint(c, t);
        } catch (const std::invalid_argument &) {
            caught = true;
        }
        check(caught, "parameter validation");
    }
    bool caught = false;
    try {
        bezierPoint({}, .5);
    } catch (const std::invalid_argument &) {
        caught = true;
    }
    check(caught, "empty input");
}
