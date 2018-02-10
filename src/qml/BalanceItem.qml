import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

Item {
    property int amount: peersModel.totalAvailableFunds / 1000 + walletModel.totalAvailableFunds

    ColumnLayout {
        anchors.leftMargin: 50
        QQC2.Label {
            color: Kirigami.Theme.textColor
            font.pixelSize: 26
            text: (amount).toLocaleString(locale, 'f' , 0) + " SAT"
        }

        QQC2.Label {
            color: Kirigami.Theme.textColor
            text: ExchangeRate.getAmountInCurrency(amount)

        }
    }
}
