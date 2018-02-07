pragma Singleton
import QtQuick 2.0

Item {
    id: exchangeRate

    // Get this from main window
    property var locale
    property string cryptoCurrencyCode: "BTC"
    property string currencyCode: "USD"
    property real rate: 0.0

    // Update every hour
    property int updateInterval: 360 * 1000

    Timer {
        interval: updateInterval; running: true; repeat: true
        onTriggered: reload()
    }

    // Stuff for API access
    property string consumerKey : ""
    property string consumerSecret : ""
    property string bearerToken : ""


    property int status: XMLHttpRequest.UNSENT
    property bool isLoading: status === XMLHttpRequest.LOADING
    property bool wasLoading: false
    signal isLoaded

    function reload() {
        var req = new XMLHttpRequest;
        req.open("GET",
                 "https://apiv2.bitcoinaverage.com/indices/global/ticker/" +
                 cryptoCurrencyCode +
                 currencyCode);

        req.onreadystatechange = function() {
            status = req.readyState;
            if (status === XMLHttpRequest.DONE) {
                var objectArray = JSON.parse(req.responseText);
                if (objectArray.errors !== undefined)
                    console.log("Error fetching exchange rate: " +
                                objectArray.errors[0].message)
                else {
                    rate = objectArray.last
                }
                if (wasLoading == true)
                    exchangeRate.isLoaded()
            }
            wasLoading = (status === XMLHttpRequest.LOADING);
        }
        req.send();
    }

    onCryptoCurrencyCodeChanged: {
        if (cryptoCurrencyCode.length == 3)
        reload();
    }

    onCurrencyCodeChanged: {
        if (currencyCode.length == 3)
        reload();
    }

    Component.onCompleted: {
        reload();
    }

    function getAmountInCurrency(amountCrypto){
        // Convert weird JS values to zero
        amountCrypto = amountCrypto || 0
        return ((amountCrypto * rate).toLocaleString(locale, 'f' , 2)) + " " + currencyCode
    }
}
