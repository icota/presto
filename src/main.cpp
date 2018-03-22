#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFont>
#include <QFontDatabase>

#include "LightningModel.h"
#include "QClipboardProxy.h"
#include "AutoPilot.h"

#ifdef Q_OS_ANDROID
#include "AndroidNfcHelper.h"
#else
#include "NfcHelper.h"
#endif

#include "./3rdparty/kirigami/src/kirigamiplugin.h"
#include "./3rdparty/qzxing/src/QZXing.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    app.setOrganizationName("Codex Apertus");
    app.setOrganizationDomain("codexapertus.com");
    app.setApplicationName("Presto!");

    LightningModel* lightningModel = new LightningModel;
    AutoPilot* autoPilot = new AutoPilot;

#ifdef Q_OS_ANDROID
    AndroidNfcHelper* nfcHelper = new AndroidNfcHelper;
#else
    NfcHelper* nfcHelper = new NfcHelper;
#endif

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("lightningModel", lightningModel);

    engine.rootContext()->setContextProperty("peersModel", lightningModel->peersModel());
    engine.rootContext()->setContextProperty("paymentsModel", lightningModel->paymentsModel());
    engine.rootContext()->setContextProperty("walletModel", lightningModel->walletModel());
    engine.rootContext()->setContextProperty("invoicesModel", lightningModel->invoicesModel());
    engine.rootContext()->setContextProperty("nfcHelper", nfcHelper);
    engine.rootContext()->setContextProperty("autoPilot", autoPilot);
    qmlRegisterUncreatableMetaObject(
      InvoiceTypes::staticMetaObject,
      "Lightning.Invoice",
      1, 0,
      "Invoice",
      "Error: only enums"
    );


    KirigamiPlugin::getInstance().registerTypes();
    QZXing::registerQMLTypes();
    QZXing::registerQMLImageProvider(engine);

    engine.rootContext()->setContextProperty("clipboard",
                                             new QClipboardProxy(QGuiApplication::clipboard()));

    // Platform independent way of getting a fixed width font to QML
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    engine.rootContext()->setContextProperty("fixedFont", fixedFont);

    engine.load(QUrl(QLatin1String("qrc:/src/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
