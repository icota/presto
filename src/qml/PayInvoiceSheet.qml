import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

Kirigami.OverlaySheet {

    property int createdAtTimestamp: 0
    property string currency: "bc"
    property string description
    property int expiry
    property int msatoshiAmount: 0
    property int timestamp
    property string payee
    property string bolt11

    property date currentDate: new Date()

    header:
        OverlaySheetHeader {
        text: qsTr("Payment Details")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {

        RowLayout {
            QQC2.Label {
                
                wrapMode: Text.WordWrap
                text: msatoshiAmount + " mSAT"
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: parent
                font.pixelSize: 16
            }

            QQC2.Label {
                
                wrapMode: Text.WordWrap
                text: ExchangeRate.getAmountInCurrency(msatoshiAmount / 1000)
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: parent
                font.pixelSize: 12
            }
        }


        QQC2.Label {
            
            wrapMode: Text.WordWrap
            text: description
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            font.pixelSize: 16
        }

        RowLayout {

            QQC2.Button {
                text: qsTr("Pay")
                Layout.topMargin: 25
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true
                font.pixelSize: 16
                onClicked: {
                    paymentsModel.pay(bolt11)
                }
            }
        }
    }


}
