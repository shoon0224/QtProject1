import QtQuick 2.12
import QtQuick.Window 2.12
import QtLocation 5.9
import QtPositioning 5.8
import QtQuick.Controls 2.13 //Button사용
import QtQuick.Dialogs 1.2
import QtQuick.Shapes 1.12 //핀모양 만들기
import WeatherInfo 1.0
import "components"


Window {
    /*윈도우 속성값들*/
    id: win
    visible: true
    //화면크기고정
    minimumWidth: 720
    minimumHeight: 720
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight



    AppModel {
        id: model
        onReadyChanged: {
            if (model.ready){
                console.log("날씨 데이터를 불러왔습니다.")
            }
            else{
                console.log("날씨 데이터를 불러오는 중입니다.")
            }
        }
    }







    Plugin{
        /*Map Api 관련 값인 것 같음*/
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


        property alias geocodeModel: geocodeModel
        signal showGeocodeInfo()
        signal geocodeFinished()
        signal coordinatesCaptured(double latitude, double longitude)


        /*위도, 경도 좌표 입력 시 해당 좌표로 이동*/
        GeocodeModel {
            id: geocodeModel
            plugin: map.plugin
            onStatusChanged: {
                if ((status == GeocodeModel.Ready) || (status == GeocodeModel.Error))
                    map.geocodeFinished()
            }
            onLocationsChanged:
            {
                if (count == 1) {
                    /*해당 좌표로 지도가 중심이되어 이동함*/
                    map.center.latitude = get(0).coordinate.latitude
                    map.center.longitude = get(0).coordinate.longitude
                }
            }
        }







        MapItemView {
            model: geocodeModel

        }


        Text {
            anchors.right: parent.right
            anchors.rightMargin: 30
            text: qsTr("왼쪽  더블클릭 시 확대\n오른쪽 더블클릭 시 축소\n길게 누르면 핀생성\n현재 줌레벨 : "+ (map.zoomLevel).toFixed(4))
            color: "grey"
            font.pixelSize: 16
            opacity: 0.9
            z: parent.z + 10
        }

        /*날씨 표시 구역*/
        BigForecastIcon {
            id:solo
            x:5
            y:150
            visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? false : true)
            //아이콘 크기
            width: 100
            height: 100
            opacity: 1
            weatherIcon: (model.hasValidWeather ? model.weather.weatherIcon : "01d")//날씨 아이콘
            topText: (model.hasValidWeather ? model.weather.temperature : "") // 온도
            bottomText: (model.hasValidWeather ? model.weather.weatherDescription : "") //날씨상태

        }


        Item {
            focus: true
            Keys.onPressed: {
                if(map.zoomLevel > 11 || map.zoomLevel < 10)
                {
                    switch(event.key){
                    case Qt.Key_Return:
                        var crd = map.toCoordinate(Qt.point(360, 360))
                        latitudeE.text = (crd.latitude).toFixed(4)
                        longitudeE.text = (crd.longitude).toFixed(4)
                        model.sendLatitude(crd.latitude)
                        model.sendLongitude(crd.longitude)
                        model.myQmlSlot5()
                    }
                }
                if(map.zoomLevel >= 10 && map.zoomLevel <= 11)
                {
                    switch(event.key){
                    case Qt.Key_Return:
                        console.log("crd1")
                        var crd1 = map.toCoordinate(Qt.point(120, 120))
                        model.sendLatitude(crd1.latitude)
                        model.sendLongitude(crd1.longitude)
                        here1.text = crd1.latitude + "\n" + crd1.longitude
                        model.myQmlSlot1()

                        console.log("crd2")
                        var crd2 = map.toCoordinate(Qt.point(360, 120))
                        model.sendLatitude(crd2.latitude)
                        model.sendLongitude(crd2.longitude)
                        here2.text = crd2.latitude + "\n" + crd2.longitude
                        model.myQmlSlot2()

                        console.log("crd3")
                        var crd3 = map.toCoordinate(Qt.point(600, 120))
                        model.sendLatitude(crd3.latitude)
                        model.sendLongitude(crd3.longitude)
                        here3.text = crd3.latitude + "\n" + crd3.longitude
                        model.myQmlSlot3()

                        console.log("crd4")
                        var crd4 = map.toCoordinate(Qt.point(120, 360))
                        model.sendLatitude(crd4.latitude)
                        model.sendLongitude(crd4.longitude)
                        here4.text = crd4.latitude + "\n" + crd4.longitude
                        model.myQmlSlot4()

                        console.log("crd5")
                        var crd5 = map.toCoordinate(Qt.point(360, 360))
                        model.sendLatitude(crd5.latitude)
                        model.sendLongitude(crd5.longitude)
                        here5.text = crd5.latitude + "\n" + crd5.longitude
                        model.myQmlSlot5()

                        console.log("crd6")
                        var crd6 = map.toCoordinate(Qt.point(600, 360))
                        model.sendLatitude(crd6.latitude)
                        model.sendLongitude(crd6.longitude)
                        here6.text = crd6.latitude + "\n" + crd6.longitude
                        model.myQmlSlot6()

                        console.log("crd7")
                        var crd7 = map.toCoordinate(Qt.point(120, 600))
                        model.sendLatitude(crd7.latitude)
                        model.sendLongitude(crd7.longitude)
                        here7.text = crd7.latitude + "\n" + crd7.longitude
                        model.myQmlSlot7()

                        console.log("crd8")
                        var crd8 = map.toCoordinate(Qt.point(360, 600))
                        model.sendLatitude(crd8.latitude)
                        model.sendLongitude(crd8.longitude)
                        here8.text = crd8.latitude + "\n" + crd8.longitude
                        model.myQmlSlot8()

                        console.log("crd9")
                        var crd9 = map.toCoordinate(Qt.point(600, 600))
                        model.sendLatitude(crd9.latitude)
                        model.sendLongitude(crd9.longitude)
                        here9.text = crd9.latitude + "\n" + crd9.longitude
                        model.myQmlSlot9()
                    }
                }
            }
        }
    }
    BigForecastIcon {
        id:weather1
        x:70
        y:70
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather1.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather1 ? model.weather1.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather1 ? model.weather1.temperature : "??") // 온도
        bottomText: (model.hasValidWeather1 ? model.weather1.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here1
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
    }

    BigForecastIcon {
        id:weather2
        x:310
        y:70
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather2.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather2 ? model.weather2.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather2 ? model.weather2.temperature : "??") // 온도
        bottomText: (model.hasValidWeather2 ? model.weather2.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here2
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
    }

    BigForecastIcon {
        id:weather3
        x:550
        y:70
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather3.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather3 ? model.weather3.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather3 ? model.weather3.temperature : "??") // 온도
        bottomText: (model.hasValidWeather3 ? model.weather3.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here3
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
    }

    BigForecastIcon {
        id:weather4
        x:70
        y:310
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather4.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather4 ? model.weather4.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather4 ? model.weather4.temperature : "??") // 온도
        bottomText: (model.hasValidWeather4 ? model.weather4.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here4
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
    }

    BigForecastIcon {
        id:weather5
        x:310
        y:310
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather5.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather5 ? model.weather5.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather5 ? model.weather5.temperature : "??") // 온도
        bottomText: (model.hasValidWeather5? model.weather5.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here5
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
    }

    BigForecastIcon {
        id:weather6
        x:550
        y:310
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather6.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather6 ? model.weather6.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather6 ? model.weather6.temperature : "??") // 온도
        bottomText: (model.hasValidWeather6 ? model.weather6.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here6
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
    }

    BigForecastIcon {
        id:weather7
        x:70
        y:550
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather7.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather7 ? model.weather7.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather7 ? model.weather7.temperature : "??") // 온도
        bottomText: (model.hasValidWeather7 ? model.weather7.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here7
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
    }

    BigForecastIcon {
        id:weather8
        x:310
        y:550
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather8.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather8 ? model.weather8.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather8 ? model.weather8.temperature : "??") // 온도
        bottomText: (model.hasValidWeather8 ? model.weather8.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here8
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
    }

    BigForecastIcon {
        id:weather9
        x:550
        y:550
        visible: (map.zoomLevel >= 10 && map.zoomLevel <= 11 ? true : false)
        width: 100
        height: 100
        opacity: (model.weather9.weatherIcon ? 1 : 0.3)
        weatherIcon: (model.hasValidWeather9 ? model.weather9.weatherIcon : "01d") //날씨 아이콘
        topText: (model.hasValidWeather9 ? model.weather9.temperature : "??") // 온도
        bottomText: (model.hasValidWeather9 ? model.weather9.weatherDescription : "No weather data") //날씨상태
        Text{
            id:here9
            x:0
            y:100
            font.pointSize: 8
            text:""
        }
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
        enabled: (map.zoomLevel > 11 || map.zoomLevel < 10 ? true : false) // 이 예외처리를 안해주면 위에 버튼클릭이 안됨


        /* 클릭 했을 시 이벤트 핸들러 */
        onClicked:  {

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
    }

    /*좌측 위도경도 표시 레이어*/
    Rectangle{
        id:layer
        width:250
        height:130
        /*투명도*/
        visible:(map.zoomLevel >= 10 && map.zoomLevel <= 11 ? false : true)
        opacity: 0.9
        /*테두리*/
        border.width:1

        Text { //위도
            id:latT
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
            id:longiT
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
                model.sendLatitude(latitudeE.text)
                model.sendLongitude(longitudeE.text)
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
                    map.zoomLevel = 15
                    model.sendLatitude(latitudeE.text)
                    model.sendLongitude(longitudeE.text)
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

