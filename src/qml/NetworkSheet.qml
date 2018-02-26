import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import Qt.labs.settings 1.0
import "." // QTBUG-34418

Kirigami.OverlaySheet {
    header:
        OverlaySheetHeader {
        text: qsTr("Network")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {
        Settings {
            id: settings
            // we will add stuff to change rgb and node alias
        }


        QQC2.Label {
            wrapMode: Text.WordWrap
            font: fixedFont
            text: lightningModel.network
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: "Your Node ID:"
        }

        QQC2.Label {
            id: idLabel
            wrapMode: Text.WordWrap
            font: fixedFont
            text: lightningModel.id.substring(0, 10)
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    clipboard.setText(lightningModel.id)
                    showPassiveNotification(qsTr("Node ID Copied to Clipboard"))
                }
            }
        }
    }
}
