import QtQuick 1.0

Item {
    width: 200
    height: 100

    ListView {
        anchors.fill: parent;
        model:  ListModel {
            ListElement {
                name: "BMW"
                speed: 200
            }
            ListElement {
                name: "Mercedes"
                speed: 180
            }
            ListElement {
                name: "Audi"
                speed: 190
            }
            ListElement {
                name: "VW"
                speed: 180
            }
        }


        delegate:  Item {
            height:  40
            Row {
                spacing: 10
                Text {
                    text: name;
                    font.bold: true
                }

                Text { text: "speed: " + speed }
            }


        }
    }
}
