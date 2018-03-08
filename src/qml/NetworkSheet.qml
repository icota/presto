import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import Qt.labs.settings 1.0
import "." // QTBUG-34418

Kirigami.OverlaySheet {
    property string nodeURI: lightningModel.id +
                             "@" +
                             lightningModel.address +
                             ":" +
                             lightningModel.port

    header:
        OverlaySheetHeader {
        text: qsTr("My Node")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {
        Settings {
            id: settings
            // we will add stuff to change rgb and node alias
        }

        QRCode {
            Layout.alignment: Qt.AlignCenter
            // This is the URI format Eclair wallet supports
            value : nodeURI
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    clipboard.setText(nodeURI)
                    showPassiveNotification(qsTr("Node URI Copied to Clipboard"))
                }
            }
        }

        QQC2.Label {
            id: idLabel
            wrapMode: Text.WordWrap
            Layout.alignment: Qt.AlignCenter
            width: 100
            font: fixedFont
            text: lightningModel.id//.substring(0, 10)
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    clipboard.setText(lightningModel.id)
                    showPassiveNotification(qsTr("Node ID Copied to Clipboard"))
                }
            }
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            font: fixedFont
            text: "(" + lightningModel.network + ")"
            Layout.alignment: Qt.AlignCenter
        }

    }
}
