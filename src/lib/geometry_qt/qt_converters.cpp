#include "geometry_qt/qt_converters.h"

namespace geometry_qt {

geometry::Point toGeometryPoint(const QPointF &point) {
    return {point.x(), -point.y()};
}

QPointF toQPointF(const geometry::Point &point) {
    return {point.x, -point.y};
}

} // namespace geometry_qt
