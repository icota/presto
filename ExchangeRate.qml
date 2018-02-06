import QtQuick 2.0
pragma Singleton

Item {
    id: wrapper

    // Insert valid consumer key and secret tokens below
    property string consumerKey : ""
    property string consumerSecret : ""
    property string bearerToken : ""

    property variant model: currencies
    property string from : ""
    property string phrase : ""

    property int status: XMLHttpRequest.UNSENT
    property bool isLoading: status === XMLHttpRequest.LOADING
    property bool wasLoading: false
    signal isLoaded

    ListModel { id: currencies }

    function encodePhrase(x) { return encodeURIComponent(x); }

    function reload() {
        currencies.clear()

        var req = new XMLHttpRequest;
        req.open("GET", "https://apiv2.bitcoinaverage.com/indices/global/ticker/BTCUSD");
        req.setRequestHeader("Authorization", "Bearer " + bearerToken);
        req.onreadystatechange = function() {
            status = req.readyState;
            if (status === XMLHttpRequest.DONE) {
                var objectArray = JSON.parse(req.responseText);
                if (objectArray.errors !== undefined)
                    console.log("Error fetching tweets: " + objectArray.errors[0].message)
                else {
                    for (var key in objectArray.statuses) {
                        var jsonObject = objectArray.statuses[key];
                        currencies.append(jsonObject);
                    }
                }
                if (wasLoading == true)
                    wrapper.isLoaded()
            }
            wasLoading = (status === XMLHttpRequest.LOADING);
        }
        req.send();
    }

    onPhraseChanged: reload();
    onFromChanged: reload();

    Component.onCompleted: {

        reload();
    }

}
