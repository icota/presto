#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "LightningModel.h"


int main(int argc, char *argv[])
{


    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // This is loop dependent
    LightningModel* lightningModel = new LightningModel;
    // doesn't show up
    qDebug() << "wtf";

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("peersModel", lightningModel->peersModel());
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;


    return app.exec();
}
