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
        QRScannerViewfinder {
            id: qrScannerViewfinder
            Layout.alignment: Qt.AlignCenter
            width: parent.width
            height: width
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
            onTextChanged: {
                checkIfValidOnChainAddress(text);
            }
        }

        AmountTextField {
            milisatoshi: false
            id: amountTextField
        }

        RowLayout {

            QQC2.Switch {
                id: withdrawAllSwitch
                checked: false
                onCheckedChanged: {
                    if (checked) {
                        amountTextField.enabled = false
                        amountTextField.amount = walletModel.totalAvailableFunds
                    }
                    else
                    {
                        amountTextField.enabled = true

                    }
                }
            }

            QQC2.Label {            
                text: qsTr("Send All")
            }

        }

        QQC2.Button {
            id: withdrawButton
            enabled: pasteTextArea.text.length > 0
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            text: qsTr("Send")
            onClicked: {
                walletModel.withdrawFunds(pasteTextArea.text,
                                          withdrawAllSwitch.checked ? "all" : amountTextField.amount)
            }
        }
    }


    function checkIfValidOnChainAddress(text) {
        // validate somehow?
    }
}
