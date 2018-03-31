import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

Item {
    property var delegate
    property int numTablesSelected: 0
    property var onInsert: function(){ return undefined; }
    property var model
    id: container
    ListView {
        anchors.fill: parent
        model: container.model
        delegate: container.delegate
        clip: true
    }
    RowLayout {
        id: buttons
        spacing: 2
        width: parent.width * 0.25
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 4
        height: 25
        Button {
            id: removeLedTable
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "-"
            font.pointSize: 14
            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 40
                color: removeLedTable.down ? "#d6d6d6" : "mistyrose"
                border.color: "#26282a"
                border.width: 1
                radius: 4
            }
            visible: numTablesSelected > 0
            onClicked: {
                for (var idx=0; idx<model.count; idx++) {
                    var item = model.get(idx);
                    if (item._currentlySelected) {
                        model.remove(idx, 1);
                        numTablesSelected--;
                        idx--;
                    }
                }
            }
        }
        Button {
            id: addLedTable
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "+"
            font.pointSize: 14
            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 40
                color: addLedTable.down ? "#d6d6d6" : "darkseagreen"
                border.color: "#26282a"
                border.width: 1
                radius: 4
            }
            onClicked: {
                var newvalue = onInsert();
                if (newvalue !== undefined) {
                    newvalue._currentlySelected = false;
                    model.append(newvalue);
                }
            }
        }
    }

    function toggleSelected(currentlySelected) {
        if (currentlySelected) {
            numTablesSelected++;
        } else {
            numTablesSelected--;
        }
        return currentlySelected;
    }
}
