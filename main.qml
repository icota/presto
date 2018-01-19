import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.0 as Kirigami

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Presto!")

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page1 {
            Kirigami.Action {
                id: lightningSendAction
            }
        }

        Page {
            ListView {
                id: peersListView
                model: peersModel
                anchors.fill: parent
                delegate: Text { text: "Peer: " + peerid + ", " + msatoshitotal }
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("TRANSACTIONS")
        }
        TabButton {
            text: qsTr("CHANNELS OR PEERS?")
        }
    }
}
