import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    parentWidthRatio: 0.75

    property string peerToFund

    ColumnLayout {

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: qsTr("Peer ID: " + peerToFund)
        }

        QQC2.TextField {
            id: amountTextField
            validator: IntValidator {bottom: 0;}
            placeholderText: qsTr("Enter Amount in milisatoshi")
        }

        QQC2.Button {
            text: "Fund"
            onClicked: {
                peersModel.fundChannel(peerToFund, amountTextField.text)
            }
        }
    }
}
