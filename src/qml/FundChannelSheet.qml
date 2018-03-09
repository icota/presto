import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

Kirigami.OverlaySheet {
    property string peerToFund

    header:
        OverlaySheetHeader {
        text: qsTr("Fund Channel")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {

        QQC2.Label {       
            wrapMode: Text.Wrap
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font.pixelSize: 16
            text: qsTr("Peer ID:")
        }

        QQC2.Label {          
            wrapMode: Text.Wrap
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font: fixedFont
            text: peerToFund.substring(0, 10)
        }

        AmountTextField {
            milisatoshi: false
            id: amountTextField
            Layout.topMargin: 25
        }


        QQC2.Button {
            Layout.topMargin: 25
            text: qsTr("OK");
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font.pixelSize: 16
            enabled: amountTextField.amount
            onClicked: {
                peersModel.fundChannel(peerToFund, amountTextField.amount)
            }
        }
    }
}
