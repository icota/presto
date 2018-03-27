import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.OverlaySheet {
    onSheetOpenChanged: {
        if (sheetOpen) {
        }
        else {
            resetSheet()
        }
    }

    header:
        OverlaySheetHeader {
        text: qsTr("Create Invoice")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }

    ColumnLayout {
        QQC2.TextField {
            id: labelTextField
            placeholderText: qsTr("Invoice Label")
        }

        QQC2.TextField {
            id: descriptionTextField
            placeholderText: qsTr("Description")
        }

        AmountTextField {
            milisatoshi: false
            id: amountTextField
            Layout.topMargin: 25
        }

        QQC2.Button {
            Layout.topMargin: 25
            text: qsTr("Create")
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            onClicked: {
                invoicesModel.addInvoice(labelTextField.text,
                                         descriptionTextField.text,
                                         amountTextField.amount * 1000,
                                         86400) // Number of seconds in a day
            }
        }
    }

    function resetSheet () {
        labelTextField.text = ""
        descriptionTextField.text = ""
        amountTextField.amount = 0
    }
}
