import QtQuick 2.7
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami
//import "." // QTBUG-34418

Item {
    id: msatoshiAmountItem
    property int amount: 0
    
    ColumnLayout {
        QQC2.Label {
            color: Kirigami.Theme.textColor
            font.pixelSize: 26
            text: (parent.amount).toLocaleString(locale, 'f' , 0) + " mSAT"
        }
        
        QQC2.Label {
            color: Kirigami.Theme.textColor
            text: ExchangeRate.getAmountInCurrency(amount * 1000)
            
        }
    }
}
