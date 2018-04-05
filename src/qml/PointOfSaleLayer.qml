import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.ScrollablePage {
    id: page
    Layout.fillWidth: true
    Layout.fillHeight: true

    property string invoiceWeAreWaitingFor
    property string demoInvoiceDescription: "<img src=https://sites.google.com/a/codexapertus.com/www/home/LogoMakr_6tvqhe.png>" +
                                            "<table><tr><th>Item</th><th>Qty</th></tr>" +
                                            "<tr><td>Bananas</td><td>6</td><td>$2.49</td></tr>" +
                                            "<tr><td>Apples</td><td>10</td><td>$3.49</td></tr>" +
                                            "<tr><th></th><th>TOTAL</th><th>$5.98</th></tr></table>"

    Rectangle {
        anchors.fill: parent
        color: "black"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                demoReceipt.visible = true
                demoReceiptPaidText.visible = false
                getRandomInvoice()
            }
        }
    }


    title: "Contactless Point of Sale"

    PointOfSaleTotalAmount {
        id: grandTotal
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        amount: 0
        visible: false
    }

    QQC2.Label {
        id: demoReceipt
        Layout.alignment: Qt.AlignHCenter
        font.pixelSize: Kirigami.Units.gridUnit * 2
        textFormat: Text.RichText
        visible: true
        text: demoInvoiceDescription
    }

    QQC2.Label {
        id: demoReceiptPaidText
        Layout.alignment: Qt.AlignHCenter
        font.pixelSize: Kirigami.Units.gridUnit * 6
        visible: false
        text: "
    THANK
    YOU!"
    }

    function getRandomInvoice() {
        var amount = Math.floor(((Math.random()) * 9000 + 1000) * 1000)
        grandTotal.amount = amount
        invoiceWeAreWaitingFor = "INVOICE #" + Math.floor(Math.random()*10000)
        invoicesModel.addInvoice(invoiceWeAreWaitingFor,
                                 invoiceWeAreWaitingFor +
                                 demoInvoiceDescription,
                                 amount,
                                 86400)
    }

    Connections {
        target: invoicesModel
        onInvoiceAdded: {
            nfcHelper.setBolt11(bolt11)
            invoicesModel.waitInvoice(invoiceWeAreWaitingFor)
        }

        onInvoiceStatusChanged: {
            demoReceipt.visible = false
            demoReceiptPaidText.visible = true
        }

    }

    Component.onCompleted: {
        getRandomInvoice()
    }
}
