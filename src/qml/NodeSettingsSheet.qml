import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import Qt.labs.settings 1.0
import "." // QTBUG-34418

Kirigami.OverlaySheet {
    header:
        OverlaySheetHeader {
        text: qsTr("Node Settings")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {

        Settings {
            id: settings
            property string nodeAddress: nodeAddressTextField.text
            property string nodeRpcUsername: rpcUsernameTextField.text
            property string nodeRpcPassword: rpcPasswordTextField.text

        }

        QQC2.TextField {
            id: nodeAddressTextField
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            selectByMouse: true
            font: fixedFont
            placeholderText: qsTr("Node Address")
            text: settings.nodeAddress
        }

        QQC2.TextField {
            id: rpcUsernameTextField
            selectByMouse: true
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font: fixedFont
            placeholderText: qsTr("RPC Username")
            text: settings.nodeRpcUsername
        }

        QQC2.TextField {
            id: rpcPasswordTextField
            selectByMouse: true
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font: fixedFont
            placeholderText: qsTr("RPC Password")
            text: settings.nodeRpcPassword
            echoMode: TextInput.Password
            passwordCharacter: "*"
        }

        QQC2.Button {
            id: connectButton
            Layout.topMargin: 25
            text: qsTr("Connect")
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font.pixelSize: 16
            enabled: (rpcUsernameTextField.length > 0) &&
                     (rpcPasswordTextField.length > 0)
            onClicked: {
                busyIndicator.visible = true
                enabled = false
                text = ""
                // TODO: tell the model to launch daemon
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
