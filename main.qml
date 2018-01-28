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
                text: "Lightning Network"
                iconName: "view-list-icons"
                Kirigami.Action {
                    text: "Pay"
                }
                Kirigami.Action {
                    text: "Invoice"
                }
                Kirigami.Action {
                    text: "Peers"
                    onTriggered: {
                        pageStack.push(peersPageComponent)
                        // show list of peers
                    }
                }
                Kirigami.Action {
                    text: "Connect to Peer"
                    onTriggered: {
                        connectToPeerSheet.sheetOpen = !connectToPeerSheet.sheetOpen
                    }
                }
            },
            Kirigami.Action {
                text: "Pay Lightning Invoice"
                onTriggered: {
                    console.log("shits");
                }
            },
            Kirigami.Action {
                text: "Connect to LN peer"
                onTriggered: {
                    connectToPeerSheet.sheetOpen = !connectToPeerSheet.sheetOpen
                }
            }
        ]
    }

    ConnectToPeerSheet {
        id: connectToPeerSheet
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }
    pageStack.initialPage: transactionsPageComponent

    Component {
        id: peersPageComponent



        Kirigami.ScrollablePage {
            title: "Peers"

            ListView {
                id: peersListView
                model: peersModel
                anchors.fill: parent
                delegate: Text { text: "Peer: " + peerid + ", " + msatoshitotal }
            }
        }
    }

    Component {
        id: transactionsPageComponent



        Kirigami.ScrollablePage {
            title: walletModel.totalAvailableFunds + " SAT" + " / " // add LN funds
            actions {
                main: Kirigami.Action {
                    iconName: captureInvoiceSheet.sheetOpen ? "dialog-cancel" : "document-send"
                    onTriggered: {
                        print("Action button in buttons page clicked");
                        captureInvoiceSheet.sheetOpen = !captureInvoiceSheet.sheetOpen
                    }
                }
                left: Kirigami.Action {
                    iconName: "mail-send-receive"
                    onTriggered: {
                        print("Left action triggered")
                        walletModel.requestNewAddress()
                        // ask the thing for new address
                    }
                }
                right: Kirigami.Action {
                    iconName: "document-edit"
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

            OnchainAddressSheet {
                id: onchainAddressSheet
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

            Connections {
                target: walletModel
                onNewAddress: {
                    onchainAddressSheet.onchainAddress = newAddress
                    onchainAddressSheet.sheetOpen = !captureInvoiceSheet.sheetOpen
                }
            }

            ListView {
                id: paymentsListView
                model: paymentsModel
                anchors.fill: parent
                delegate: Text { text: "Amount: " + msatoshi + ", Incoming: " + incoming + ", Timestamp: " + timestamp }
            }
        }
    }
}
