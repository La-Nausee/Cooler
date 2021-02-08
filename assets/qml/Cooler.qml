import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Universal 2.2

Item {
    id: root

    BusyIndicator {
        id:busy_indicator
        anchors.centerIn: parent
        running: true
        visible: false
    }
    Label{
        text: "Scanning"
        visible: busy_indicator.visible
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: busy_indicator.bottom
    }

    property bool connected: false
    signal componentTriggered(bool checked, string address)
    signal updatePage()

    onComponentTriggered: {
        if(!checked)
        {
            root.connected = cooller.connectToDevice(address)
            root.updatePage()
        }
        else
        {
            root.rePaint()
            root.connected = false
        }
    }

    function rePaint()
    {
        root.connected = cooller.isReady()
        sensor_model.clear()
        var allRemoteDevices = scanner.allRemoteDevices().split(",")
        for(var i=0; i< allRemoteDevices.length-1; i+=3)
        {
            sensor_model.append({"rssi":allRemoteDevices[i], "device_name": allRemoteDevices[i+1], "address":allRemoteDevices[i+2]})
        }
    }

    function initialize(){
        sensor_model.clear()
        console.log("request to scan")
        busy_indicator.visible = true
        scanner.start()
        busy_indicator.visible = false
        root.rePaint()
    }


    ListView{
        focus: true
        activeFocusOnTab: true
        cacheBuffer:100
        anchors.fill: parent
        visible: !busy_indicator.visible

        ListModel{
            id:sensor_model
        }

        model:sensor_model

        delegate: Rectangle{
            id:sensor_rct
            width:root.width
            height: column.height
            color: "transparent"
            signal clicked(bool checked, string address);
            Column{
                id:column
                height:row.height
                Row{
                    id:row
                    height:rssi_item.height
                    spacing: 0
                    Item{
                        id:rssi_item
                        width: rssi_rct.width + 10
                        height: rssi_rct.height + 10
                        Rectangle{
                            id:rssi_rct
                            anchors.centerIn: parent
                            width: (rssi_label.width > rssi_label.height ?  rssi_label.width : rssi_label.height ) + 20
                            height: width
                            radius: width/2
                            color: Qt.rgba((150.0 - parseInt(rssi))/255.0,(255.0 + parseInt(rssi))/255.0, 0.5, 1)
                            Label{
                                id:rssi_label
                                anchors.centerIn: parent
                                text: rssi
                            }
                        }
                    }

                    Item{
                        id:vline_item
                        width:5
                        height:rssi_item.height
                        Rectangle{
                            anchors.left: parent.left
                            height: parent.height
                            width: 1
                            color: "grey"
                        }
                    }

                    Item{
                        id:device_item
                        width: sensor_rct.width - vline_item.width - rssi_item.width
                        height: rssi_item.height
                        Column{
                            anchors.fill:parent
                            anchors.topMargin:  5
                            Item{
                                width: parent.width
                                height: parent.height/2
                                Label{
                                    id:name_label
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    font.bold: true
                                    text: device_name
                                }
                                Button{
                                    id:button
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.top: parent.top
                                    anchors.right: parent.right
                                    anchors.rightMargin: 5
                                    text: "Connect"
    //                                enabled: root.connected ? sensor_rct.checked : true
    //                                checked: !sensor_rct.checked
    //                                checkable: true
                                    onClicked: {
                                        if(root.connected)
                                        {
                                           root.connected = false
                                           cooller.disconnectFromDevice()
                                           root.rePaint()
                                        }
                                        else
                                        {
                                            root.connected  = cooller.connectToDevice(address)
                                            if(cooller.isReady())
                                            {
                                                root.rePaint()
                                                root.updatePage()
                                            }
                                        }
                                    }

                                    contentItem: Text {
                                        text: button.text
                                        font: button.font
                                        color: "white"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        elide: Text.ElideRight
                                    }

                                    background: Rectangle {
                                        implicitWidth: 70
                                        implicitHeight: 30
                                        opacity: enabled ? 1 : 0.3
                                        color: enabled ? button.down ? "darkgreen":"green" :"grey"
                                        radius: 15
                                    }

                                    Component.onCompleted: {
                                        if(cooller.isReady())
                                        {
                                            if(cooller.remoteAddress() === address_label.text)
                                            {
                                                button.enabled = true
                                                button.text = "Disconnect"
                                            }
                                            else
                                            {
                                                button.enabled = false
                                                button.text = "Connect"
                                            }
                                        }
                                        else
                                        {
                                            button.enabled = true
                                            button.text = "Connect"
                                        }
                                    }
                                }
                            }

                            Label{
                                id:address_label
                                text: address
                            }
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

            Component.onCompleted: clicked.connect(root.componentTriggered)
        }

        onFlickStarted: {
            console.log(contentY)
            console.log(originY)
            if ((contentY < originY) && (!root.connected))
            {
                var dy = contentY - originY;
                if (dy < -20){
                    root.initialize()
                }
            }
            else
            {
                root.rePaint()
            }
        }
    }

}
