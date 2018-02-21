QT += quick multimedia # added multimedia for the camera functionality
CONFIG += c++11

android {
message("Building for Android...")
QT += androidextras
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES += qml.qrc \
    kirigami-icons.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH +=

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

include(3rdparty/kirigami/kirigami.pri)
include(3rdparty/qzxing/src/QZXing.pri)

QJSONRPC_HEADERS += \
    3rdparty/qjsonrpc/src/qjsonrpcabstractserver.h \
    3rdparty/qjsonrpc/src/qjsonrpcabstractserver_p.h \
    3rdparty/qjsonrpc/src/qjsonrpcglobal.h \
    3rdparty/qjsonrpc/src/qjsonrpcmessage.h \
    3rdparty/qjsonrpc/src/qjsonrpcmetatype.h \
    3rdparty/qjsonrpc/src/qjsonrpcservice.h \
    3rdparty/qjsonrpc/src/qjsonrpcservice_p.h \
    3rdparty/qjsonrpc/src/qjsonrpcserviceprovider.h \
    3rdparty/qjsonrpc/src/qjsonrpcservicereply.h \
    3rdparty/qjsonrpc/src/qjsonrpcservicereply_p.h \
    3rdparty/qjsonrpc/src/qjsonrpcsocket.h \
    3rdparty/qjsonrpc/src/qjsonrpcsocket_p.h

QJSONRPC_SOURCES += \
    3rdparty/qjsonrpc/src/qjsonrpcabstractserver.cpp \
    3rdparty/qjsonrpc/src/qjsonrpcmessage.cpp \
    3rdparty/qjsonrpc/src/qjsonrpcservice.cpp \
    3rdparty/qjsonrpc/src/qjsonrpcserviceprovider.cpp \
    3rdparty/qjsonrpc/src/qjsonrpcservicereply.cpp \
    3rdparty/qjsonrpc/src/qjsonrpcsocket.cpp \

HEADERS += \
    $${QJSONRPC_HEADERS} \
    src/LightningModel.h \
    src/PaymentsModel.h \
    src/PeersModel.h \
    src/WalletModel.h \
    src/InvoicesModel.h \
    src/QClipboardProxy.h

SOURCES += \
    $${QJSONRPC_SOURCES} \
    src/main.cpp \
    src/LightningModel.cpp \
    src/PaymentsModel.cpp \
    src/PeersModel.cpp \
    src/WalletModel.cpp \
    src/InvoicesModel.cpp \
    src/QClipboardProxy.cpp

DISTFILES += \
    src/qml/qmldir \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
