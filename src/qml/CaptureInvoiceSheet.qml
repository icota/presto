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

    ColumnLayout {
        id: columnLayout
        // Make the sheet wider like this
        //implicitWidth: parent.width * 0.75

        // I have no idea what I'm doing
        implicitHeight: qrScannerViewfinder.height + 30

        QQC2.Label {
            color: Kirigami.Theme.textColor
            id: scanLabel
            Layout.alignment: Qt.AlignCenter
            width: parent.width
            text: qsTr("Scan Invoice")
            font.pixelSize: 16
        }

        QRScannerViewfinder {
            id: qrScannerViewfinder
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
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
            placeholderText: qsTr("Or Paste Here")
            background: Rectangle {
                anchors.fill: parent
            }
            onTextChanged: {
                checkIfValidBolt11(text);
            }
        }
    }

    function checkIfValidBolt11(text) {
        payInvoiceSheet.bolt11 = text
        paymentsModel.decodePayment(text);
    }
}

