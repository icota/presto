import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import QtGraphicalEffects 1.0

import QtMultimedia 5.8
import QZXing 2.3

Rectangle {
    id: viewfinderRectangle
    color: "black"

    radius: Kirigami.Units.gridUnit / 2
    clip: true

    property alias camera: camera
    property alias zxingFilter: zxingFilter
    
    VideoOutput {
        id: viewfinderOutput
        source: camera
        autoOrientation: true
        filters: [ zxingFilter ]
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectCrop
        layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: viewfinderRectangle
            }
    }

    Rectangle {
        id: captureZone
        color: "red"
        opacity: 0.2
        width: parent.width / 2
        height: parent.height / 2
        anchors.centerIn: parent
        radius: Kirigami.Units.gridUnit / 2
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
                return viewfinderOutput.mapRectToSource(
                            viewfinderOutput.mapNormalizedRectToItem(
                                Qt.rect(0.25, 0.25, 0.5, 0.5)));
            }
            
            decoder {
                enabledDecoders: QZXing.DecoderFormat_QR_CODE
            }
        }
    }
}
