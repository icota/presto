import QtQuick 2.7
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
//import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Kirigami.ApplicationWindow {
    id: root
    width: 1000
    height: 600
    title: qsTr("Presto!")

    header: Kirigami.ApplicationHeader {
        headerStyle: Kirigami.ApplicationHeaderStyle.TabBar
        backButtonEnabled: false
    }
    globalDrawer: Kirigami.GlobalDrawer {
        // Depends on the screen
        //drawerOpen: false

        topContent: [
            BalanceItem {
                anchors.leftMargin: 25
                height: 65
            }
        ]

        actions: [
            Kirigami.Action {
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
                text: "On-Chain (" +
                      walletModel.totalAvailableFunds.toLocaleString(locale, 'f' , 0) +
                      " SAT)"
                iconName: "view-list-icons"
                Kirigami.Action {
                    text: qsTr("Request Payment")
                    iconName: "go-down"
                    onTriggered: {
                        walletModel.requestNewAddress()
                    }
                }
                Kirigami.Action {
                    text: qsTr("Send Payment")
                    iconName: "go-up"
                    onTriggered: {
                        onchainWithdrawSheet.sheetOpen = !onchainWithdrawSheet.sheetOpen
                    }
                }
            },


            Kirigami.Action {
                text: "Settings"
                iconName: "preferences-system"
                onTriggered: {
                    settingsSheet.sheetOpen = true;
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
        id: invoicesPageComponent

        Kirigami.ScrollablePage {
            title: qsTr("Invoices") + " (" + invoicesListView.count + ")"

            actions {
                main: Kirigami.Action {
                    iconName: sendInvoiceSheet.sheetOpen ? "dialog-cancel" : "document-send"
                    tooltip: qsTr("Create a new Invoice")
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

                    QQC2.Label {
                        color: Kirigami.Theme.textColor
                        text: "Label: " + label +
                              ", msatoshi: " + msatoshi +
                              ", " + expiresAt }
                    actions: [
                        Kirigami.Action {
                            iconName: "edit-delete"
                            tooltip: qsTr("Delete")
                            onTriggered: {
                                invoicesModel.deleteInvoice(label, status)
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
            title: qsTr("Peers") + " (" + peersListView.count + ")"

            actions {
                main: Kirigami.Action {
                    iconName: connectToPeerSheet.sheetOpen ? "dialog-cancel" : "list-add"
                    tooltip: qsTr("Connect to a Peer")
                    onTriggered: {
                        connectToPeerSheet.sheetOpen = !connectToPeerSheet.sheetOpen
                    }
                }
            }

            ListView {
                id: peersListView
                model: peersModel
                anchors.fill: parent
                delegate: Kirigami.SwipeListItem {
                    height: 50
                    PeersDelegateItem {
                    }

                    actions: [
                        Kirigami.Action {
                            iconName: "list-add"
                            tooltip: qsTr("Add a new Peer")
                            onTriggered: {
                                fundChannelSheet.peerToFund = peerid
                                fundChannelSheet.sheetOpen = !fundChannelSheet.sheetOpen
                            }
                        },
                        Kirigami.Action {
                            iconName: "dialog-cancel"
                            tooltip: qsTr("Close the Channel")
                            onTriggered: {
                                peersModel.closeChannel(peerid)
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
            title: qsTr("Payments") + " (" + paymentsListView.count + ")"
            actions {
                main: Kirigami.Action {
                    iconName: captureInvoiceSheet.sheetOpen ? "dialog-cancel" : "document-send"
                    tooltip: qsTr("Pay")
                    onTriggered: {
                        captureInvoiceSheet.sheetOpen = !captureInvoiceSheet.sheetOpen
                    }
                }
            }

            ListView {
                id: paymentsListView
                model: paymentsModel
                anchors.fill: parent
                delegate: Text { text: "Amount: " + msatoshi +
                                       ", Incoming: " + incoming +
                                       ", Timestamp: " + timestamp }
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

    SettingsSheet {
        id: settingsSheet
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
            errorPopup.contentItem.text = error
            errorPopup.open()
        }
    }

    Connections {
        target: walletModel
        onNewAddress: {
            onchainAddressSheet.onchainAddress = newAddress
            onchainAddressSheet.sheetOpen = !captureInvoiceSheet.sheetOpen
        }

        onErrorString: {
            errorPopup.contentItem.text = error
            errorPopup.open()
        }
    }

    Connections {
        target: peersModel
        onErrorString: {
            errorPopup.contentItem.text = error
            errorPopup.open()
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
            errorPopup.contentItem.text = error
            errorPopup.open()
        }
    }

    // Notifications
    QQC2.Popup {
        id: errorPopup
        modal: true
        focus: true
        contentItem: QQC2.Label {}
        closePolicy: QQC2.Popup.CloseOnEscape | QQC2.Popup.CloseOnPressOutside
    }
}

