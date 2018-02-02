import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    property string bolt11

    ColumnLayout {

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: "Share this invoice"
        }

        QRCode {
            Layout.alignment: Qt.AlignCenter
            //Layout.fillWidth: true
            width : 320
            height : 320
            value : "lightning:" + bolt11
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: bolt11
        }
    }
}
