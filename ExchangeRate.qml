pragma Singleton
import QtQuick 2.0

Item {
    id: exchangeRate

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

    onCryptoCurrencyCodeChanged: reload();
    onCurrencyCodeChanged: reload();

    Component.onCompleted: {
        reload();
    }
}
