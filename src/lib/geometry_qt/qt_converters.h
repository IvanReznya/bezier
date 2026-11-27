#pragma once

// Мостик между Qt и нашей библиотекой: перевод QPointF <-> geometry::Point.
// Алгоритмы о Qt не знают, окна не знают о geometry::Point — встречаются они только здесь.

#include <QPointF>

#include "geometry/point.h"

namespace geometry_qt {

geometry::Point toGeometryPoint(const QPointF &point);
QPointF toQPointF(const geometry::Point &point);

} // namespace geometry_qt
