import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

RowLayout {
    property bool milisatoshi: true
    property alias amount: amountSpinBox.value
    QQC2.SpinBox {
        id: amountSpinBox
        Layout.fillWidth: true
        editable: true
        from: 0
        to: 10000 * 100000
        stepSize: milisatoshi ? 10000000 : 10000
        textFromValue: function(value, locale) {
            return milisatoshi ? value + qsTr(" mSAT") : value + qsTr(" SAT");
        }
    }
    
    QQC2.Label {
        id: currencyAmountLabel
        
        wrapMode: Text.Wrap
        Layout.alignment: Qt.AlignCenter
        text: ExchangeRate.getAmountInCurrency(milisatoshi ? (amountSpinBox.value / 1000) : amountSpinBox.value)
        font.pixelSize: 16
    }
}
