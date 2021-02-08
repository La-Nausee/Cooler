import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    width: 375
    height: 667
    visible: true
    property bool splash_visible: true

    Adapter{
        visible: splash_visible
        onFinished:{
            splash_visible =!splash_visible
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        visible: !splash_visible

        Cooler {
            id:cooler
            Timer{
                repeat: false
                interval: 100
                running: !splash_visible
                onTriggered:{
                    cooler.initialize()
                }
            }

            onUpdatePage: {
                swipeView.incrementCurrentIndex()
            }
        }

        Setup{
            onUpdatePage: {
                console.log("disconnected andquire to change view")
                cooler.rePaint()
                swipeView.decrementCurrentIndex()
            }
        }
    }

    footer:  Button{
                visible: !splash_visible
                enabled: !splash_visible
                text: qsTr("Exit")
                onClicked: {
                    cooller.disconnectFromDevice()
                    Qt.quit()
                }
    }
}
