import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Page {
    ListView {
        id: peersListView
        model: paymentsModel
        delegate: Text { text: "Amount: " + msatoshi + ", " + incoming }
    }
}
