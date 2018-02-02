import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    //parentWidthRatio: 0.75

    property string onchainAddress

    ColumnLayout {

        QQC2.Label {
            visible: !walletModel.totalAvailableFunds
            wrapMode: Text.WordWrap
            text: "NO FUNDS"
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: "Pay to this address"
        }

        QRCode {
            Layout.alignment: Qt.AlignCenter
            //Layout.fillWidth: true
            width : 320
            height : 320
            value : "bitcoin:" + onchainAddress
        }
    }
}
