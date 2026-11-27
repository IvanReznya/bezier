#include "app.h"
#include <QApplication>
#include <iostream>
int main(int argc, char **argv) {
    bool smoke = argc > 1 && QString::fromLocal8Bit(argv[1]) == "--smoke-test";
    if (smoke)
        qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);
    try {
        TaskWindow window;
        window.resize(1180, 800);
        window.show();
        if (smoke) {
            application.processEvents();
            window.smokeTest();
            application.processEvents();
            if (!window.grab().save("preview.png"))
                throw std::runtime_error("Cannot save preview");
            std::cout << "GUI smoke test passed\n";
            return 0;
        }
        return application.exec();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
