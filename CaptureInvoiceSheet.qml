import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

import QtMultimedia 5.8
import QZXing 2.3

Kirigami.OverlaySheet {
    id: captureInvoiceSheet
    parentWidthRatio: 0.75

    ColumnLayout {
        //anchors.fill: parent

        QQC2.Label {
            id: scanLabel
            Layout.alignment: Qt.AlignCenter
            wrapMode: Text.WordWrap
            text: qsTr("Scan the QR Code")
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log();
                }
            }
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
                //Layout.alignment: Qt.AlignCenter
                //Layout.preferredWidth: parent.width
                //Layout.fillWidth: true
                //width: 200
                //height: 200
                //Layout.fillHeight: true
                anchors.fill: parent
                onContentRectChanged: {
                    viewfinderRectangle.height = contentRect.height
                }

                //width: parent.width
                //height: parent.height
                //focus : visible // to receive focus and capture key events when visible
            }

            Item {
                id: cameraViewFinderItem
                visible: false
                Camera {
                    id: camera
                    //captureMode: CaptureMode.CaptureStillImage

                    imageCapture {
                        id: qrCapture
                        onImageCaptured: {
                            imageToDecode.source = preview
                            decoder.decodeImageQML(imageToDecode);
                        }

                        onReadyForCaptureChanged: {
                            if (qrCapture.ready) {
                                //camera.start()
                                //capture();
                            }
                        }
                    }
                }

                QZXingFilter {
                    id: zxingFilter
                    captureRect: {
                        // setup bindings
                        viewfinderOutput.contentRect;
                        viewfinderOutput.sourceRect;
                        return viewfinderOutput.mapRectToSource(viewfinderOutput.mapNormalizedRectToItem(Qt.rect(
                                                                                                             0.25, 0.25, 0.5, 0.5
                                                                                                             )));
                    }
                    decoder {
                        enabledDecoders: QZXing.DecoderFormat_QR_CODE

                        onTagFound: {
                            console.log("Barcode data: " + tag)
                        }


                    }

                    onDecodingStarted: {
                        console.log("Decoding of image started...")
                    }
                    onDecodingFinished: {
                        console.log("Decoding finished " + (succeeded==true ? "successfully" :    "unsuccessfully") )
                    }
                }
            }

        }

        QQC2.TextArea {
            id: pasteTextArea
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            placeholderText: qsTr("Or Paste the Invoice Here")
            background: Rectangle {
                anchors.fill: parent
            }
            onTextChanged: {
                checkIfValidBolt11(text);
                // check if it's valid bolt11 and go
            }
        }
    }



    function checkIfValidBolt11(text) {
        paymentsModel.decodePayment("lnbc1u1pdxf4egpp5la3wgzamjh59rhlax7860ggf6vcwg8z73n5eg5fvslzcaaq4zn9sdru0v3xgg36yffx2ctyypqhyarfvdkx2w3qfphhqurfw3uj66r0wqsjytpzdy3r5g3c89jrwdmpv5mj6wtzv3jj6dp4xgmj6wtxxd3j6e3c89sn2ctzvcmkyefsyf7scqzysh6ppvsc27jyhnxlppc589zn28cfmnqlzxru7zjc8rr56mjkd62nyjdj7eyqwwv860yug4jfwfn77jm0y0d698sr04leh6hynfylwgwsqevz6ms");
    }
}

