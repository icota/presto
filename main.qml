import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.ApplicationWindow {
    id: root
    width: 640
    height: 480
    title: qsTr("Presto!")
    header: Kirigami.ApplicationHeader {}
    globalDrawer: Kirigami.GlobalDrawer {
        drawerOpen: false
        title: "Presto!"
        titleIcon: "applications-graphics"
        actions: [
            Kirigami.Action {
                text: "View"
                iconName: "view-list-icons"
                Kirigami.Action {
                    text: "action 1"
                }
                Kirigami.Action {
                    text: "action 2"
                }
                Kirigami.Action {
                    text: "action 3"
                }
            },
            Kirigami.Action {
                text: "Pay Lightning Invoice"
                onTriggered: {
                    console.log("shits");
                }
            },
            Kirigami.Action {
                text: "action 4"
            }
        ]
    }
    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }
    pageStack.initialPage: mainPageComponent
    Component {
        id: mainPageComponent



        Kirigami.ScrollablePage {
            title: "Transactions"
            actions {
                main: Kirigami.Action {
                    iconName: captureInvoiceSheet.sheetOpen ? "dialog-cancel" : "document-edit"
                    onTriggered: {
                        print("Action button in buttons page clicked");
                        captureInvoiceSheet.sheetOpen = !captureInvoiceSheet.sheetOpen
                    }
                }
                left: Kirigami.Action {
                    iconName: "go-previous"
                    onTriggered: {
                        print("Left action triggered")
                    }
                }
                right: Kirigami.Action {
                    iconName: "go-next"
                    onTriggered: {
                        print("Right action triggered")
                    }
                }
                contextualActions: [
                    Kirigami.Action {
                        text:"Action for buttons"
                        iconName: "bookmarks"
                        onTriggered: print("Action 1 clicked")
                    },
                    Kirigami.Action {
                        text:"Action 2"
                        iconName: "folder"
                        enabled: false
                    },
                    Kirigami.Action {
                        text: "Action for Sheet"
                        visible: captureInvoiceSheet.sheetOpen
                    }
                ]
            }

            CaptureInvoiceSheet {
                id: captureInvoiceSheet
            }

            PayInvoiceSheet {
                id: payInvoiceSheet
            }

            Connections {
                target: paymentsModel
                onPaymentDecoded: {
                    captureInvoiceSheet.sheetOpen = false;

                    payInvoiceSheet.createdAtTimestamp = createdAt;
                    payInvoiceSheet.currency = currency;
                    payInvoiceSheet.description = description;
                    payInvoiceSheet.expiry = expiry;
                    payInvoiceSheet.msatoshiAmount = msatoshi;
                    payInvoiceSheet.payee = payee;

                    payInvoiceSheet.sheetOpen = true;

                }
            }

            ListView {
                id: paymentsListView
                model: paymentsModel
                anchors.fill: parent
                delegate: Text { text: "Amount: " + msatoshi + ", Incoming: " + incoming + ", Timestamp: " + timestamp }
            }

            //            QQC2.SwipeView {
            //                id: swipeView
            //                anchors.fill: parent
            //                //currentIndex: tabBar.currentIndex

            //                Page1 {
            //                }

            //                QQC2.Page {
            //                    ListView {
            //                        id: peersListView
            //                        model: peersModel
            //                        anchors.fill: parent
            //                        delegate: Text { text: "Peer: " + peerid + ", " + msatoshitotal }
            //                    }
            //                }
            //            }
        }
    }
}
