#pragma once
#include "geometry_qt/qt_converters.h"
#include <QApplication>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTextBrowser>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>
inline void ensure(bool condition, const char *message) {
    if (!condition)
        throw std::runtime_error(message);
}
inline QPointF mousePosition(QMouseEvent *event) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
inline void canvasBackground(QPainter &p, const QRect &rect) {
    p.fillRect(rect, QColor("#f8fafc"));
    p.setPen(QColor("#e2e8f0"));
    for (int x = 0; x < rect.width(); x += 40)
        p.drawLine(x, 0, x, rect.height());
    for (int y = 0; y < rect.height(); y += 40)
        p.drawLine(0, y, rect.width(), y);
}
inline QPushButton *addButton(QVBoxLayout *layout, const QString &text,
                              const std::function<void()> &action) {
    auto *button = new QPushButton(text);
    layout->addWidget(button);
    QObject::connect(button, &QPushButton::clicked, button, action);
    return button;
}
inline void simulateClick(QWidget *target, QPointF point) {
    QMouseEvent press(QEvent::MouseButtonPress, point, point, point, Qt::LeftButton, Qt::LeftButton,
                      Qt::NoModifier);
    QApplication::sendEvent(target, &press);
    QMouseEvent release(QEvent::MouseButtonRelease, point, point, point, Qt::LeftButton,
                        Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(target, &release);
}
inline void simulateDrag(QWidget *target, QPointF start, QPointF end) {
    QMouseEvent press(QEvent::MouseButtonPress, start, start, start, Qt::LeftButton, Qt::LeftButton,
                      Qt::NoModifier);
    QApplication::sendEvent(target, &press);
    QMouseEvent move(QEvent::MouseMove, end, end, end, Qt::NoButton, Qt::LeftButton,
                     Qt::NoModifier);
    QApplication::sendEvent(target, &move);
    QMouseEvent release(QEvent::MouseButtonRelease, end, end, end, Qt::LeftButton, Qt::NoButton,
                        Qt::NoModifier);
    QApplication::sendEvent(target, &release);
}
inline void setupWindow(QMainWindow &window, const QString &title, QWidget *canvas,
                        QVBoxLayout *&controls) {
    window.setWindowTitle(title);
    auto *central = new QWidget;
    auto *layout = new QHBoxLayout(central);
    canvas->setMinimumSize(650, 480);
    layout->addWidget(canvas, 1);
    auto *panel = new QWidget;
    panel->setMinimumWidth(290);
    panel->setMaximumWidth(350);
    controls = new QVBoxLayout(panel);
    auto *heading = new QLabel(title);
    heading->setWordWrap(true);
    heading->setStyleSheet("font-size:18px;font-weight:600;color:#0f172a;margin:6px 0 12px 0;");
    controls->addWidget(heading);
    layout->addWidget(panel);
    window.setCentralWidget(central);
}
