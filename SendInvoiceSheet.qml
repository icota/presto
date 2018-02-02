import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    ColumnLayout {
        QQC2.Label {
            wrapMode: Text.WordWrap
            text: "INVOICE SOMEBODY"
        }

        QQC2.TextField {
            id: labelTextField
            placeholderText: qsTr("Enter Label")
        }

        QQC2.TextField {
            id: descriptionTextField
            placeholderText: qsTr("Enter Description")
        }

        QQC2.TextField {
            id: amountTextField
            validator: IntValidator {bottom: 0;}
            placeholderText: qsTr("Enter Amount in milisatoshi")
        }


        QQC2.Button {
            text: "Pay"
            onClicked: {
                invoicesModel.addInvoice(labelTextField.text,
                                         descriptionTextField.text,
                                         amountTextField.text)
            }
        }
    }
}
