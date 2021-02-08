import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Universal 2.2

Rectangle{
    id:root
    enabled: true
    property bool first: true
    property string fan_cur_str: ""
    property string pel_cur_str: ""
    property string bat_lev_str: ""
    signal updatePage()
    Column{
        anchors.fill: parent
        spacing: 5
        Rectangle{
            height: root.height/20
            width: root.width
            border.color: "lightgray"
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: qsTr("LED: ")+ led_dial.value.toFixed(0) +"%"
            }
        }

        Rectangle{
            height: root.height/7
            width: root.width
            Dial{
                id:led_dial
                width: parent.height
                height: parent.height
                anchors.centerIn: parent
                from:0
                to:100
                value:0
                stepSize: 1
                onMoved: {
                    led_tmr.restart()
                }
                Timer{
                    id:led_tmr
                    repeat: false
                    running: false
                    interval: 100
                    onTriggered: {
                        cooller.setLEDPWM(led_dial.value)
                    }
                }
            }
        }

        Rectangle{
            height: root.height/20
            width: root.width
            border.color: "lightgray"
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: qsTr("PEL: ")+ pel_dial.value.toFixed(0) +"%"
            }
        }

        Rectangle{
            height: root.height/7
            width: root.width
            Dial{
                id:pel_dial
                width: parent.height
                height: parent.height
                anchors.centerIn: parent
                from:0
                to:100
                value:0
                stepSize: 1
//                live:false
                onMoved: {
                    pel_tmr.restart()
                }
                Timer{
                    id:pel_tmr
                    repeat: false
                    running: false
                    interval: 100
                    onTriggered: {
                        cooller.setPelPWM(pel_dial.value)
                    }
                }
            }
        }

        Rectangle{
            height: root.height/20
            width: root.width
            border.color: "lightgray"
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: qsTr("FAN: ")+ fan_dial.value.toFixed(0) +"%"
            }
        }

        Rectangle{
            height: root.height/7
            width: root.width
            Dial{
                id:fan_dial
                width: parent.height
                height: parent.height
                anchors.centerIn: parent
                from:0
                to:100
                value:0
                stepSize: 1
                onMoved: {
                    fan_tmr.restart()
                }
                Timer{
                    id:fan_tmr
                    repeat: false
                    running: false
                    interval: 100
                    onTriggered: {
                        cooller.setFanPWM(fan_dial.value)
                    }
                }
            }
        }

        Rectangle{
            height: root.height/20
            width: root.width
            border.color: "lightgray"
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: qsTr("Status Information")
            }
        }

        Rectangle{
            height: root.height/14
            width: root.width
            Row{
                anchors.fill: parent
                spacing: 0
                Rectangle{
                    height: parent.height
                    width: parent.width/2
                    Text {
                        id: fan_cur
                        anchors.centerIn: parent
                        text: qsTr("FAN: ") + root.fan_cur_str
                    }
                }
                Rectangle{
                    height: parent.height
                    width: parent.width/2
                    Text {
                        id: pel_cur
                        anchors.centerIn: parent
                        text: qsTr("PEL: ") + root.pel_cur_str
                    }
                }

            }
        }

        Rectangle{
            height: root.height/14
            width: root.width
            Row{
                anchors.fill: parent
                spacing: 0
                Rectangle{
                    height: parent.height
                    width: parent.width/2
                    Text {
                        id: bat_lev
                        anchors.centerIn: parent
                        text: qsTr("BAT: ") + root.bat_lev_str
                    }
                }
                Rectangle{
                    height: parent.height
                    width: parent.width/2
//                    Text {
//                        id: temp_val
//                        anchors.centerIn: parent
//                        text: qsTr("PEL: ") + root.pel_cur_str
//                    }
                }

            }
        }

        Button{
            width: parent.width
            height: parent.height/15
            text: qsTr("Shutdown")
            onClicked: {
                led_dial.value = 0;
                fan_dial.value = 0
                pel_dial.value = 0;
                root.pel_cur_str = ""
                root.fan_cur_str = ""
                root.bat_lev_str = ""
                root.enabled = false;
                cooller.shutdown()
                root.updatePage();
            }
        }

        Timer{
            running: true
            repeat: true
            interval: 1000
            onTriggered: {
                root.enabled = cooller.isReady()
                if(root.enabled)
                {
                    if(led_dial.value == 0)
                    {
                        led_dial.value = cooller.getLEDPWM()
                        pel_dial.value = cooller.getPelPWM()
                        fan_dial.value = cooller.getFanPWM()
                    }

                    root.pel_cur_str = cooller.getPelCur()
                    root.fan_cur_str = cooller.getFanCur()
                    root.bat_lev_str = cooller.getBatLevel()
                }
                else
                {
                    led_dial.value = 0;
                    fan_dial.value = 0
                    pel_dial.value = 0;
                    root.pel_cur_str = ""
                    root.fan_cur_str = ""
                    root.bat_lev_str = ""
                    root.enabled = false;
                }

            }
        }
    }
}
