#pragma once
#include "app_support.h"
#include "geometry/bezier.h"
class Canvas : public QWidget {
  public:
    std::vector<QPointF> points;
    std::function<void()> changed;
    std::function<void(QPainter &)> overlay;
    int limit = -1, drag = -1;
    QColor pointColor = QColor("#2563eb");
    int answer = 2;
    explicit Canvas(QWidget *parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
    }

  protected:
    void mousePressEvent(QMouseEvent *e) override {
        if (e->button() != Qt::LeftButton)
            return;
        auto p = mousePosition(e);
        drag = -1;
        for (size_t i = 0; i < points.size(); ++i)
            if (QLineF(p, points[i]).length() < 12) {
                drag = int(i);
                return;
            }
        if (limit < 0 || int(points.size()) < limit) {
            points.push_back(p);
            if (changed)
                changed();
            update();
        }
    }
    void mouseMoveEvent(QMouseEvent *e) override {
        if (drag < 0 || !(e->buttons() & Qt::LeftButton))
            return;
        auto p = mousePosition(e);
        points[size_t(drag)] = {std::clamp(p.x(), 0., double(width())),
                                std::clamp(p.y(), 0., double(height()))};
        if (changed)
            changed();
        update();
    }
    void mouseReleaseEvent(QMouseEvent *) override {
        drag = -1;
    }
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        canvasBackground(painter, rect());
        if (overlay)
            overlay(painter);
        for (size_t i = 0; i < points.size(); ++i) {
            QColor color = pointColor;
            if (limit == 3 && i == 2 && answer != 2)
                color = answer > 0   ? QColor("#16a34a")
                        : answer < 0 ? QColor("#dc2626")
                                     : QColor("#eab308");
            painter.setPen(QPen(color.darker(), 1));
            painter.setBrush(color);
            painter.drawEllipse(points[i], 6, 6);
            painter.setPen(QColor("#0f172a"));
            QString label = limit == 3   ? QString(i == 0   ? "A"
                                                   : i == 1 ? "B"
                                                            : "P")
                            : limit == 4 ? QString(QChar('A' + int(i)))
                                         : QString("P%1").arg(qulonglong(i + 1));
            painter.drawText(points[i] + QPointF(9, -9), label);
        }
    }
};

