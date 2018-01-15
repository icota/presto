import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

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
        }

        Page {
            ListView {
                id: peersListView
                model: peersModel
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
