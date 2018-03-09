import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import Qt.labs.settings 1.0
import "." // QTBUG-34418

Kirigami.OverlaySheet {
    header:
        OverlaySheetHeader {
        text: qsTr("Preferences")
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }



    ColumnLayout {

        Settings {
            id: settings
            property string currencyCode: currencyCodeTextField.text
        }

        QQC2.Label {
            wrapMode: Text.Wrap
            text: qsTr("Currency Code:")
        }

        QQC2.TextField {
            id: currencyCodeTextField
            font.capitalization: Font.AllUppercase
            wrapMode: Text.Wrap
            text: settings.currencyCode
            inputMask: "AAA" // ISO 4217
            placeholderText: "USD"

            onTextChanged: {
                ExchangeRate.currencyCode = text.toUpperCase()
            }
        }
    }
}
