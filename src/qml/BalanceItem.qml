import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
import "." // QTBUG-34418

ColumnLayout {
    property int amount: peersModel.totalAvailableFunds / 1000 + walletModel.totalAvailableFunds
    QQC2.Label {
        
        font.pixelSize: 26
        text: (amount).toLocaleString(locale, 'f' , 0) + " SAT"
    }

    QQC2.Label {
        
        text: lightningModel.connectedToDaemon ? ExchangeRate.getAmountInCurrency(amount) : qsTr("Disconnected")

    }
}

