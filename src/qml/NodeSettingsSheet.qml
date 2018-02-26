import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import Qt.labs.settings 1.0
import "." // QTBUG-34418

Kirigami.OverlaySheet {
    header:
        OverlaySheetHeader {
        text: qsTr("Node Settings")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }
}
