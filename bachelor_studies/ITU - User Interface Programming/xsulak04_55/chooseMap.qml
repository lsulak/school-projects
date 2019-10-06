import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Qt.labs.folderlistmodel 2.1

Rectangle {
    id: chooseMap
    anchors.fill: parent
   // anchors.topMargin: topPanel.height + anchors.margins
    width: parent.width
    height: parent.height

    property int baseControlHeight
    baseControlHeight: 40

    property int baseFontHeight
    baseFontHeight: baseControlHeight * 0.4

    //border.color: "white"
    //border.width: 1
    //radius: 5

    property variant floor;
    property variant location;

    gradient: Gradient {
        GradientStop { position: 0.15
        color: '#09556D' //"#11A6D4"
        }
        GradientStop { position: 1.05
        color: "#06061B"
        }
    }

    /* Header - first row of table*/
    Rectangle {
        id: mapListHeaderHeader
        height: baseControlHeight
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: 10;}
        color: "#111111"

        property string textColor;
        textColor: "white"

        property string mainColor;
        mainColor: "#d16d1d"

        Rectangle {
            height: parent.height
            width: parent.width
            gradient: Gradient {
                GradientStop { position: 0.0; color: "white" }
                GradientStop { position: 1.0; color: "black" }
            }
            opacity: 0.02
        }
        Rectangle {
            width: 8
            height: parent.height
            color: mapListHeaderHeader.mainColor
        }

        /* Location - name of folder, Floor - from map file, can be negative or positive */
        RowLayout {
            height: parent.height
            anchors.left: parent.left
            anchors.right:  parent.right
            anchors.leftMargin: height / 2
            anchors.rightMargin: height / 2
            Text {
                text: "Location"
                Layout.preferredWidth: parent.width / 4
                font.pixelSize: baseFontHeight
                color: mapListHeaderHeader.textColor
            }
            Text {
                text: "Floor"
                Layout.preferredWidth: parent.width / 4
                visible: (parent.width / 4 > 60)
                font.pixelSize: baseFontHeight
                color: mapListHeaderHeader.textColor
            }

            /* Button (picture) for opening main (parent) folder */
            Rectangle {
                id:back_button
                Image {
                    id: backLabel
                    width: 40;
                    height:40;
                    source: "img/folderup.jpg"
                    smooth: true
                }
                anchors {
                    right: parent.right; verticalCenter: parent.verticalCenter; margins: 10
                }
                implicitWidth: backLabel.width; height: parent.height ; smooth: true
                scale: ma_.pressed ? 1.2 : 1
                MouseArea { id: ma_; anchors.fill: parent;
                    onClicked:
                        if(Qt.LeftButton == mouse.button)
                            folderModel.folder = "map/";
                }
            }
        }
    }
    /* List of files beginning with prefix 'map' */
    FolderListModel {
        id: folderModel
        folder: "map"
        nameFilters: ["map*"]
    }

    /* List of map files and folders */
    ListView {
        id: mapList
        spacing: 4
        anchors { top: mapListHeaderHeader.bottom; left: parent.left; right: parent.right; bottom: parent.bottom; margins: 10; topMargin: 15 }
        clip: true
        model: folderModel

        /* Show list of map files and folders */
        delegate: Rectangle {
            width: mapList.width
            height: baseControlHeight
            color: "#111111"
            clip: true

            property string textColor;
            textColor: "white"

            property string blueColor;
            blueColor: "#111c29"

            property bool detailShown;
            detailShown: false

            /* Click on the whole row */
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    if(Qt.LeftButton == mouse.button && fileName.substring(0,3) != "map")
                        folderModel.folder  += "/" + fileName;

                    else if(Qt.LeftButton == mouse.button && fileName.substring(0,3) == "map")
                    {
                        folderModel.folder = "map/";
                        mainWindow.mapSource = "map/" + location + "/" + fileName;
                        //mainWindow.onMenu();
						normalView.currentPage = "MapPage";
                    }
                }
            }
            /* Parsing file name, or folder name*/
            Component.onCompleted: {
                var tmp, number;
                tmp = fileName.substring(3);
                number = tmp.indexOf(".");
                tmp = tmp.substring(0, number);

                if(tmp[0] == '-')
                {
                    tmp = tmp.substring(1);
                    if(tmp == "1")
                        tmp = "Basement -1st";
                    else if(tmp == "2")
                        tmp = "Basement -2nd";
                    else if(tmp == "3")
                        tmp = "Basement -3rd";
                    else if(tmp >= "4")
                        tmp = "Basement -"+tmp+"nd";
                }
                else
                {
                    if(tmp == "1")
                        tmp = "1st Floor";
                    else if(tmp == "2")
                        tmp = "2nd Floor";
                    else if(tmp == "3")
                        tmp = "3rd Floor";
                    else if(tmp >= "4")
                        tmp = tmp+"th Floor";
                }
                floor = tmp;
                floorText.text = floor;

                if(fileName.substr(0,3) == "map")
                {
                    var tmpC = filePath.lastIndexOf('/');
                    location = filePath.substring(0, tmpC);

                    tmpC = location.lastIndexOf('/');
                    location = location.substring(tmpC+1);

                    mainWindow.mapSource = "map/" + location + "/" + fileName; //By clicking a new map will be loaded
                }

                else
                    location = fileName;

                locText.text = location;
             }

            Rectangle {
                height: parent.height
                width: parent.width
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "white" }
                    GradientStop { position: 1.0; color: "black" }
                }
                opacity: 0.02
            }

            Rectangle {
                width: 8
                height: parent.height
                color: Qt.lighter(blueColor, 2)
            }

            /* And finally there is drawing of rest of the table */
            RowLayout {

                height: parent.height
                anchors.left: parent.left
                anchors.right: detailButton.left
                anchors.leftMargin: height / 2
                anchors.rightMargin: height / 2
                Text {
                    id: locText
                    text: loc
                    Layout.preferredWidth: parent.width / 4
                    font.pixelSize: baseFontHeight
                    color: textColor
                }
                Text {
                    id: floorText
                    text: floor
                    Layout.preferredWidth: parent.width / 3
                    visible: (parent.width / 4 > 60)
                    font.pixelSize: baseFontHeight
                    color: textColor
                }
            }
            /* Button, mouse click on folder - go inside, or mouse click on file - choosing map */
            Rectangle {
                id: detailButton
                height: parent.height
                width: Math.max(parent.parent.width / 8, label.implicitWidth + height)
                color: blueColor
                anchors.right: parent.right
                border.color: "black"
                border.width: 1
                Text {
                    id: label;
                    text: "Select"
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: baseFontHeight

                }
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: parent.height / 10
                    color: Qt.lighter(parent.color, 2)
                }
                Rectangle {
                    height: parent.height
                    width: parent.width

                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "white" }
                        GradientStop { position: 1.0; color: "black" }
                    }
                    opacity: 0.04
                }


                /* Click on the button. If folder - go inside, if file - load new map */
                MouseArea {
                    anchors.fill: parent
					hoverEnabled: true
                    onClicked: {
                        if(Qt.LeftButton == mouse.button && fileName.substring(0,3) != "map")
                            folderModel.folder  += "/" + fileName;

                        else if(Qt.LeftButton == mouse.button && fileName.substring(0,3) == "map")
                        {
                            folderModel.folder = "map/";
                            mainWindow.mapSource = "map/" + location + "/" + fileName;
                            //mainWindow.onMenu();
                            normalView.currentPage = "MapPage";
                         }
                    }
                }
            }
        }

        populate: Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
        }
    }
}
