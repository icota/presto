import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import Qt.labs.settings 1.0
import "." // QTBUG-34418

Kirigami.OverlaySheet {

    ColumnLayout {
        Settings {
            id: settings
            property string currencyCode: currencyCodeTextField.text
        }


        QQC2.Label {
            wrapMode: Text.WordWrap
            color: Kirigami.Theme.textColor
            text: "Network: "
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            color: Kirigami.Theme.textColor
            font: fixedFont
            text: lightningModel.network
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            color: Kirigami.Theme.textColor
            text: "Your Node ID:"
        }

        QQC2.Label {
            id: idLabel
            wrapMode: Text.WordWrap
            color: Kirigami.Theme.textColor
            font: fixedFont
            text: lightningModel.id
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    clipboard.setText(idLabel.text)
                    showPassiveNotification(qsTr("Node ID Copied to Clipboard"))
                }
            }
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            color: Kirigami.Theme.textColor
            text: qsTr("Currency Code:")
        }

        QQC2.TextField {
            id: currencyCodeTextField
            font.capitalization: Font.AllUppercase
            wrapMode: Text.WordWrap
            text: settings.currencyCode
            inputMask: "AAA" // ISO 4217

            onTextChanged: {
                ExchangeRate.currencyCode = text.toUpperCase()
            }
        }
    }
}
