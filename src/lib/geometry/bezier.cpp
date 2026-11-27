#include "geometry/bezier.h"
#include <cmath>
#include <stdexcept>
namespace geometry {
std::vector<std::vector<Point>> deCasteljauLevels(const std::vector<Point> &control, double t) {
    if (control.empty() || !std::isfinite(t) || t < 0 || t > 1)
        throw std::invalid_argument("Нужны точки и t от 0 до 1");
    for (auto p : control)
        if (!std::isfinite(p.x) || !std::isfinite(p.y))
            throw std::invalid_argument("Неконечная координата");
    std::vector<std::vector<Point>> levels{control};
    while (levels.back().size() > 1) {
        const auto &row = levels.back();
        std::vector<Point> next;
        for (size_t i = 0; i + 1 < row.size(); ++i)
            next.push_back(
                {(1 - t) * row[i].x + t * row[i + 1].x, (1 - t) * row[i].y + t * row[i + 1].y});
        levels.push_back(std::move(next));
    }
    return levels;
}
Point bezierPoint(const std::vector<Point> &control, double t) {
    return deCasteljauLevels(control, t).back().front();
}
std::vector<Point> sampleBezier(const std::vector<Point> &control, int segments) {
    if (segments < 1)
        throw std::invalid_argument("Число сегментов должно быть положительным");
    std::vector<Point> result;
    result.reserve(size_t(segments) + 1);
    for (int i = 0; i <= segments; ++i)
        result.push_back(bezierPoint(control, double(i) / segments));
    return result;
}
} // namespace geometry
