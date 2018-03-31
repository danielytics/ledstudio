#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>

#include "LedStripSimulator.h"
#include "LedStripSimulatorDriver.h"

#include <QDebug>

#include "Random.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QFontDatabase::addApplicationFont(":/QMLFontAwesome/font.ttf");
    QQmlApplicationEngine engine;

    LedStripSimulator::registerTypes();
    LedStripSimulatorDriver::registerTypes();

    const QUrl mainQml(QStringLiteral("qrc:/main.qml"));

    // Catch the objectCreated signal, so that we can determine if the root component was loaded
    // successfully. If not, then the object created from it will be null. The root component may
    // get loaded asynchronously.
    const QMetaObject::Connection connection = QObject::connect(
                &engine, &QQmlApplicationEngine::objectCreated,
                &app, [&](QObject *object, const QUrl &url) {
            if (url != mainQml)
            return;

            if (!object)
            app.exit(-1);
            else
            QObject::disconnect(connection);
}, Qt::QueuedConnection);

    engine.load(mainQml);

    return app.exec();
}
