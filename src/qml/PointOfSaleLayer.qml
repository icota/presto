import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.ScrollablePage {
    id: page
    Layout.fillWidth: true
    Layout.fillHeight: true

    property string invoiceWeAreWaitingFor

    Rectangle {
        anchors.fill: parent
        color: "black"
        MouseArea {
            anchors.fill: parent
            onClicked: {
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
    }

    QQC2.Label {
        Layout.alignment: Qt.AlignHCenter
        font.pixelSize: Kirigami.Units.gridUnit * 6
        visible: false
        text: "
    THANK YOU!"
    }

    function getRandomInvoice() {
        var amount = Math.floor(((Math.random()) * 9000 + 1000) * 1000)
        grandTotal.amount = amount
        invoiceWeAreWaitingFor = "INVOICE #" + Math.floor(Math.random()*10000)
        invoicesModel.addInvoice(invoiceWeAreWaitingFor,
                                 "6 Bananas, 2 Apples",
                                 amount,
                                 86400)
    }

    Connections {
        target: invoicesModel
        onInvoiceAdded: {
            invoicesModel.waitInvoice(invoiceWeAreWaitingFor)
            nfcHelper.setBolt11(bolt11)
            //bolt11 beamed to device
        }

        onErrorString: {
            //showPassiveNotification(error)
        }
    }
}
