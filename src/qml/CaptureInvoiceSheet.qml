import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

import QtMultimedia 5.8
import QZXing 2.3

Kirigami.OverlaySheet {
    onSheetOpenChanged: {
        if (sheetOpen) {
            qrScannerViewfinder.camera.start()
        }
        else {
            qrScannerViewfinder.camera.stop()
        }
    }

    header:
        OverlaySheetHeader {
        text: qsTr("Scan QR Code")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }


    ColumnLayout {
        id: columnLayout

        QRScannerViewfinder {
            id: qrScannerViewfinder
            width: parent.width
            height: width
            Layout.alignment: Qt.AlignCenter
            zxingFilter.decoder.onTagFound: {
                checkIfValidBolt11(tag)
            }
        }

        QQC2.TextArea {
            anchors.top: qrScannerViewfinder.bottom
            anchors.topMargin: 10
            id: pasteTextArea
            font: fixedFont
            selectByMouse: true
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            placeholderText: qsTr("Or Paste Invoice Here")
            onTextChanged: {
                checkIfValidBolt11(text);
            }
        }
    }

    function checkIfValidBolt11(text) {
        if (text.startsWith("lightning:")) {
            text = text.slice(10)
        }

        payInvoiceSheet.bolt11 = text
        paymentsModel.decodePayment(text);
    }
}

