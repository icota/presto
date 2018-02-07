import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

import QtMultimedia 5.8
import QZXing 2.3

Kirigami.OverlaySheet {
    onSheetOpenChanged: {
        if (sheetOpen) {
            camera.start()
        }
        else {
            camera.stop()
        }
    }
    ColumnLayout {
        //anchors.fill: parent

        QQC2.Label {
            id: scanLabel
            Layout.alignment: Qt.AlignCenter
            wrapMode: Text.WordWrap
            text: qsTr("Scan the QR Code")
        }

        Rectangle {
            id: viewfinderRectangle
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            height: 400

            VideoOutput {
                id: viewfinderOutput
                source: camera
                visible: true
                filters: [ zxingFilter ]
                anchors.fill: parent
                onContentRectChanged: {
                    viewfinderRectangle.height = contentRect.height
                }
            }

            Item {
                id: cameraViewFinderItem
                visible: false
                Camera {
                    id: camera
                    cameraState: Camera.UnloadedState
                    imageCapture {
                        id: qrCapture
                        onImageCaptured: {
                            imageToDecode.source = preview
                            decoder.decodeImageQML(imageToDecode);
                        }
                    }
                }

                QZXingFilter {
                    id: zxingFilter
                    captureRect: {
                        // setup bindings
                        viewfinderOutput.contentRect;
                        viewfinderOutput.sourceRect;
                        return viewfinderOutput.mapRectToSource(viewfinderOutput.mapNormalizedRectToItem(
                                                                    Qt.rect(0.25, 0.25, 0.5, 0.5)));
                    }
                    decoder {
                        enabledDecoders: QZXing.DecoderFormat_QR_CODE

                        onTagFound: {
                            console.log("Barcode data: " + tag)
                            if (tag.startsWith("bitcoin:")) {
                                pasteTextArea.text = tag.substring(8);
                            }
                        }
                    }
                }
            }
        }

        QQC2.TextArea {
            id: pasteTextArea
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            placeholderText: qsTr("Or Paste the Address Here")
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
