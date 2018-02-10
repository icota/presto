import QtQuick 2.7
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Item {
    anchors.fill: parent
    //property alias amountMsatoshi: msatoshiAmountItem.amount
    RowLayout
    {
        anchors.fill: parent
        // status icon with tooltip" , connected: " + connected

        // peerid or
        QQC2.Label {
            id: peerIdLabel
            color: Kirigami.Theme.textColor
            font: fixedFont
            text: peerid
        }

        // always amount

        MsatoshiAmountItem {
            anchors.left: peerIdLabel.right
            id: msatoshiAmountItem
            amount: msatoshitous
        }
    }
}
