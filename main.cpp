#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "LightningModel.h"

#include "./3rdparty/kirigami/src/kirigamiplugin.h"
#include "./3rdparty/qzxing/src/QZXing.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // This is loop dependent
    LightningModel* lightningModel = new LightningModel;
    // qdebug doesn't show up
    qDebug() << "wtf";

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("peersModel", lightningModel->peersModel());
    engine.rootContext()->setContextProperty("paymentsModel", lightningModel->paymentsModel());
    engine.rootContext()->setContextProperty("walletModel", lightningModel->walletModel());
    engine.rootContext()->setContextProperty("invoicesModel", lightningModel->invoicesModel());

    KirigamiPlugin::getInstance().registerTypes();
    QZXing::registerQMLTypes();
    QZXing::registerQMLImageProvider(engine);

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
