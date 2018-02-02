QT += quick multimedia # added multimedia for the camera functionality
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    LightningModel.cpp \
    qjsonrpc/qjsonrpcabstractserver.cpp \
    qjsonrpc/qjsonrpcmessage.cpp \
    qjsonrpc/qjsonrpcservice.cpp \
    qjsonrpc/qjsonrpcserviceprovider.cpp \
    qjsonrpc/qjsonrpcservicereply.cpp \
    qjsonrpc/qjsonrpcsocket.cpp \
    PaymentsModel.cpp \
    PeersModel.cpp \
    WalletModel.cpp \
    InvoicesModel.cpp

RESOURCES += qml.qrc \
    kirigami-icons.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += "/home/igor/Code/build-kirigami-Desktop_Qt_5_9_0_GCC_64bit-Debug"

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

include(3rdparty/kirigami/kirigami.pri)
include(3rdparty/qzxing/src/QZXing.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    LightningModel.h \
    qjsonrpc/qjsonrpcabstractserver.h \
    qjsonrpc/qjsonrpcabstractserver_p.h \
    qjsonrpc/qjsonrpcglobal.h \
    qjsonrpc/qjsonrpcmessage.h \
    qjsonrpc/qjsonrpcmetatype.h \
    qjsonrpc/qjsonrpcservice.h \
    qjsonrpc/qjsonrpcservice_p.h \
    qjsonrpc/qjsonrpcserviceprovider.h \
    qjsonrpc/qjsonrpcservicereply.h \
    qjsonrpc/qjsonrpcservicereply_p.h \
    qjsonrpc/qjsonrpcsocket.h \
    qjsonrpc/qjsonrpcsocket_p.h \
    PaymentsModel.h \
    PeersModel.h \
    WalletModel.h \
    InvoicesModel.h
