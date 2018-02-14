import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

RowLayout {
    property bool milisatoshi: true
    property alias amount: amountTextField.text
    QQC2.TextField {
        id: amountTextField
        validator: IntValidator { bottom: 0; }
        selectByMouse: true
        placeholderText: milisatoshi ? qsTr("Enter Amount in satoshi") : qsTr("Enter Amount in milisatoshi")
    }
    
    QQC2.Label {
        id: currencyAmountLabel
        color: Kirigami.Theme.textColor
        wrapMode: Text.WordWrap
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: parent
        text: ExchangeRate.getAmountInCurrency(milisatoshi ? amountTextField.text : (amountTextField.text / 1000))
        font.pixelSize: 16
    }
}
