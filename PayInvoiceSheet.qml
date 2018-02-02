import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {

    property int createdAtTimestamp: 0
    property string currency: "bc"
    property string description
    property int expiry
    property int msatoshiAmount: 0
    property int timestamp
    property string payee
    property string bolt11

    property var locale: Qt.locale()
    property date currentDate: new Date()

    ColumnLayout {



        QQC2.Label {
            wrapMode: Text.WordWrap
            text: "INVOICE INFO"
        }



        QQC2.Label {
            wrapMode: Text.WordWrap
            text: msatoshiAmount
        }



        QQC2.Label {
            wrapMode: Text.WordWrap
            text: description
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: Date(createdAtTimestamp + expiry).toLocaleDateString()
        }

        RowLayout {

        QQC2.Button {
            text: "Pay"
            onClicked: {
                paymentsModel.pay(bolt11)
            }
        }

        QQC2.Button {
            text: "Cancel"
            onClicked: {
                sheetOpen.false
            }
        }

        }
    }


}
