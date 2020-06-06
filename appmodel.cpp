/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "appmodel.h"
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qnmeapositioninfosource.h>
#include <qgeopositioninfo.h>
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTimer>
#include <QUrlQuery>
#include <QElapsedTimer>
#include <QLoggingCategory>
#include <qgeocoordinate.h>
#include <QQmlApplicationEngine>
#include <QMessageBox>

#define ZERO_KELVIN 273.15

Q_LOGGING_CATEGORY(requestsLog,"wapp.requests")

WeatherData::WeatherData(QObject *parent) :
    QObject(parent)
{
}

WeatherData::WeatherData(const WeatherData &other) :
    QObject(0),
    m_dayOfWeek(other.m_dayOfWeek),
    m_weather(other.m_weather),
    m_weatherDescription(other.m_weatherDescription),
    m_temperature(other.m_temperature)
{
}

QString WeatherData::dayOfWeek() const
{

    return m_dayOfWeek;
}

/*!
 * The icon value is based on OpenWeatherMap.org icon set. For details
 * see http://bugs.openweathermap.org/projects/api/wiki/Weather_Condition_Codes
 *
 * e.g. 01d ->sunny day
 *
 * The icon string will be translated to
 * http://openweathermap.org/img/w/01d.png
 */
QString WeatherData::weatherIcon() const
{

    return m_weather;
}

QString WeatherData::weatherDescription() const
{

    return m_weatherDescription;
}

QString WeatherData::temperature() const
{

    return m_temperature;
}





void WeatherData::setDayOfWeek(const QString &value)
{
    m_dayOfWeek = value;
    emit dataChanged();
}

void WeatherData::setWeatherIcon(const QString &value)
{
    m_weather = value;
    emit dataChanged();
}

void WeatherData::setWeatherDescription(const QString &value)
{
    qCDebug(requestsLog) << "setWeatherDescription";
    m_weatherDescription = value;
    emit dataChanged();
}

void WeatherData::setTemperature(const QString &value)
{
    m_temperature = value;
    emit dataChanged();
}





class AppModelPrivate
{
public:
    static const int baseMsBeforeNewRequest = 0; // 5 s, increased after each missing answer up to 10x

    QGeoPositionInfoSource *src;//위치 소스
    QGeoCoordinate coord;//좌표
    QGeoCoordinate coord1;
    QGeoCoordinate coord2;
    QGeoCoordinate coord3;
    QGeoCoordinate coord4;
    QGeoCoordinate coord5;
    QGeoCoordinate coord6;
    QGeoCoordinate coord7;
    QGeoCoordinate coord8;
    QGeoCoordinate coord9;


    QString longitude, latitude; //경도 위
    QString latitude1, longitude1;
    QString latitude2, longitude2;
    QString latitude3, longitude3;
    QString latitude4, longitude4;
    QString latitude5, longitude5;
    QString latitude6, longitude6;
    QString latitude7, longitude7;
    QString latitude8, longitude8;
    QString latitude9, longitude9;

    QNetworkAccessManager *nam;// 네트워크연결관리자
    QNetworkSession *ns;//네트워크세션

    WeatherData now;//날씨데이터
    WeatherData now1;
    WeatherData now2;
    WeatherData now3;
    WeatherData now4;
    WeatherData now5;
    WeatherData now6;
    WeatherData now7;
    WeatherData now8;
    WeatherData now9;

    bool ready;

    QElapsedTimer throttle;
    int nErrors;
    int minMsBeforeNewRequest;
    QTimer requestNewWeatherTimer;
    QTimer delayedCityRequestTimer;
    QString app_ident;
    int count;


    AppModelPrivate() :
        src(NULL),
        nam(NULL),
        ns(NULL),

        ready(false),

        nErrors(0),
        minMsBeforeNewRequest(baseMsBeforeNewRequest)
    {
        requestNewWeatherTimer.setSingleShot(false);
        requestNewWeatherTimer.setInterval(20*60*1000); // 20 min
        throttle.invalidate();
        app_ident = QStringLiteral("36496bad1955bf3365448965a42b9eac");
    }
};


