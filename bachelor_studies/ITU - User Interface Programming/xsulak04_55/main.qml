import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import io.thp.pyotherside 1.3

ApplicationWindow {
    title: qsTr("Wi-Fi locator")
    id: mainWindow

    width: 720
    height: 860
    color: "black"

    property bool menu_shown: false
    property string mapSource: "map/FIT VUT/map1.png" //default map

    /* this rectangle contains the "menu" */
    Rectangle {
        id: menuView
        anchors.fill: parent
        border.color: "black"
        border.width: 2
        radius: 10
        //color: "#303030";

        gradient: Gradient {
            GradientStop { position: 0.2;
            color: "#09556D"
            }
            GradientStop { position: 1.0;
            color: "#06061B"
            }
        }
        Image {
            id: logo
            rotation: 25;
            width: 200;
            height:200;
            x: 10
            source: "img/wifi-logo.png"
            smooth: true
        }
        opacity: mainWindow.menu_shown ? 1 : 0

        Behavior on opacity { NumberAnimation { duration: 300 } }

        /* this is my sample menu content */
        ListView {
            anchors { fill: parent; margins: 5;  topMargin: parent.height/3; }
            model: ListModel {
                ListElement {
                    label: "Choose Wi-Fi"
                    source: "ScanPage"
                }
                ListElement {
                    label: "Choose Map"
                    source: "chooseMap"
                }
                ListElement {
                    label: "Map"
                    source: "MapPage"
                }
                ListElement {
                    label: "About"
                    source: "About"
                }
                ListElement {
                    label: "Quit"
                    source: "Quit"
                }
            }
            spacing: 0

            delegate: Rectangle {
                height: 40;
                width: parent.width - 44;
               // color: "#009900"
                gradient: Gradient {
                    GradientStop { position: 0.0
                    color: "white"
                    }
                    GradientStop { position: 0.05
                    color: "#747c82"
                    }
                    GradientStop { position: 1
                    color: "#233a47"//06061B better?????????
                    }
                }
                border.color: "black"
                border.width: 1
                //border.top: 0
               // radius: 1

                Text {
                    anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
                    color: "white";
                    font.pixelSize: 20;
                    text: label;
                    font.bold: true;
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        //console.log("Loading " + source);
                        normalView.currentPage = source
                        mainWindow.onMenu();
                    }
                }
            }
        }
    }
    /* this rectangle contains the "normal" view in your app */
    Rectangle {
        id: normalView
        anchors.fill: parent
        color: "#09556D"

        gradient: Gradient {
            GradientStop { position: 0.2
            color: "#09556D"
            }
            GradientStop { position: 1.0
            color: "#06061B"
            }
        }
        /* this is what moves the normal view aside */

        transform: Translate {
            id: normalViewTranslate
            x: 0
            Behavior on x { NumberAnimation { duration: 400; easing.type: Easing.OutQuad } }
        }

        /* quick and dirty menu "button" for this demo (TODO: replace with your own) */
        Rectangle {
            id: topPanel

            color:parent.color
            width: parent.width
            height: Math.min(parent.height / 64 + 25, 40)

             x: 15

//            id: menu_bar_
            anchors.top: parent.top
            Rectangle {
                id: menu_button
                Image {
                    id: menuIcon
                    width: 25;
                    height:25;
                   // x: 15
                    anchors.verticalCenter: parent.verticalCenter

                    source: "img/home.png"
                    smooth: true
                }
                Text{
                    id: menuLabel
                    text : "Menu"
                    anchors.left: menuIcon.right
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 20;
                    font.bold: true;
                    color: "white"
                }
                anchors {left: parent.left; verticalCenter: parent.verticalCenter; margins: 2 }
                color: parent.color; implicitWidth: menuIcon.width + menuLabel.width; height: parent.height ; smooth: true
                scale: ma_.pressed ? 1.2 : 1
               // border.color: "black"
               // border.width: 2


               // Text { id: menuLabel; anchors.centerIn: parent; anchors.margins: 10; font.pixelSize: 48; fontSizeMode: Text.VerticalFit; text: "" }
                MouseArea { id: ma_; anchors.fill: parent; onClicked: mainWindow.onMenu(); }
            }
        }

        //Put the name of the QML files containing your pages (without the '.qml')
        property variant pagesList  : [
            "ScanPage",
            "chooseMap",
            "MapPage",
            "About",
            "Quit"
        ];

        // Set this property to another file name to change page
        property string  currentPage : "ScanPage";

        Repeater {
            model: normalView.pagesList;
            delegate: Loader {
                active: false;
                asynchronous: true;
                anchors.top: topPanel.bottom
                anchors { top: topPanel.bottom; bottom: parent.bottom; rightMargin: 20; leftMargin: 20; bottomMargin: 20;left: parent.left; right: parent.right; }
                visible: (normalView.currentPage === modelData);
                source: "%1.qml".arg(modelData)
                onVisibleChanged:      { loadIfNotLoaded(); }
                Component.onCompleted: { loadIfNotLoaded(); }

                function loadIfNotLoaded () {
                    // to load the file at first show
                    if (visible && !active) {
                        active = true;
                    }
                }
            }
        }
        /* put this last to "steal" touch on the normal window when menu is shown */
        MouseArea {
            anchors.fill: parent
            enabled: mainWindow.menu_shown
            onClicked: mainWindow.onMenu();
        }
        Rectangle {
            anchors.fill: parent
            opacity: mainWindow.menu_shown ? 0.4 : 0
            Behavior on opacity { NumberAnimation { duration: 300 } }
            color: "#000"
        }

    }

    /* this functions toggles the menu and starts the animation */
    function onMenu()
    {
        normalViewTranslate.x = mainWindow.menu_shown ? 0 : Math.min(Math.max(mainWindow.width * 0.3, 200), 250)
        mainWindow.menu_shown = !mainWindow.menu_shown;
    }
}
