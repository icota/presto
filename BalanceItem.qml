import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

Item {
    ColumnLayout {
        QQC2.Label {
            font.pixelSize: 22
            text: (peersModel.totalAvailableFunds + walletModel.totalAvailableFunds).toLocaleString(locale, 'f' , 0)
                  + " SAT"
        }
        QQC2.Label {
            text: ((peersModel.totalAvailableFunds + walletModel.totalAvailableFunds) *
                  ExchangeRate.rate).toLocaleString(locale, 'f' , 2) + " " + ExchangeRate.currencyCode

        }
    }
}
