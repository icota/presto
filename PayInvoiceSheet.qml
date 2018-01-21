import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

import QtMultimedia 5.8
import QZXing 2.3

Kirigami.OverlaySheet {
    id: payInvoiceSheet
    //onSheetOpenChanged: page.actions.main.checked = sheetOpen
    QQC2.Label {
        wrapMode: Text.WordWrap
        text: "Scan the QR Code or Paste the Invoice"
    }

    QQC2.TextArea {
        placeholderText: "Paste the Invoice Here"
        onTextChanged: {
            // check if it's valid bolt11 and go
        }
    }

    Item {
        Camera {
            id: camera

            imageCapture {
                onImageCaptured: {
                    imageToDecode.source = preview
                    decoder.decodeImageQML(imageToDecode);
                }
            }
        }

        QZXing {
            id: decoder

            enabledDecoders: QZXing.DecoderFormat_QR_CODE

            onDecodingStarted: {

                console.log("Decoding of image started...")
            }

            onTagFound: console.log("Barcode data: " + tag)

            onDecodingFinished: console.log("Decoding finished " + (succeeded==true ? "successfully" :    "unsuccessfully") )
        }

        VideoOutput {
            source: camera
            visible: true
            height: 300
            width: 300
            //anchors.fill: parent
            focus : visible // to receive focus and capture key events when visible
        }

        Image {
            id: imageToDecode
//            height: 300
//            width: 300
        }

        function decode(preview) {
            imageToDecode.source = preview
            decoder.decodeImageQML(imageToDecode);
        }
    }








}

