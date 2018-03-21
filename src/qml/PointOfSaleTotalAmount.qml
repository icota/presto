import QtQuick 2.7
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

ColumnLayout {
    property int amount: 0
    QQC2.Label {
        font.pixelSize: Kirigami.Units.gridUnit * 10
        text: (amount/1000).toLocaleString(locale, 'f' , 0) + " SAT"
    }

    QQC2.Label {
        Layout.alignment: Qt.AlignCenter
        font.pixelSize: Kirigami.Units.gridUnit * 6
        text: ExchangeRate.getAmountInCurrency(amount / 1000)
    }
}
