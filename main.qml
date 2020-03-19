import QtQuick 2.12
import QtQuick.Window 2.12
import QtLocation 5.9
import QtPositioning 5.8
import QtQuick.Controls 2.13 //Button사용
import QtQuick.Dialogs 1.2
import QtQuick.Shapes 1.12 //핀모양 만들기
import WeatherInfo 1.0


// 14a836908caa68e11b37f96d8c06c91d // openweathermap key

Window {
    /*윈도우 속성값들*/

    visible: true
    width: 512
    height: 512


    /*appmodel.cpp 가*/
    AppModel {
        id: model
        onReadyChanged: {
            if (model.ready){
                console.log("날씨를 불러왔습니다.")
                }
            else{
                console.log("날씨를 불러오는 중입니다.")
                }
        }
    }



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

        /*날씨 표시 구영*/
        Rectangle{
            id: weatherinfo
            x:60
            y:130
            width: 100
            height:100
            color: "red"
            Text {
                id: weathertext
                text: "여기날씨"
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

                console.log(model.hasValidWeather ? model.weather.weatherDescription : "no weather data")
                console.log((model.hasValidCity ? model.city : "Unknown location") + (model.useGps ? " (GPS)" : ""))

                /* openweathermap url 사용하여 시도
                var apiURL = "api.openweathermap.org/data/2.5/weather?lat="+ latitudeE.text + "& lon = " + longitudeE.text +"& appid =" + "14a836908caa68e11b37f96d8c06c91d"
                        $.ajax({
                            url: apiURL,
                            dataType: "json",
                            type: "GET",
                            asy도nc: "false",
                            success: function(resp) {
                                console.log(resp);
                                console.log("현재온도 : "+ (resp.main.temp- 273.15) );
                                console.log("현재습도 : "+ resp.main.humidity);
                                console.log("날씨 : "+ resp.weather[0].main );
                                console.log("상세날씨설명 : "+ resp.weather[0].description );
                                console.log("날씨 이미지 : "+ resp.weather[0].icon );
                                console.log("바람   : "+ resp.wind.speed );
                                console.log("나라   : "+ resp.sys.country );
                                console.log("도시이름  : "+ resp.name );
                                console.log("구름  : "+ (resp.clouds.all) +"%" );
                            }
                        })
                */

                /* kma.js 라이브러리 중 weather.js 부분
                const feedParser = require('feedparser-promised')
                    , moment = require('moment')
                    , kma = require('../lib/kma');

                class Weather {

                    townWeather(latitude, longitude) {
                        return new Promise((resolve, reject) => {
                            kma.convertBcode(latitude, longitude)
                                .then(bcode => {
                                    const url = `http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=${bcode}`;
                                    return feedParser.parse(url)
                                })
                                .then(feedData => {
                                    let data;

                                    if (feedData.length === 0) return reject({
                                        title: `날씨 정보를 가져올 수 없습니다`,
                                        message: `${bcode}에 해당하는 날씨 정보를 가져올 수 없습니다`,
                                        link: url
                                    });

                                    const weather = feedData[0];
                                    const date = moment(weather['meta']['rss:pubdate']['#'], 'YYYY년 MM월 DD일 (ddd)요일 HH:mm', 'ko');

                                    // 날씨 데이터
                                    data = {
                                        title: weather.title,
                                        category: weather.categories[0],
                                        author: weather.author,
                                        lastUpdated: moment(date).format(),
                                        info: []
                                    };


                                    const weatherInfo = weather['rss:description']['body']['data'];
                                    weatherInfo.forEach(info => {
                                        const hour = info['hour']['#'];
                                        const day = info['day']['#'];
                                        const temp = info['temp']['#'];
                                        const tmx = info['tmx']['#'];
                                        const tmn = info['tmn']['#'];
                                        const sky = info['sky']['#'];
                                        const pty = info['pty']['#'];
                                        const wfkor = info['wfkor']['#'];
                                        const wfen = info['wfen']['#'];
                                        const pop = info['pop']['#'];
                                        const r12 = info['r12']['#'];
                                        const s12 = info['s12']['#'];
                                        const ws = info['ws']['#'];
                                        const wd = info['wd']['#'];
                                        const wdkor = info['wdkor']['#'];
                                        const wden = info['wden']['#'];
                                        const reh = info['reh']['#'];
                                        const r06 = info['r06']['#'];
                                        const s06 = info['s06']['#'];

                                        let newInfo = {
                                            time: moment(date)
                                                .add(day, 'days')
                                                .hours(hour)
                                                .format(),
                                            temperature: {
                                                current: Number(temp),
                                                high: tmx === -999 ? null : tmx,
                                                min: tmn === -999 ? null : tmn
                                            },
                                            sky: {
                                                code: Number(sky),
                                                value: this.parseSkyCode(sky)
                                            },
                                            rain: {
                                                code: Number(pty),
                                                value: this.parseRainyProbabilityCode(pty),

                                                probability: Number(pop),
                                                expect: {
                                                    6: {
                                                        rainfall: Number(r06),
                                                        snowfall: Number(s06)
                                                    },
                                                    12: {
                                                        rainfall: Number(r12),
                                                        snowfall: Number(s12)
                                                    }
                                                }
                                            },
                                            weather: {
                                                value: {ko: wfkor, en: wfen}
                                            },
                                            wind: {
                                                speed: Number(Number(ws).toFixed(2)),
                                                direction: {
                                                    code: Number(wd),
                                                    value: {
                                                        ko: wdkor,
                                                        en: wden
                                                    }
                                                }
                                            },
                                            humidity: Number(reh)
                                        };

                                        data.info.push(newInfo);
                                    });

                                    resolve(data);
                                })
                                .catch(error => {
                                    reject(error);
                                });
                        });
                    }

                    parseSkyCode(skyCode) {
                        switch (skyCode | 0) {
                            case 1:
                                return '맑음';
                            case 2:
                                return '구름조금';
                            case 3:
                                return '구름많음';
                            case 4:
                                return '흐림';
                            default:
                                return null;
                        }
                    }

                    parseRainyProbabilityCode(pty) {
                        switch (pty | 0) {
                            case 0:
                                return '없음';
                            case 1:
                                return '비';
                            case 2:
                                return '비/눈';
                            case 3:
                                return '눈/비';
                            case 4:
                                return '눈';
                            default:
                                return null;
                        }
                    }
                }

                module.exports = new Weather();
            */
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
