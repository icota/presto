import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.ApplicationWindow {
    id: root
    width: 1000
    height: 600
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
                text: "My Invoices"
                onTriggered: {
                    // show invoices list
                    pageStack.push(myInvoicesPageComponent)
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

    pageStack.initialPage: transactionsPageComponent

    Component {
        id: myInvoicesPageComponent

        Kirigami.ScrollablePage {
            title: "Invoices"

            actions {
                main: Kirigami.Action {
                    iconName: sendInvoiceSheet.sheetOpen ? "dialog-cancel" : "list-add"
                    onTriggered: {
                        sendInvoiceSheet.sheetOpen = !sendInvoiceSheet.sheetOpen
                    }
                }
            }

            ListView {
                id: invoicesListView
                model: invoicesModel
                anchors.fill: parent
                delegate: Kirigami.SwipeListItem {

                    QQC2.Label { text: "Label: " + label + ", msatoshi: " + msatoshi + ", " + expiresAt }
                    actions: [
                        Kirigami.Action {
                            iconName: "edit-delete"
                            onTriggered: {
                                // modal dialog to delete
                            }
                        }
                    ]
                }
            }
        }
    }

    Component {
        id: peersPageComponent

        Kirigami.ScrollablePage {
            title: "Peers"

            ListView {
                id: peersListView
                model: peersModel
                anchors.fill: parent
                delegate: Kirigami.SwipeListItem {

                    QQC2.Label { text: "Peer: " + peerid + ", connected: " + connected + ", " + msatoshitotal }
                    actions: [
                        Kirigami.Action {
                            iconName: "list-add"
                            onTriggered: {
                                fundChannelSheet.peerToFund = peerid
                                fundChannelSheet.sheetOpen = !fundChannelSheet.sheetOpen
                            }
                        }
                    ]
                }
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
                        walletModel.requestNewAddress()
                    }
                }
                right: Kirigami.Action {
                    iconName: "document-edit"
                    onTriggered: {
                        print("Right action triggered")
                    }
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

    // Sheets

    CaptureInvoiceSheet {
        id: captureInvoiceSheet
    }

    FundChannelSheet {
        id: fundChannelSheet
    }

    PayInvoiceSheet {
        id: payInvoiceSheet
    }

    OnchainAddressSheet {
        id: onchainAddressSheet
    }

    SendInvoiceSheet {
        id: sendInvoiceSheet
    }

    ShareInvoiceSheet {
        id: shareInvoiceSheet
    }

    // Connections

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

    Connections {
        target: invoicesModel
        onInvoiceAdded: {
            // share invoice
            shareInvoiceSheet.bolt11 = bolt11;
            sendInvoiceSheet.sheetOpen = false;
            shareInvoiceSheet.sheetOpen = true;
        }
    }
}