class TaskWindow : public QMainWindow {
  public:
    Canvas *canvas = new Canvas;
    QCheckBox *online = new QCheckBox("Онлайн-пересчёт"),
              *quadratic = new QCheckBox("Квадратичная — зелёная"),
              *cubic = new QCheckBox("Кубическая — синяя"),
              *general = new QCheckBox("Общая степень n — фиолетовая"),
              *construction = new QCheckBox("Построение де Кастельжо");
    QSlider *parameter = new QSlider(Qt::Horizontal);
    QLabel *parameterText = new QLabel;
    QTextBrowser *result = new QTextBrowser;
    std::vector<geometry::Point> saved;
    TaskWindow() {
        QVBoxLayout *controls;
        setupWindow(*this, "Кривые Безье", canvas, controls);
        auto *hint =
            new QLabel("Клики добавляют контрольные точки.\nТочки можно перетаскивать.\n\nКривые "
                       "имеют общие начало P1 и конец Pn. Квадратичная: P1, P2, Pn. Кубическая: "
                       "P1, P2, P(n−1), Pn. Общая: все точки.");
        hint->setWordWrap(true);
        controls->addWidget(hint);
        for (auto *box : {online, quadratic, cubic, general, construction}) {
            box->setChecked(true);
            controls->addWidget(box);
            connect(box, &QCheckBox::toggled, this, [this]() { canvas->update(); });
        }
        connect(online, &QCheckBox::toggled, this, [this](bool enabled) {
            if (enabled)
                calculate();
        });
        parameter->setRange(0, 1000);
        parameter->setValue(450);
        controls->addWidget(parameterText);
        controls->addWidget(parameter);
        connect(parameter, &QSlider::valueChanged, this, [this]() { updateParameter(); });
        addButton(controls, "Рассчитать", [this]() { calculate(); });
        addButton(controls, "Пример пяти точек", [this]() {
            canvas->points = {{80, 360}, {160, 70}, {330, 110}, {470, 430}, {630, 280}};
            calculate();
        });
        addButton(controls, "Очистить", [this]() {
            canvas->points.clear();
            saved.clear();
            result->setPlainText("Добавьте минимум четыре точки для сравнения степеней 2 и 3.");
            canvas->update();
        });
        controls->addWidget(result, 1);
        canvas->changed = [this]() {
            if (online->isChecked())
                calculate();
            else {
                saved.clear();
                result->setPlainText("Данные изменены. Нажмите «Рассчитать».");
                canvas->update();
            }
        };
        canvas->overlay = [this](QPainter &p) { draw(p); };
        updateParameter();
        canvas->points = {{80, 360}, {160, 70}, {330, 110}, {470, 430}, {630, 280}};
        calculate();
    }
    void updateParameter() {
        parameterText->setText(QString("t = %1").arg(parameter->value() / 1000., 0, 'f', 3));
        canvas->update();
    }
    void calculate() {
        saved.clear();
        for (auto p : canvas->points)
            saved.push_back(geometry_qt::toGeometryPoint(p));
        result->setPlainText(QString("Контрольных точек: %1\nОбщая степень: %2\n\n%3")
                                 .arg(qulonglong(saved.size()))
                                 .arg(saved.empty() ? 0 : qulonglong(saved.size() - 1))
                                 .arg(saved.size() < 4
                                          ? "Для одновременных квадратичной и кубической кривых "
                                            "добавьте хотя бы 4 точки."
                                          : "Сравните зелёную, синюю и фиолетовую кривые. Ползунок "
                                            "t показывает промежуточные линейные интерполяции."));
        canvas->update();
    }
    void drawCurve(QPainter &p, const std::vector<geometry::Point> &control, QColor color,
                   Qt::PenStyle style = Qt::SolidLine) {
        auto points = geometry::sampleBezier(control);
        QPolygonF path;
        for (auto v : points)
            path << geometry_qt::toQPointF(v);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(color, 3, style));
        p.drawPolyline(path);
    }
    void draw(QPainter &p) {
        if (!canvas->points.empty()) {
            QPolygonF polygon;
            for (auto v : canvas->points)
                polygon << v;
            p.setPen(QPen(QColor("#94a3b8"), 1.5, Qt::DashLine));
            p.setBrush(Qt::NoBrush);
            p.drawPolyline(polygon);
        }
        if (saved.size() < 2)
            return;
        if (general->isChecked())
            drawCurve(p, saved, QColor("#7c3aed"), Qt::DashLine);
        if (cubic->isChecked() && saved.size() >= 4)
            drawCurve(p, {saved[0], saved[1], saved[saved.size() - 2], saved.back()},
                      QColor("#2563eb"));
        if (quadratic->isChecked() && saved.size() >= 3)
            drawCurve(p, {saved[0], saved[1], saved.back()}, QColor("#16a34a"));
        if (construction->isChecked()) {
            auto levels = geometry::deCasteljauLevels(saved, parameter->value() / 1000.);
            for (size_t row = 1; row < levels.size(); ++row) {
                QPolygonF line;
                for (auto v : levels[row])
                    line << geometry_qt::toQPointF(v);
                p.setPen(QPen(QColor::fromHsv(int(25 + row * 38) % 360, 190, 210), 1.5));
                p.setBrush(Qt::NoBrush);
                p.drawPolyline(line);
                for (auto v : line) {
                    p.setBrush(QColor("#f59e0b"));
                    p.drawEllipse(v, 3, 3);
                }
            }
            auto point = geometry_qt::toQPointF(levels.back()[0]);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor("#e11d48"));
            p.drawEllipse(point, 7, 7);
        }
    }
    void smokeTest() {
        canvas->points.clear();
        saved.clear();
        for (auto p : {QPointF(80, 360), QPointF(160, 70), QPointF(330, 110), QPointF(470, 430),
                       QPointF(630, 280)})
            simulateClick(canvas, p);
        ensure(saved.size() == 5, "Bezier click input");
        parameter->setValue(600);
        simulateDrag(canvas, {160, 70}, {180, 90});
        ensure(saved[1].x == 180 && saved[1].y == -90, "Bezier online dragging");
        online->setChecked(false);
        simulateDrag(canvas, {180, 90}, {180, 110});
        ensure(saved.empty(), "Bezier stale result hidden");
        calculate();
        ensure(saved[1].y == -110, "Bezier manual calculation");
    }
};
