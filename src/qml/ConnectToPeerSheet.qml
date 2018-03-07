import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    header:
        OverlaySheetHeader {
        text: qsTr("Add Peer")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {
        QQC2.TextField {
            id: idTextField
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            selectByMouse: true
            font: fixedFont
            placeholderText: qsTr("Peer ID")
            onTextEdited: {
                if (text.includes("@"))
                {
                    var addressStringSplit = text.split("@")
                    text = addressStringSplit[0]
                    addressTextField.text = addressStringSplit[1].replace(/(\r\n|\n|\r)/gm,""); // Remove any newlines left after pasting from the browser
                }
            }
        }

        QQC2.TextField {
            id: addressTextField
            selectByMouse: true
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font: fixedFont
            placeholderText: qsTr("Peer Address")
        }

        QQC2.Button {
            id: connectButton
            Layout.topMargin: 25
            text: qsTr("Connect")
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font.pixelSize: 16
            enabled: addressTextField.length > 0
            onClicked: {
                peersModel.connectToPeer(idTextField.text, addressTextField.text)
                busyIndicator.visible = true
                enabled = false
                text = ""
            }

            QQC2.BusyIndicator {
                id: busyIndicator
                anchors.fill: parent
                visible: false
            }

            Connections {
                target: peersModel

                onErrorString: {
                    connectButton.reEnableButton()
                }

                onConnectedToPeer: {
                    connectButton.reEnableButton()
                }
            }

            function reEnableButton () {
                busyIndicator.visible = false
                connectButton.enabled = true
                text = qsTr("Connect")
            }
        }
    }
}