//! [0]
AppModel::AppModel(QObject *parent) : QObject(parent), d(new AppModelPrivate)
{
    //! [0]
    //    timeout함수를 사용하여 시간마다 함수를 실행시켜주는 구문
    //    connect(&d->delayedCityRequestTimer, SIGNAL(timeout()), this, SLOT(queryWeather()));

    d->requestNewWeatherTimer.start();


    //! [1]
    // make sure we have an active network session
    d->nam = new QNetworkAccessManager(this);

    QNetworkConfigurationManager ncm;
    d->ns = new QNetworkSession(ncm.defaultConfiguration(), this);
    connect(d->ns, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    // the session may be already open. if it is, run the slot directly
    if (d->ns->isOpen())
        this->networkSessionOpened();
    d->ns->open();
}
//! [1]

AppModel::~AppModel() //소멸자함수
{
    d->ns->close();
    if (d->src)
        d->src->stopUpdates();
    delete d;
}

//! [2]
void AppModel::networkSessionOpened()
{
    d->src = QGeoPositionInfoSource::createDefaultSource(this);

    if (d->src) {
        connect(d->src, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(positionUpdated(QGeoPositionInfo)));
        d->src->startUpdates();
    }

}
//! [2]


//! [3]
void AppModel::positionUpdated(QGeoPositionInfo)
{
    //        d->coord = gpsPos.coordinate(); //좌표에 GPS사용
    //    queryWeather();
}
//! [3]


void AppModel::queryWeather()
{
    qCDebug(requestsLog) << "queryWeather";

    switch (d->count) {
    case 0:
    {
        qCDebug(requestsLog) <<"case0 좌표 호출";
        d->latitude.setNum(d->coord.latitude());
        d->longitude.setNum(d->coord.longitude());
    }break;
    case 1:
    {
        qCDebug(requestsLog) <<"case1 좌표 호출";
        d->longitude1.setNum(d->coord1.longitude());
        d->latitude1.setNum(d->coord1.latitude());
    }break;
    case 2:
    {
        qCDebug(requestsLog) <<"case2 좌표 호출";
        d->longitude2.setNum(d->coord2.longitude());
        d->latitude2.setNum(d->coord2.latitude());
    }break;
    case 3:
    {
        d->longitude3.setNum(d->coord3.longitude());
        d->latitude3.setNum(d->coord3.latitude());
    }break;
    case 4:
    {
        d->longitude4.setNum(d->coord4.longitude());
        d->latitude4.setNum(d->coord4.latitude());
    }break;
    case 5:
    {
        d->longitude5.setNum(d->coord5.longitude());
        d->latitude5.setNum(d->coord5.latitude());
    }break;
    case 6:
    {
        d->longitude6.setNum(d->coord6.longitude());
        d->latitude6.setNum(d->coord6.latitude());
    }break;
    case 7:
    {
        d->longitude7.setNum(d->coord7.longitude());
        d->latitude7.setNum(d->coord7.latitude());
    }break;
    case 8:
    {
        d->longitude8.setNum(d->coord8.longitude());
        d->latitude8.setNum(d->coord8.latitude());
    }break;
    case 9:
    {
        d->longitude9.setNum(d->coord9.longitude());
        d->latitude9.setNum(d->coord9.latitude());
    }break;

    }

    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;

    switch (d->count) {
    case 0:
    {
        qCDebug(requestsLog) <<"case 쿼리에 좌표삽입";
        query.addQueryItem("lat", d->latitude);
        query.addQueryItem("lon", d->longitude);
    }break;
    case 1:
    {
        qCDebug(requestsLog) <<"case1 쿼리에 좌표삽입";
        query.addQueryItem("lat", d->latitude1);
        query.addQueryItem("lon", d->longitude1);
    }break;
    case 2:
    {
        qCDebug(requestsLog) <<"case2 쿼리에 좌표삽입";
        query.addQueryItem("lat", d->latitude2);
        query.addQueryItem("lon", d->longitude2);
    }break;
    case 3:
    {
        query.addQueryItem("lat", d->latitude3);
        query.addQueryItem("lon", d->longitude3);
    }break;
    case 4:
    {
        query.addQueryItem("lat", d->latitude4);
        query.addQueryItem("lon", d->longitude4);
    }break;
    case 5:
    {
        query.addQueryItem("lat", d->latitude5);
        query.addQueryItem("lon", d->longitude5);
    }break;
    case 6:
    {
        query.addQueryItem("lat", d->latitude6);
        query.addQueryItem("lon", d->longitude6);
    }break;
    case 7:
    {
        query.addQueryItem("lat", d->latitude7);
        query.addQueryItem("lon", d->longitude7);
    }break;
    case 8:
    {
        query.addQueryItem("lat", d->latitude8);
        query.addQueryItem("lon", d->longitude8);
    }break;
    case 9:
    {
        query.addQueryItem("lat", d->latitude9);
        query.addQueryItem("lon", d->longitude9);
    }break;
    }

    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right away
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData(rep); });

}


