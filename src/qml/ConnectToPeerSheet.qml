import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    ColumnLayout {
        QQC2.Label {
            color: Kirigami.Theme.textColor
            wrapMode: Text.WordWrap
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font.pixelSize: 16
            text: qsTr("Peer ID")
        }

        QQC2.TextField {
            id: idTextField
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            selectByMouse: true
            font: fixedFont
            placeholderText: qsTr("Enter ID")
        }

        QQC2.Label {
            Layout.topMargin: 10
            color: Kirigami.Theme.textColor
            wrapMode: Text.WordWrap
            Layout.fillWidth: parent
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Peer Address")
            font.pixelSize: 16
        }

        QQC2.TextField {
            id: addressTextField
            selectByMouse: true
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            placeholderText: qsTr("Enter Address")
        }

        QQC2.Button {
            text: qsTr("Connect")
            font.pixelSize: 16
            Layout.topMargin: 25
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            enabled: addressTextField.length > 0
            onClicked: {
                peersModel.connectToPeer(idTextField.text, addressTextField.text)
            }
        }
    }
}
