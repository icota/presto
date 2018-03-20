import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    property string bolt11

    header:
        OverlaySheetHeader {
        text: qsTr("Share Invoice")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {
        QRCode {
            Layout.alignment: Qt.AlignHCenter
            width: parent.width * 0.65
            value : "lightning:" + bolt11
        }

        QQC2.Label {
            wrapMode: Text.WrapAnywhere
            font: fixedFont
            text: bolt11
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            Layout.maximumWidth: parent.width * 0.65
            MouseArea {
                //anchors.fill: parent
                onClicked: {
                    clipboard.setText(bolt11)
                    showPassiveNotification(qsTr("Invoice Copied to Clipboard"))
                }
            }
        }
    }
}
