import QtQuick 2.7
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.1 as Kirigami

Kirigami.ScrollablePage {
    id: page
    Layout.fillWidth: true


    title: "Please Pay"

    QQC2.Label {
        text: "HELLLOOOO"
    }

    QQC2.Button {
        text: qsTr("Go Back")
        onClicked: {
            pageStack.layers.pop();
        }
    }

}
