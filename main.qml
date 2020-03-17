import QtQuick 2.12
import QtQuick.Window 2.12
import QtLocation 5.9
import QtPositioning 5.8
import QtQuick.Controls 2.13 //Button사용
import QtQuick.Dialogs 1.2
import QtQuick.Shapes 1.12 //핀모양 만들기

Window {
    /*윈도우 속성값들*/
    visible: true
    width: 512
    height: 512

    Plugin{
        /*Api 관련 값인 것 같음*/
        id: mapPlugin
        name: "osm"
    }

    Map{
        id:map
        /*위도, 경도 좌표 default 값 x,y좌표*/
        property var xLati : 35.0860
        property var yLongi : 129.0711
        /*화면에 채움*/
        anchors.fill: parent
        /*지도 연결*/
        plugin:mapPlugin
        /*디버깅시 지도 시작 default Coordinate 값*/
        center: QtPositioning.coordinate(xLati,yLongi)
        /*디버깅시 지도확대 정도 값*/
        zoomLevel: 16

        Text {
            anchors.right: parent.right
            anchors.rightMargin: 30
            text: qsTr("왼쪽  더블클릭 시 확대\n오른쪽 더블클릭 시 축소\n길게 누르면 핀생성\n현재 줌레벨 : "+ (map.zoomLevel).toFixed(4))
            color: "grey"
            font.pixelSize: 16
            opacity: 0.9
            z: parent.z + 10
        }

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
        /*마우스 이벤트 공간*/
        MouseArea{
            id:ma //에마우스 공간 id 이름
            anchors.fill: parent // 마우스이벤트 범위을 map에 가득 채움

            /*마우스 버튼 영역에서 반응을 가능하게 하는 속성*/
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            /* 클릭 했을 시 이벤트 핸들러 */
            onClicked:  {
                /* 마우스 포인터 지점의 x, y 값의 위도경도 좌표값을 crd 변수에 대입 */
                var crd = map.toCoordinate(Qt.point(mouseX, mouseY))
                /* 아래에 있는 textEdit의 text값에 위 좌표값의 각각 경도와 위도값을 toFixed(4)를 이용하여*/
                /* 소수점 4자리 까지 출력 */
                latitudeE.text = (crd.latitude).toFixed(4)
                longitudeE.text = (crd.longitude).toFixed(4)
                /* 위도 경도 값을 콘솔창에 소수점 15자리(최대 출력 가능한 소수점)까지 출력 */
                console.log("위도: "+crd.latitude, "경도: "+crd.longitude)
            }

            /*더블 클릭 했을 시 이벤트 핸들러*/
            onDoubleClicked:{
                /* 왼쪽 버튼 더블클릭 했을 시*/
                if(mouse.button == Qt.LeftButton){
                    /*지도 확대 1값 만큼 확대*/
                    map.zoomLevel = map.zoomLevel + 2
                }
                /* 오른쪽 버튼 더블클릭 했을 시 */
                else if(mouse.button == Qt.RightButton)
                    /* 지도 축소 1값 만큼 축소 */
                    map.zoomLevel = map.zoomLevel - 2
            }
            /*길게 누를 시 핀 생성*/
            onPressAndHold: {
                /*마우스 위치값을 변수 crd에 대입*/
                var crd = map.toCoordinate(Qt.point(mouseX, mouseY))
                console.log(crd)
                /*있던 핀 제거*/
//                markerModel.clear()
                /*위치에 핀 표시*/
                markerModel.append({ "latitude": crd.latitude, "longitude": crd.longitude })
                latitudeE.text = (crd.latitude).toFixed(4)
                longitudeE.text = (crd.longitude).toFixed(4)
            }
        }

        /*핀생성*/
        MapItemView {
            id: mivMarker

            /*핀 형태*/
            model: ListModel {
                id: markerModel
            }

            delegate: Component {
                MapQuickItem {
                    coordinate: QtPositioning.coordinate(latitude, longitude)
                    anchorPoint: Qt.point(e1.width * 0.5, e1.height + slideIn)
                    property real slideIn : 0
                    sourceItem: Shape {
                        id: e1
                        vendorExtensionsEnabled: false
                        width: 15
                        height: 15
                        visible: true

                        /*위아래 좌표값*/
                        transform: Scale {
                            origin.y: e1.height * 0.5
                            yScale: -1
                        }

                        ShapePath {
                            id: c_sp1

                            /*핀 테두리 두께*/
                            strokeWidth: -1

                            /* 핀 색상 */
                            fillColor: "red"

                            /*마우스 포인트 끝지점 오차계산*/
                            /*상하 오차값*/
                            property real half: e1.width * 0.5
                            /*좌우 오차값*/
                            property point center: Qt.point(e1.x + e1.width * 0.5 , e1.y + e1.height * 0.5)

                            /*핀 모양 계산 값*/
                            property point top: Qt.point(center.x, center.y - half )
                            property point bottomLeft: Qt.point(center.x - half, center.y + half )
                            property point bottomRight: Qt.point(center.x + half, center.y + half )

                            startX: center.x;
                            startY: center.y + half

                            /*핀모양 (그리기)생성*/
                            PathLine { x: c_sp1.bottomLeft.x; y: c_sp1.bottomLeft.y }
                            PathLine { x: c_sp1.top.x; y: c_sp1.top.y }
                            PathLine { x: c_sp1.bottomRight.x; y: c_sp1.bottomRight.y }
                            PathLine { x: c_sp1.center.x; y: c_sp1.center.y + c_sp1.half }
                        }
                    }
                }
            }
        }

        /*좌측 위도경도 표시 레이어*/
        Rectangle{
            id:layer
            width:250
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
                text: qsTr("위도 : ")
            }

            TextEdit{ //위도좌표
                id:latitudeE
                x: 82
                y: 27
                z: 2
                width: 175
                focus: true
                font.bold: true
                font.pixelSize: 20
                text: map.xLati
            }

            /*위도 TextEdit border만들기*/
            Rectangle{
                x: 78
                y: 27
                z :1
                width: 150
                height: 24
                opacity: 0.9
                border.color: "black"
                border.width: 1
                radius: 4
            }

            Text { //경도
                id:longi
                x: 30
                y: 60
                font.pixelSize: 20
                font.bold: true
                text: qsTr("경도 : ")
            }

            TextEdit{ //경도좌표
                id:longitudeE
                x: 82
                y: 65
                z:2
                width: 175
                font.bold:true
                font.pixelSize: 20
                text: map.yLongi
            }
            /*경도 TextEdit border만들기*/
            Rectangle{
                x: 78
                y: 65
                z :1
                width: 150
                height: 24
                opacity: 0.9
                border.color: "black"
                border.width: 1
                radius: 4
            }

            /*삼영이엔씨 좌표로 즉시 이동*/
            Button{
                id: goSam
                x:30
                y:100
                width:70
                height: 20
                text: "삼영이엔씨"
                font.pixelSize: 11
                font.bold: true
                highlighted: true
                /*삼영이엔씨로 이동 그리고 삼영이엔씨 좌표 출력*/
                onClicked: {
                    map.center = QtPositioning.coordinate(map.xLati,map.yLongi)
                    latitudeE.text = map.xLati
                    longitudeE.text = map.yLongi
                }
            }

            /*TextEdit에 입력된 좌표로 즉시이동*/
            Button{
                id: goBtn
                x:110
                y:100
                width:60
                height: 20
                text: "이동"
                font.pixelSize: 15
                font.bold: true
                highlighted: true
                onClicked: {
                    /* 위도 경도 입력이 없을 경우 예외처리 */
                    if(latitudeE.text == "" || longitudeE.text == ""){

                        console.log("위도와 경도값을 모두 입력해주세요")
                    }
                    else{
                        /* 입력된 좌표로 이동 */
                        map.center = QtPositioning.coordinate(latitudeE.text, longitudeE.text)
                        console.log("해당 좌표로 이동합니다.")
                        map.zoomLevel = 15
                    }
                }
            }

            /* 위도 경도에 입력된 자표값 삭제*/
            Button{
                id: clearBtn
                x:180
                y:100
                width:60
                height: 20
                text: "초기화"
                font.pixelSize: 15
                font.bold: true
                highlighted: true
                onClicked: {
                    latitudeE.text = ""
                    longitudeE.text = ""
                    console.log("위도,좌표 값 제거")
                }
            }

            /* 생성된 핀 제거*/
            Button{
                id: clearPin
                x: 0
                y:130
                width:60
                height: 20
                text: "핀제거"
                font.pixelSize: 14
                highlighted: true
                font.bold: true
                onClicked: {
                    markerModel.clear()
                }
            }

        }
    }
}