static QString niceTemperatureString(double t)
{
    return QString::number(qRound(t-ZERO_KELVIN)) + QChar(0xB0);
}

void AppModel::handleWeatherNetworkData(QNetworkReply *networkReply)
{
    qCDebug(requestsLog) << "(handleWeatherNetworkData) got weather network data";
    if (!networkReply){
        return;
    }

    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue val;
            if (obj.contains(QStringLiteral("weather"))) {
                val = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = val.toArray();
                val = weatherArray.at(0);
                tempObject = val.toObject();


                switch (d->count) {
                case 0:
                {
                    d->now.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now.setWeatherIcon(tempObject.value("icon").toString());
                }break;


                case 1:
                {
                    qCDebug(requestsLog) << "d->now1 값에 날씨 대입";
                    d->now1.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now1.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 2:
                {
                    qCDebug(requestsLog) << "d->now2 값에 날씨 대입";
                    d->now2.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now2.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 3:
                {
                    d->now3.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now3.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 4:
                {
                    d->now4.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now4.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 5:
                {
                    d->now5.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now5.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 6:
                {
                    d->now6.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now6.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 7:
                {
                    d->now7.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now7.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 8:
                {
                    qCDebug(requestsLog) << "d->now8 값에 날씨 대입";
                    d->now8.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now8.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 9:
                {
                    qCDebug(requestsLog) << "d->now9 값에 날씨 대입";
                    d->now9.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now9.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                }
            }


            if (obj.contains(QStringLiteral("main"))) {
                val = obj.value(QStringLiteral("main"));
                tempObject = val.toObject();
                val = tempObject.value(QStringLiteral("temp"));


                switch (d->count) {
                case 0:
                {
                    d->now.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 1:
                {
                    d->now1.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 2:
                {
                    d->now2.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 3:
                {
                    d->now3.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 4:
                {
                    d->now4.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 5:
                {
                    d->now5.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 6:
                {
                    d->now6.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 7:
                {
                    d->now7.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 8:
                {
                    d->now8.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                case 9:
                {
                    d->now9.setTemperature(niceTemperatureString(val.toDouble()));
                }break;
                }
            }
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();

    }
    networkReply->deleteLater();
}


bool AppModel::hasValidWeather() const
{
    return (!(d->now.weatherIcon().isEmpty()) && (d->now.weatherIcon().size() > 1) && d->now.weatherIcon() != "");
}

bool AppModel::hasValidWeather1() const
{
    qCDebug(requestsLog) <<"hasValidWeather1";
    return (!(d->now1.weatherIcon().isEmpty()) && (d->now1.weatherIcon().size() > 1) && d->now1.weatherIcon() != "");
}


bool AppModel::hasValidWeather2() const
{
    qCDebug(requestsLog) <<"hasValidWeather2";
    return (!(d->now2.weatherIcon().isEmpty()) && (d->now2.weatherIcon().size() > 1) && d->now2.weatherIcon() != "");
}


bool AppModel::hasValidWeather3() const
{
    return (!(d->now3.weatherIcon().isEmpty()) && (d->now3.weatherIcon().size() > 1) && d->now3.weatherIcon() != "");
}


bool AppModel::hasValidWeather4() const
{
    return (!(d->now4.weatherIcon().isEmpty()) && (d->now4.weatherIcon().size() > 1) && d->now4.weatherIcon() != "");
}


bool AppModel::hasValidWeather5() const
{
    return (!(d->now5.weatherIcon().isEmpty()) && (d->now5.weatherIcon().size() > 1) && d->now5.weatherIcon() != "");
}


bool AppModel::hasValidWeather6() const
{
    return (!(d->now6.weatherIcon().isEmpty()) && (d->now6.weatherIcon().size() > 1) && d->now6.weatherIcon() != "");
}


bool AppModel::hasValidWeather7() const
{
    return (!(d->now7.weatherIcon().isEmpty()) && (d->now7.weatherIcon().size() > 1) && d->now7.weatherIcon() != "");
}


bool AppModel::hasValidWeather8() const
{
    qCDebug(requestsLog) <<"hasValidWeather8";
    return (!(d->now8.weatherIcon().isEmpty()) && (d->now8.weatherIcon().size() > 1) && d->now8.weatherIcon() != "");
}

bool AppModel::hasValidWeather9() const
{
    qCDebug(requestsLog) <<"hasValidWeather9";
    return  (!(d->now9.weatherIcon().isEmpty()) && (d->now9.weatherIcon().size() > 1) && d->now9.weatherIcon() != "");
}


WeatherData *AppModel::weather() const
{
    return &(d->now);
}
WeatherData *AppModel::weather1() const
{
    qCDebug(requestsLog) << "weather1";
    return &(d->now1);
}
WeatherData *AppModel::weather2() const
{
    qCDebug(requestsLog) << "weather2";
    return &(d->now2);
}
WeatherData *AppModel::weather3() const
{
    return &(d->now3);
}
WeatherData *AppModel::weather4() const
{
    return &(d->now4);
}
WeatherData *AppModel::weather5() const
{
    return &(d->now5);
}
WeatherData *AppModel::weather6() const
{
    return &(d->now6);
}
WeatherData *AppModel::weather7() const
{
    return &(d->now7);
}
WeatherData *AppModel::weather8() const
{
    qCDebug(requestsLog) << "weather8";
    return &(d->now8);
}
WeatherData *AppModel::weather9() const
{
    qCDebug(requestsLog) << "weather9";
    return &(d->now9);
}



bool AppModel::ready() const
{
    return d->ready;
}

bool AppModel::hasSource() const
{
    return (d->src != NULL);
}

double AppModel::sendLatitude(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord.setLatitude(lat);
    d->count = 0;
    return lat;
}
double AppModel::sendLongitude(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord.setLongitude(lon);
    return lon;
}

double AppModel::sendLatitude1(double lat) //qml에서 경도값을 받아오는 set함수
{
    qCDebug(requestsLog)<<"sendLatitude1";
    d->coord1.setLatitude(lat);
    d->count = 1;

    return lat;
}
double AppModel::sendLongitude1(double lon) //qml에서 위도값을 받아오는 set함수
{
    qCDebug(requestsLog)<<"sendLongitude1";
    d->coord1.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude2(double lat) //qml에서 경도값을 받아오는 set함수
{
    qCDebug(requestsLog)<<"sendLatitude2";
    d->coord2.setLatitude(lat);
    d->count = 2;

    return lat;
}
double AppModel::sendLongitude2(double lon) //qml에서 위도값을 받아오는 set함수
{
    qCDebug(requestsLog)<<"sendLongitude2";
    d->coord2.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude3(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord3.setLatitude(lat);
    d->count = 3;

    return lat;
}
double AppModel::sendLongitude3(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord3.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude4(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord4.setLatitude(lat);
    d->count = 4;

    return lat;
}
double AppModel::sendLongitude4(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord4.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude5(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord5.setLatitude(lat);
    d->count = 5;

    return lat;
}
double AppModel::sendLongitude5(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord5.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude6(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord6.setLatitude(lat);
    d->count = 6;

    return lat;
}
double AppModel::sendLongitude6(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord6.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude7(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord7.setLatitude(lat);
    d->count = 7;

    return lat;
}
double AppModel::sendLongitude7(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord7.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude8(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord8.setLatitude(lat);

    d->count = 8;
    return lat;
}
double AppModel::sendLongitude8(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord8.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude9(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord9.setLatitude(lat);
    d->count = 9;
    return lat;
}
double AppModel::sendLongitude9(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord9.setLongitude(lon);
    return lon;
}

void AppModel::myQmlSlot() //콜백방식으로 쿼리시티를 호출하기 위해서 선언한 함수 qml에서 이벤트 발생시 호출된다.
{
    qCDebug(requestsLog) << "myQmlSlot";
    queryWeather();
}


double AppModel::sendZoomLevel(double lev){ //추후 줌했을때 실행할 함수
    if(lev >10.0 && lev <11.0){
    }
    return lev;
}
