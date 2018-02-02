import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    parentWidthRatio: 0.75

    ColumnLayout {
        //anchors.fill: parent

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: qsTr("Peer ID")
        }

        QQC2.TextField {
            id: idTextField
            placeholderText: qsTr("Enter ID")
        }


        QQC2.Label {
            wrapMode: Text.WordWrap
            text: qsTr("Peer Address")
        }

        QQC2.TextField {
            id: addressTextField
            placeholderText: qsTr("Enter address")
        }

        QQC2.Button {
            text: "Connect"
            onClicked: {
                peersModel.connectToPeer(idTextField.text, addressTextField.text)
            }
        }
    }
}
