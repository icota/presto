import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    property string onchainAddress

    header:
        OverlaySheetHeader {
        text: qsTr("My Address")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {
        QRCode {
            Layout.alignment: Qt.AlignHCenter
            width: parent.width * 0.8
            value : "bitcoin:" + onchainAddress
        }

        QQC2.Label {
            
            Layout.alignment: Qt.AlignCenter
            wrapMode: Text.Wrap
            font: fixedFont
            text: onchainAddress

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    clipboard.setText(onchainAddress)
                    showPassiveNotification(qsTr("Address Copied to Clipboard"))
                }
            }
        }
    }
}
