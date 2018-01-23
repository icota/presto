import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    id: payInvoiceSheet

    QQC2.Label {
        wrapMode: Text.WordWrap
        text: "INVOICE INFO"
    }

    QQC2.Button {
        text: "Pay"
        onClicked: {
            // pay the invoice
        }
    }


}
