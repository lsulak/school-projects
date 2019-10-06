import QtQuick 2.3
import QtQuick.Controls 1.2

Rectangle {
    id: mapPage
    anchors.fill: parent

    gradient: Gradient {
        GradientStop { position: 0.15
        color: "#09556D"
        }
        GradientStop { position: 1.05
        color: "#06061F"
        }
    }

    Flickable {
        id: mapImageFrame
        clip:true
        width: mapImage.width
        height: mapImage.height
        anchors.fill: parent
        //anchors.margins: 0

        contentWidth: mapImage.width * mapImage.scale
        contentHeight: mapImage.height * mapImage.scale

        boundsBehavior: Flickable.DragAndOvershootBounds
        //var wifiX, wifiY;

        Image {
            id: mapImage
            fillMode: Image.PreserveAspectFit
            //scale:  1.0
            cache: false
            source: mainWindow.mapSource
            smooth: true
            //width: parent.width;
            //height: parent.height;

            y: height * (scale - 1) / 2
            x: width * (scale - 1) / 2

            property double markScale: 1.0

            MouseArea {
                anchors.fill: parent
                //drag.target: mapPage2

                acceptedButtons: Qt.LeftButton
                //drag.axis: Drag.XAndYAxis

                onWheel: {
                    //console.log(Map2share.shared, Map2share.getMap())
                    //console.log("!",mapPage2.x/mapPage2.scale );
                    var newScale = mapImage.scale + mapImage.scale * wheel.angleDelta.y / 120 / 10;
                    mapImage.scale = (newScale < 1) ? 1 : newScale;
                    mapImage.markScale -= mapImage.markScale * wheel.angleDelta.y / 2400 ;
                    mapImage.markScale = (mapImage.markScale > 1) ? 1 : mapImage.markScale;
                    //if(newScale < 5)
                      //  mapPage2.scale = (newScale < 1 ) ? 1 : newScale;
               }

                onClicked: {

                    if (mouse.button == Qt.LeftButton)
                    {
                        //console.log(mouse.x, youMark.width, mapImage.markScale / mapImage.scale)
                        youMark.x = mouse.x - youMark.width * mapImage.markScale / mapImage.scale
                        youMark.y = mouse.y// - youMark.height * (1 - mapImage.markScale)
                        //wifiX = mouse.x
                        //wifiY = mouse.y
                        youMark.visible = true
                    }
                }
            }

            Image {
                id: youMark
                width: 30;
                height:35;
                visible:false;
                scale: mapImage.markScale
                source: "img/you.png"
                mipmap: true

            }
         }
    }
}
