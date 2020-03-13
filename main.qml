import QtQuick 2.12
import QtQuick.Window 2.12
import QtLocation 5.9
import QtPositioning 5.8

Window {
    /*필수 윈도우 속성값들*/
    visible: true
    width: 512
    height: 512

    Plugin{
        /*Api관련 값인것 같음*/
        id: mapPlugin
        name: "osm"
    }


    Map{
        id:map
        /*위도, 경도 좌표 default 값 x,y좌표*/
        property var xLati: 35.0858
        property var yLongi: 129.0709
        /*화면에 채움*/
        anchors.fill: parent
        /*지도 연결*/
        plugin:mapPlugin
        /*디버깅시 지도 시작 default Coordinate 값*/
        center: QtPositioning.coordinate(xLati,yLongi)
        /*디버깅시 지도확대 정도 값*/
        zoomLevel: 16
        //bearing: 20 회전값


        /*맵 중앙 십자표시*/
        Rectangle{
            width:9
            height:1
            color: "red"
            /*십자가 중앙정렬*/
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
        Rectangle{
            width:1
            height:9
            color: "red"
            /*십자가 중앙정렬*/
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }



    /*좌측 위도경도 표시 레이어*/
    Rectangle{
        id:point
        width:180
        height:130
        /*투명도*/
        opacity:0.8
        /*테두리*/
        border.width:1

        Text { //위도
            id:lat
            x:30
            y:20
            font.pixelSize:20
            font.bold: true
            text: qsTr("위도")
        }

        TextEdit{ //위도 입력폼
            id:latitude
            x: 75
            y: 28
            width: 100
            height: 20
            focus: true
            text:""
        }

        Text { //경도
            id:longi
            x: 30
            y: 60
            font.pixelSize: 20
            font.bold: true
            text: qsTr("경도")
        }

        TextEdit{ //경도 입력폼
            id:longitude
            x: 75
            y: 68
            width: 100
            height: 20
            text:""
        }


        /*위도 경도를 입력했을 때 해당 위치로 대려다 주는 시그널*/
        signal showPlace(variant coordinate)

        /*마우스 이벤트*/
        MouseArea{
            anchors.fill: parent

            /*마우스 핸들러*/
            onClicked:{
                var coordinate = QtPositioning.coordinate(parseFloat(latitude.text), parseFloat(longitude.text));

                if(coordinate.isValid) {
                    showPlace(coordinate)
                }
            }
        }
    }
}
