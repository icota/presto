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
        QQC2.Label {
            color: Kirigami.Theme.textColor
            id: scanLabel
            Layout.alignment: Qt.AlignCenter
            wrapMode: Text.WordWrap
            text: qsTr("Scan QR Code")
        }

        QRScannerViewfinder {
            id: qrScannerViewfinder
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            zxingFilter.decoder.onTagFound: {
                if (tag.startsWith("bitcoin:")) {
                    pasteTextArea.text = tag.substring(8);
                }
            }
        }

        QQC2.TextArea {
            id: pasteTextArea
            font: fixedFont
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            placeholderText: qsTr("Or Paste Address Here")
            background: Rectangle {
                anchors.fill: parent
            }
            onTextChanged: {
                checkIfValidOnChainAddress(text);
            }
        }

        QQC2.TextArea {
            id: amountTextArea
            enabled: !withdrawAllSwitch.checked
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            placeholderText: qsTr("Amount to Withdraw")
            background: Rectangle {
                anchors.fill: parent
            }
        }

        RowLayout {

            QQC2.Switch {
                id: withdrawAllSwitch
                checked: false
                onCheckedChanged: {
                    if (checked) {
                        amountTextArea.text = walletModel.totalAvailableFunds
                    }
                }
            }

            QQC2.Label {
                color: Kirigami.Theme.textColor
                text: qsTr("Withdraw All")
            }

        }

        QQC2.Button {
            id: withdrawButton
            enabled: pasteTextArea.text.length > 0
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            text: qsTr("WITHDRAW")
            onClicked: {
                walletModel.withdrawFunds(pasteTextArea.text,
                                          withdrawAllSwitch.checked ? "all" : amountTextArea.text)
            }
        }
    }


    function checkIfValidOnChainAddress(text) {
        // validate somehow?
    }
}
