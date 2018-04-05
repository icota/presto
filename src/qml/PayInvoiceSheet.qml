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
        QQC2.Label {
            wrapMode: Text.Wrap
            text: description
            textFormat: Text.RichText
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize: 16
            Layout.maximumWidth: Kirigami.Units.gridUnit * 15
        }

        AmountTextField {
            milisatoshi: false
            id: amountTextField
            Layout.topMargin: Kirigami.Units.smallSpacing
            visible: msatoshiAmount === -1
        }

        QQC2.Button {
            text: msatoshiAmount === -1 ? qsTr("Pay") : qsTr("Pay") + " " +
                                          (msatoshiAmount / 1000).toLocaleString(locale, 'f' , 0) +
                                          " SAT (" + ExchangeRate.getAmountInCurrency(msatoshiAmount / 1000) + ")"
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignHCenter
            //Layout.fillWidth: true
            font.pixelSize: 16
            onClicked: {
                if (msatoshiAmount === -1) {
                    paymentsModel.pay(bolt11, amountTextField.amount * 1000)
                }
                else {
                    paymentsModel.pay(bolt11)
                }
            }
        }
    }
}
