import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

Kirigami.OverlaySheet {
    property string peerToFund

    ColumnLayout {

        QQC2.Label {
            color: Kirigami.Theme.textColor
            wrapMode: Text.WordWrap
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font.pixelSize: 16
            text: qsTr("Peer ID")
        }

        QQC2.Label {
            color: Kirigami.Theme.textColor
            wrapMode: Text.WordWrap
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font: fixedFont
            text: peerToFund
        }

        MsatoshiTextField {
            satoshi: true
            id: amountTextField
            Layout.topMargin: 25
        }


        QQC2.Button {
            Layout.topMargin: 25
            text: "Fund Channel"
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font.pixelSize: 16
            onClicked: {
                peersModel.fundChannel(peerToFund, amountTextField.amount)
            }
        }
    }
}
