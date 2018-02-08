import QtQuick 2.7
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
//import "." // QTBUG-34418, singletons require explicit import to load qmldir file

QQC2.Label {
    color: Kirigami.Theme.textColor
    text: "Peer: " + peerid +
          ", connected: " + connected +
          ", " + msatoshitotal }
