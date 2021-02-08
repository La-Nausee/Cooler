#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QEventLoop>

#include "scanner.h"
#include "cooler.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
//    app.setApplicationName(QString("Cooler"));

    QEventLoop loop;
    QQmlApplicationEngine engine;
    Scanner scanner;
    Cooler cooler;

    engine.rootContext()->setContextProperty("scanner", &scanner);
    engine.rootContext()->setContextProperty("cooller", &cooler);

    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));

    engine.load(QUrl(QStringLiteral("qrc:/assets/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
