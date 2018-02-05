import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    ColumnLayout {

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: "Network"
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: lightningModel.network
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: "Your Node ID:"
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: lightningModel.id
        }
    }
}
