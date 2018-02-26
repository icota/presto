import QtQuick 2.7
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

RowLayout
{
    property alias indicator: indicator
    property string indicatorTooltip
    property alias label: label
    property alias status: status
    property alias msatoshiAmount: msatoshiAmount

    Rectangle {
        id: indicator
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        Layout.leftMargin: 10
        width: 25
        height: 25
        radius: width * 0.5
    }

    ColumnLayout {
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        Layout.fillWidth: true

        // FIX: Gotta be a more proper way to align
        anchors.left: indicator.right
        anchors.margins: 15

        QQC2.Label {
            id: label
            
            font: fixedFont
        }

        QQC2.Label {
            id: status
            
            font: fixedFont
        }
    }

    MsatoshiAmountItem {
        id: msatoshiAmount
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
    }
}
