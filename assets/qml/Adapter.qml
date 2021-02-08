import QtQuick 2.12
import QtQuick.Controls 2.5

ScrollView {
    id:root
    anchors.fill: parent
    signal finished()
    ListView {
        id: listView
        width: parent.width
        signal adapterSelected(string addr)

        onAdapterSelected: {
            console.log(addr)
            scanner.setLocalDevice(addr)
            root.finished()
        }

        ListModel{
            id:adapter_model
        }

        model: adapter_model
        delegate: Rectangle{
            id:delegate_rct
            anchors.left: parent.left
            anchors.right: parent.right
            height: column.height
            color: "transparent"
            signal selected( string address);
            Column{
                id:column
                height:row.height
                Row{
                    id:row
                    height:adapter_label_item.height
                    spacing: 0
                    Item{
                        id:adapter_label_item
                        width: adapter_label.width + 20
                        height: adapter_label.height + 20
                        Label{
                            id:adapter_label
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            text: "BLE Adapter " + i + " : " + addr
                            color: "blue"
                        }
                    }

                    Item{
                        width: delegate_rct.width - adapter_label_item.width
                        height: adapter_label_item.height
                        Button{
                            id:button
                            height: parent.height - 4
                            anchors.verticalCenter: parent.verticalCenter
//                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.rightMargin: 0
                            text: "Select"
                            hoverEnabled: true
                            onClicked: {
                                delegate_rct.selected(addr)
                            }
                            contentItem: Text {
                                text: button.text
                                font: button.font
                                color: "black"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }

//                            background: Rectangle {
//                                implicitWidth: 60
//                                implicitHeight: 10
//                                opacity: 1
//                                color: button.down ? "darkgreen":"green"
//                                radius: 5
//                            }
                        }
                    }

                }

                Rectangle{
                    id:hline_rct
                    height: 1
                    width: parent.width
                    color: "grey"
                }
            }

            Component.onCompleted: selected.connect(listView.adapterSelected)
        }
        Component.onCompleted: {
            var allLocalDevices = scanner.allLocalDevices().split(",")
            if(allLocalDevices.length  <= 2)
            {
                scanner.setLocalDevice(allLocalDevices[0])
                root.finished()
            }
            else
            {
                for(var i=0; i<allLocalDevices.length; i+=2)
                {
                    adapter_model.append({"i": i/2, "addr":allLocalDevices[i]})
//                    adapter_model.append({"i": i/2, "addr":allLocalDevices[i]})
                }
            }

            console.log(allLocalDevices.length)
        }
    }
}
