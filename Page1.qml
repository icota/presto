import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    ListView {
        id: paymentsListView
        model: paymentsModel
        width: 500
        height: 500
        //anchors.fill: parent
        delegate: Text { text: "Amount: " + msatoshi + ", " + incoming }
    }

    Button {
        id: debugButton
        text: "debug"
        visible: false
        onClicked: {
            console.log("damn girl")
        }
    }
}
