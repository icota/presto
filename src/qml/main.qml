import QtQuick 2.7
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

import Lightning.Invoice 1.0

Kirigami.ApplicationWindow {
    id: root
    width: 1000
    height: 600
    title: qsTr("Presto!")

    header: Kirigami.ApplicationHeader {
        headerStyle: Kirigami.ApplicationHeaderStyle.TabBar
        backButtonEnabled: false
        minimumHeight: Kirigami.Units.gridUnit * 2.5
        preferredHeight: Kirigami.Units.gridUnit * 2.5
        maximumHeight: Kirigami.Units.gridUnit * 2.5
    }
    globalDrawer: Kirigami.GlobalDrawer {
        // Depends on the screen
        //drawerOpen: false
        id: globalDrawer

        topContent: [
            BalanceItem {
                id: balanceItem
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.margins: 10
            },
            Kirigami.Separator {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.margins: 10
            }
        ]

        actions: [
            Kirigami.Action {
                enabled: lightningModel.connectedToDaemon
                text: "Lightning Network (" +
                      (peersModel.totalAvailableFunds / 1000).toLocaleString(locale, 'f', 0) +
                      " SAT)"
                iconName: "view-list-icons"
                Kirigami.Action {
                    text: qsTr("Connect to Peer")
                    iconName: "contact-new"
                    onTriggered: {
                        pageStack.currentIndex = 2;
                        connectToPeerSheet.sheetOpen = !connectToPeerSheet.sheetOpen
                    }
                }
                Kirigami.Action {
                    text: qsTr("Send Payment")
                    iconName: "go-up"
                    onTriggered: {
                        pageStack.currentIndex = 0;
                        captureInvoiceSheet.sheetOpen = !captureInvoiceSheet.sheetOpen
                    }

                }
                Kirigami.Action {
                    text: qsTr("Request Payment")
                    iconName: "go-down"
                    onTriggered: {
                        pageStack.currentIndex = 1;
                        sendInvoiceSheet.sheetOpen = !sendInvoiceSheet.sheetOpen
                    }
                }
            },
            Kirigami.Action {
                enabled: lightningModel.connectedToDaemon
                text: "On-Chain (" +
                      walletModel.totalAvailableFunds.toLocaleString(locale, 'f' , 0) +
                      " SAT)"
                iconName: "view-list-icons"
                Kirigami.Action {
                    text: qsTr("Send Payment")
                    iconName: "go-up"
                    onTriggered: {
                        onchainWithdrawSheet.sheetOpen = !onchainWithdrawSheet.sheetOpen
                    }
                }
                Kirigami.Action {
                    text: qsTr("Request Payment")
                    iconName: "go-down"
                    onTriggered: {
                        walletModel.requestNewAddress()
                    }
                }
            },


            Kirigami.Action {
                text: "Preferences"
                iconName: "applications-system"
                onTriggered: {
                    preferencesSheet.sheetOpen = true;
                }
            },

            Kirigami.Action {
                text: "Network"
                iconName: "network-workgroup"
                onTriggered: {
                    networkSheet.sheetOpen = true;
                }
            },
            Kirigami.Action {
                text: "About"
                iconName: "help-about"
                enabled: false
                onTriggered: {
                }
            }
        ]
    }

    pageStack.initialPage: transactionsPageComponent

    Component {
        id: transactionsPageComponent

        Kirigami.ScrollablePage {
            title: qsTr("Payments") + " (" + paymentsListView.count + ")"
            actions {
                main: Kirigami.Action {
                    visible: lightningModel.connectedToDaemon
                    iconName: captureInvoiceSheet.sheetOpen ? "dialog-cancel" : "document-send"
                    text: qsTr("Pay")
                    onTriggered: {
                        captureInvoiceSheet.sheetOpen = !captureInvoiceSheet.sheetOpen
                    }
                }
            }

            ListView {
                id: paymentsListView
                model: paymentsModel
                anchors.fill: parent
                delegate: Kirigami.SwipeListItem {
                    supportsMouseEvents: true
                    GenericListDelegate {
//                        indicator.color: paid ? "green" : "red"
//                        label.text: label
//                        status.text: "invoicestatus"
//                        msatoshiAmount.amount: msatoshi
                    }

                    actions: [
                        Kirigami.Action {
                            iconName: "edit-delete"
                            tooltip: qsTr("Delete")
                            onTriggered: {
                                //invoicesModel.deleteInvoice(label, status)
                            }
                        }
                    ]
                }
            }
        }
    }


    Component {
        id: invoicesPageComponent

        Kirigami.ScrollablePage {
            title: qsTr("Invoices") + " (" + invoicesListView.count + ")"

            actions {
                main: Kirigami.Action {
                    visible: lightningModel.connectedToDaemon
                    iconName: sendInvoiceSheet.sheetOpen ? "dialog-cancel" : "document-new"
                    text: qsTr("Create a new Invoice")
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
                    supportsMouseEvents: true
                    GenericListDelegate {
                        indicator.color: status === Invoice.PAID ? "green" : "grey"
                        label.text: invoicelabel
                        status.text: qsTr("Expires at " + expiresAt);
                        msatoshiAmount.amount: msatoshi
                    }

                    actions: [
                        Kirigami.Action {
                            iconName: "document-share"
                            tooltip: qsTr("Share")
                            onTriggered: {
                                shareInvoiceSheet.bolt11 = bolt11;
                                shareInvoiceSheet.sheetOpen = true;
                            }
                        },
                        Kirigami.Action {
                            iconName: "edit-delete"
                            tooltip: qsTr("Delete")
                            onTriggered: {
                                invoicesModel.deleteInvoice(invoicelabel, status)
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
            id: peersScrollablePage
            title: qsTr("Peers") + " (" + peersListView.count + ")"

            actions {
                main: Kirigami.Action {
                    visible: lightningModel.connectedToDaemon
                    iconName: connectToPeerSheet.sheetOpen ? "dialog-cancel" : "list-add"
                    text: qsTr("Connect to a Peer")
                    onTriggered: {
                        connectToPeerSheet.sheetOpen = !connectToPeerSheet.sheetOpen
                    }
                }
            }

            property string peerIdToClose
            function closeChannel() {
                peersModel.closeChannel(peerIdToClose)
            }

            ListView {
                id: peersListView
                model: peersModel

                anchors.fill: parent
                delegate: Kirigami.SwipeListItem {
                    supportsMouseEvents: true
                    GenericListDelegate {
                        indicator.color: connected && peerstatestring == "CHANNELD_NORMAL" ? "green" : connected ? "orange" : "red"
                        indicatorTooltip: connected ?
                                              qsTr("Connected Status") + ": " + peerstatestring : qsTr("Disconnected")
                        label.text: peerid.substring(0, 10) + (connected ? " (" + netaddress + ")" : "")
                        status.text: peerstatestring
                        msatoshiAmount.amount: msatoshitous
                    }

                    actions: [
                        Kirigami.Action {
                            iconName: "list-add"
                            text: qsTr("Fund the Channel")
                            onTriggered: {
                                fundChannelSheet.peerToFund = peerid
                                fundChannelSheet.sheetOpen = !fundChannelSheet.sheetOpen
                            }
                        },
                        Kirigami.Action {
                            iconName: "dialog-cancel"
                            text: qsTr("Close the Channel")
                            onTriggered: {
                                peersScrollablePage.peerIdToClose = peerid
                                showPassiveNotification(qsTr("Close the Channel to this Peer?"), 10000, "OK", closeChannel)
                            }
                        }
                    ]
                }
            }
        }
    }

    Component.onCompleted: {
        // Gotta be a nicer way
        pageStack.push(invoicesPageComponent)
        pageStack.push(peersPageComponent)
        pageStack.currentIndex = 0;

        ExchangeRate.locale = locale
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

    ConnectToPeerSheet {
        id: connectToPeerSheet
    }

    OnchainWithdrawSheet {
        id: onchainWithdrawSheet
    }

    PreferencesSheet {
        id: preferencesSheet
    }

    NetworkSheet {
        id: networkSheet
    }

    NodeSettingsSheet {
        id: nodeSettingsSheet
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

        onErrorString: {
            showPassiveNotification(error)
        }
    }

    Connections {
        target: walletModel
        onNewAddress: {
            onchainAddressSheet.onchainAddress = newAddress
            onchainAddressSheet.sheetOpen = !captureInvoiceSheet.sheetOpen
        }

        onErrorString: {
            showPassiveNotification(error)
        }
    }

    Connections {
        target: peersModel
        onErrorString: {
            showPassiveNotification(error)
        }

        onConnectedToPeer: {
            connectToPeerSheet.sheetOpen = false
            fundChannelSheet.peerToFund = peerId
            fundChannelSheet.sheetOpen = true
        }

        onChannelFunded: {
            fundChannelSheet.sheetOpen = false
        }
    }

    Connections {
        target: invoicesModel
        onInvoiceAdded: {
            // Share the invoice
            shareInvoiceSheet.bolt11 = bolt11;
            sendInvoiceSheet.sheetOpen = false;
            shareInvoiceSheet.sheetOpen = true;
        }

        onErrorString: {
            showPassiveNotification(error)
        }
    }
}
