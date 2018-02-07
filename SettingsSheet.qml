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
            text: "Network"
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: lightningModel.network
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: "Your Node ID:"
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: lightningModel.id
        }

        QQC2.Label {
            wrapMode: Text.WordWrap
            text: qsTr("Currency Code:")
        }

        QQC2.TextField {
            id: currencyCodeTextField
            // save this somewhere
            wrapMode: Text.WordWrap
            text: settings.currencyCode

            onTextChanged: {
                ExchangeRate.currencyCode = text
            }
        }
    }
}
