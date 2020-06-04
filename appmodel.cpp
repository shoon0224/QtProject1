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


    QString city; //도시
    QString city1;
    QString city2;
    QString city3;
    QString city4;
    QString city5;
    QString city6;
    QString city7;
    QString city8;
    QString city9;


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


    QList<WeatherData*> forecast; //예보
    QQmlListProperty<WeatherData> *fcProp;
    bool ready;
    bool useGps;
    QElapsedTimer throttle;
    int nErrors;
    int minMsBeforeNewRequest;
    QTimer delayedCityRequestTimer;
    QTimer requestNewWeatherTimer;
    QString app_ident;
    int count;


    AppModelPrivate() :
        src(NULL),
        nam(NULL),
        ns(NULL),
        fcProp(NULL),
        ready(false),
        useGps(true),
        nErrors(0),
        minMsBeforeNewRequest(baseMsBeforeNewRequest)
    {
        delayedCityRequestTimer.setSingleShot(true);
        delayedCityRequestTimer.setInterval(1000); // 1 s
        requestNewWeatherTimer.setSingleShot(false);
        requestNewWeatherTimer.setInterval(20*60*1000); // 20 min
        throttle.invalidate();
        app_ident = QStringLiteral("36496bad1955bf3365448965a42b9eac");
    }
};



static void forecastAppend(QQmlListProperty<WeatherData> *prop, WeatherData *val)
{
    Q_UNUSED(val);
    Q_UNUSED(prop);
}

static WeatherData *forecastAt(QQmlListProperty<WeatherData> *prop, int index)
{
    AppModelPrivate *d = static_cast<AppModelPrivate*>(prop->data);
    return d->forecast.at(index);
}

static int forecastCount(QQmlListProperty<WeatherData> *prop)
{
    AppModelPrivate *d = static_cast<AppModelPrivate*>(prop->data);
    return d->forecast.size();
}

static void forecastClear(QQmlListProperty<WeatherData> *prop)
{
    static_cast<AppModelPrivate*>(prop->data)->forecast.clear();
}


//! [0]
AppModel::AppModel(QObject *parent) : QObject(parent), d(new AppModelPrivate)
{
    //! [0]
    d->fcProp = new QQmlListProperty<WeatherData>(this, d, forecastAppend, forecastCount, forecastAt, forecastClear);



    //    timeout함수를 사용하여 시간마다 함수를 실행시켜주는 구문
    //    connect(&d->delayedCityRequestTimer, SIGNAL(timeout()), this, SLOT(queryCity()));
    //    connect(&d->delayedCityRequestTimer, SIGNAL(timeout()), this, SLOT(tidalCurrent()));
    //    connect(&d->requestNewWeatherTimer, SIGNAL(timeout()), this, SLOT(refreshWeather()));

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
        connect(d->src, SIGNAL(error(QGeoPositionInfoSource::Error)), this, SLOT(positionError(QGeoPositionInfoSource::Error)));
        d->src->startUpdates();
    } else {
        d->useGps = false;
        d->city = "";
        emit cityChanged();
        this->refreshWeather();
    }
}
//! [2]


//! [3]
void AppModel::positionUpdated(QGeoPositionInfo)
{
    //        d->coord = gpsPos.coordinate(); //좌표에 GPS사용

    if (!(d->useGps))
        return;
    //    queryCity();
    //    tidalCurrent();
}
//! [3]

void AppModel::tidalCurrent()
{
    if (d->throttle.isValid() && d->throttle.elapsed() < d->minMsBeforeNewRequest ) {
        qCDebug(requestsLog) << "delaying query of tidal";
        if (!d->delayedCityRequestTimer.isActive())
            d->delayedCityRequestTimer.start();
        return;
    }

    qDebug(requestsLog) << "requested query of tidalCurrent";
    d->throttle.start();
    d->minMsBeforeNewRequest = (d->nErrors + 1) * d->baseMsBeforeNewRequest;
    QString latitude, longitude;
    longitude.setNum(d->coord.longitude());
    latitude.setNum(d->coord.latitude());

    QUrl url("http://www.khoa.go.kr/oceangrid/grid/api/tidalCurrentPoint/search.do?ServiceKey=""&Sdate=""&SHour=""&SMinute=""&Edate=""&EHour=""&EMinute=""&lon=""&lat=""&ResultType=json");
    QUrlQuery query;
    query.addQueryItem("lat", latitude); // key="lat", value=latitude --> 이값이 위에서 넣은 latitude
    query.addQueryItem("lon", longitude);
    query.addQueryItem("Sdate", "20200528");
    query.addQueryItem("SHour", "09");
    query.addQueryItem("SMinute", "25");
    query.addQueryItem("Edate", "20200529");
    query.addQueryItem("EHour", "09");
    query.addQueryItem("EMinute", "26");
    query.addQueryItem("ServiceKey", "ZrMNzxjA3qcZ4Qw6XN4E4g==");

    url.setQuery(query);
    qCDebug(requestsLog) << "submitting request";

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right away
    connect(rep, &QNetworkReply::finished, this, [this, rep]() { handleTidalCurrentNetworkData(rep); });
}

void AppModel::queryCity()
{
    //don't update more often then once a minute
    //to keep load on server low
    if (d->throttle.isValid() && d->throttle.elapsed() < d->minMsBeforeNewRequest ) {
        qCDebug(requestsLog) << "delaying query of city";
        if (!d->delayedCityRequestTimer.isActive()){
            d->delayedCityRequestTimer.start();
        }
        return ;
    }

    qDebug(requestsLog) << "requested query of city";
    d->throttle.start();
    d->minMsBeforeNewRequest = (d->nErrors + 1) * d->baseMsBeforeNewRequest;

    switch (d->count) {

    case 0:
    {
        d->latitude.setNum(d->coord.latitude());
        d->longitude.setNum(d->coord.longitude());
    }break;
    case 1:
    {
        d->longitude1.setNum(d->coord1.longitude());
        d->latitude1.setNum(d->coord1.latitude());
    }break;
    case 2:
    {
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
        query.addQueryItem("lat", d->latitude);
        query.addQueryItem("lon", d->longitude);
    }break;
    case 1:
    {
        query.addQueryItem("lat", d->latitude1);
        query.addQueryItem("lon", d->longitude1);
    }break;
    case 2:
    {
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

    qCDebug(requestsLog) << "submitting request";

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right away
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleGeoNetworkData(rep); });
}



void AppModel::positionError(QGeoPositionInfoSource::Error e)
{
    Q_UNUSED(e);
    qWarning() << "Position source error. Falling back to simulation mode.";
    // cleanup insufficient QGeoPositionInfoSource instance
    d->src->stopUpdates();
    d->src->deleteLater();
    d->src = 0;

    // activate simulation mode
    d->useGps = false;
    d->city = "";
    emit cityChanged();
    this->refreshWeather();
}

void AppModel::hadError(bool tryAgain)
{
    qCDebug(requestsLog) << "hadError, will " << (tryAgain ? "" : "not ") << "rety";
    d->throttle.start();
    if(d->nErrors < 10)
        ++d->nErrors;
    d->minMsBeforeNewRequest = (d->nErrors + 1) * d->baseMsBeforeNewRequest;
    if (tryAgain)
        d->delayedCityRequestTimer.start();
}
void AppModel::handleTidalCurrentNetworkData(QNetworkReply *networkReply){

    if (!networkReply) {
        hadError(false); // should retry?
        return;
    }
    if (!networkReply->error()) {
        d->nErrors = 0;
        if (!d->throttle.isValid())
            d->throttle.start();
        d->minMsBeforeNewRequest = d->baseMsBeforeNewRequest;
        //convert coordinates to city name
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        qDebug(requestsLog) << "document tidalCurrent 값: " << document;
        QJsonObject jo = document.object();
        QJsonValue jv = jo.value(QStringLiteral("name"));

    }else{
        hadError(true);
    }
    networkReply->deleteLater();

}

void AppModel::handleGeoNetworkData(QNetworkReply *networkReply)
{
    if (!networkReply) {
        hadError(false); // should retry?
        return;
    }

    if (!networkReply->error()) {
        d->nErrors = 0;
        if (!d->throttle.isValid())
            d->throttle.start();
        d->minMsBeforeNewRequest = d->baseMsBeforeNewRequest;
        //convert coordinates to city name
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        qDebug(requestsLog) << "document 값: " << document;
        QJsonObject jo = document.object();
        QJsonValue jv = jo.value(QStringLiteral("name"));


        const QString city = jv.toString();
        qCDebug(requestsLog) << "got city: " << city;



        //여기서 도시이름이 다같이 바뀌냐 안바뀌냐 예외처리
        switch (d->count) {
        case 0:
        {
            d->city = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 1:
        {
            d->city1 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 2:
        {
            d->city2 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 3:
        {
            d->city3 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 4:
        {
            d->city4 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 5:
        {
            d->city5 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 6:
        {
            d->city6 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 7:
        {
            d->city7 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 8:
        {
            d->city8 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        case 9:
        {
            d->city9 = city;
            emit cityChanged();
            refreshWeather();
        }break;
        }
    }else{
        hadError(true);
    }
    // 원래 else로 있었음 그래야 if문 종료하고 다시 도시 검색실행을 하지않음
    networkReply->deleteLater();
}


void AppModel::refreshWeather()//도시값을 받아서 날씨를 데이터를 가져오는 함수
{
    if (d->city.isEmpty()) { //도시가 비어있으면 아래 refreshing weather skipped로그 출력
        qCDebug(requestsLog) << "refreshing weather skipped (no city)";
        return;
    }
    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;

    switch (d->count) {
    case 0:
    {
        query.addQueryItem("q", d->city);
    }break;
    case 1:
    {
        query.addQueryItem("q", d->city1);
    }break;
    case 2:
    {
        query.addQueryItem("q", d->city2);
    }break;
    case 3:
    {
        query.addQueryItem("q", d->city3);
    }break;
    case 4:
    {
        query.addQueryItem("q", d->city4);
    }break;
    case 5:
    {
        query.addQueryItem("q", d->city5);
    }break;
    case 6:
    {
        query.addQueryItem("q", d->city6);
    }break;
    case 7:
    {
        query.addQueryItem("q", d->city7);
    }break;
    case 8:
    {
        query.addQueryItem("q", d->city8);
    }break;
    case 9:
    {
        query.addQueryItem("q", d->city9);
    }break;
    }

    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);


    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right away //시그널을 바로 연결
    connect(rep, &QNetworkReply::finished, this, [this, rep]() { handleWeatherNetworkData(rep); });
}




static QString niceTemperatureString(double t)
{
    return QString::number(qRound(t-ZERO_KELVIN)) + QChar(0xB0);
}

void AppModel::handleWeatherNetworkData(QNetworkReply *networkReply) //함수
{
    qCDebug(requestsLog) << "got weather network data";
    if (!networkReply)
        return;

    if (!networkReply->error()) {
        foreach (WeatherData *inf, d->forecast)
            delete inf;
        d->forecast.clear();

        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());//JSON 데이터를 사용하는 방법
        if (document.isObject()) {//만약 JSON데이터를 가지고 있는 document가 오브젝트 타입이면
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
                    d->now1.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now1.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 2:
                {
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
                    d->now8.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                    d->now8.setWeatherIcon(tempObject.value("icon").toString());
                }break;
                case 9:
                {
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
    /*
        //retrieve the forecast
        QUrl url("http://api.openweathermap.org/data/2.5/forecast/daily");
        QUrlQuery query;

        query.addQueryItem("q", d->city);
        query.addQueryItem("mode", "json");
        query.addQueryItem("cnt", "5");
        query.addQueryItem("APPID", d->app_ident);
        url.setQuery(query);

        QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
        // connect up the signal right away
        connect(rep, &QNetworkReply::finished,
                this, [this, rep]() { handleForecastNetworkData(rep); });
     */
}


void AppModel::handleForecastNetworkData(QNetworkReply *networkReply)
{
    //    qCDebug(requestsLog) << "got forecast";
    if (!networkReply)
        return;

    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        QJsonObject jo;
        QJsonValue jv;
        QJsonObject root = document.object();
        jv = root.value(QStringLiteral("list"));
        if (!jv.isArray())
            qWarning() << "Invalid forecast object";
        QJsonArray ja = jv.toArray();
        //we need 4 days of forecast -> first entry is today
        if (ja.count() != 5)
            qWarning() << "Invalid forecast object";

        QString data;
        for (int i = 1; i<ja.count(); i++) {
            WeatherData *forecastEntry = new WeatherData();

            //min/max temperature
            QJsonObject subtree = ja.at(i).toObject();
            jo = subtree.value(QStringLiteral("temp")).toObject();
            jv = jo.value(QStringLiteral("min"));
            data.clear();
            data += niceTemperatureString(jv.toDouble());
            data += QChar('/');
            jv = jo.value(QStringLiteral("max"));
            data += niceTemperatureString(jv.toDouble());
            forecastEntry->setTemperature(data);

            //get date
            jv = subtree.value(QStringLiteral("dt"));
            QDateTime dt = QDateTime::fromMSecsSinceEpoch((qint64)jv.toDouble()*1000);
            forecastEntry->setDayOfWeek(dt.date().toString(QStringLiteral("ddd")));

            //get icon
            QJsonArray weatherArray = subtree.value(QStringLiteral("weather")).toArray();
            jo = weatherArray.at(0).toObject();
            forecastEntry->setWeatherIcon(jo.value(QStringLiteral("icon")).toString());

            //get description
            forecastEntry->setWeatherDescription(jo.value(QStringLiteral("description")).toString());

            d->forecast.append(forecastEntry);
        }


    }
    networkReply->deleteLater();
}











bool AppModel::hasValidCity() const
{
    return (!(d->city.isEmpty()) && d->city.size() > 1 && d->city != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather() const
{

    return hasValidCity() && (!(d->now.weatherIcon().isEmpty()) && (d->now.weatherIcon().size() > 1) && d->now.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity1() const
{

    return (!(d->city1.isEmpty()) && d->city1.size() > 1 && d->city1 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather1() const
{

    return hasValidCity1() && (!(d->now1.weatherIcon().isEmpty()) && (d->now1.weatherIcon().size() > 1) && d->now1.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity2() const
{

    return (!(d->city2.isEmpty()) && d->city2.size() > 1 && d->city2 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather2() const
{

    return hasValidCity2() && (!(d->now2.weatherIcon().isEmpty()) && (d->now2.weatherIcon().size() > 1) && d->now2.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity3() const
{

    return (!(d->city3.isEmpty()) && d->city3.size() > 1 && d->city3 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather3() const
{

    return hasValidCity3() && (!(d->now3.weatherIcon().isEmpty()) && (d->now3.weatherIcon().size() > 1) && d->now3.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity4() const
{

    return (!(d->city4.isEmpty()) && d->city4.size() > 1 && d->city4 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather4() const
{

    return hasValidCity4() && (!(d->now4.weatherIcon().isEmpty()) && (d->now4.weatherIcon().size() > 1) && d->now4.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity5() const
{

    return (!(d->city5.isEmpty()) && d->city5.size() > 1 && d->city5 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather5() const
{
    return hasValidCity5() && (!(d->now5.weatherIcon().isEmpty()) && (d->now5.weatherIcon().size() > 1) && d->now5.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity6() const
{
    return (!(d->city6.isEmpty()) && d->city6.size() > 1 && d->city6 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather6() const
{
    return hasValidCity6() && (!(d->now6.weatherIcon().isEmpty()) && (d->now6.weatherIcon().size() > 1) && d->now6.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity7() const
{
    return (!(d->city7.isEmpty()) && d->city7.size() > 1 && d->city7 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather7() const
{
    return hasValidCity7() && (!(d->now7.weatherIcon().isEmpty()) && (d->now7.weatherIcon().size() > 1) && d->now7.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity8() const
{
    return (!(d->city8.isEmpty()) && d->city8.size() > 1 && d->city8 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather8() const
{
    return hasValidCity8() && (!(d->now8.weatherIcon().isEmpty()) && (d->now8.weatherIcon().size() > 1) && d->now8.weatherIcon() != "");
}
/*************************************************************************************/
bool AppModel::hasValidCity9() const
{
    return (!(d->city9.isEmpty()) && d->city9.size() > 1 && d->city9 != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}
bool AppModel::hasValidWeather9() const
{
    return hasValidCity9() && (!(d->now9.weatherIcon().isEmpty()) && (d->now9.weatherIcon().size() > 1) && d->now9.weatherIcon() != "");
}
/*************************************************************************************/


WeatherData *AppModel::weather() const
{
    return &(d->now);
}
WeatherData *AppModel::weather1() const
{
    return &(d->now1);
}
WeatherData *AppModel::weather2() const
{
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
    return &(d->now8);
}
WeatherData *AppModel::weather9() const
{
    return &(d->now9);
}


QQmlListProperty<WeatherData> AppModel::forecast() const
{
    return *(d->fcProp);
}

bool AppModel::ready() const
{
    return d->ready;
}

bool AppModel::hasSource() const
{
    return (d->src != NULL);
}

bool AppModel::useGps() const
{
    return d->useGps;
}

void AppModel::setUseGps(bool value)
{
    d->useGps = value;
    if (value) {
        d->city = "";
        d->throttle.invalidate();
        emit cityChanged();
        emit weatherChanged();
    }
    emit useGpsChanged();
}

QString AppModel::city() const
{
    return d->city;
}
QString AppModel::city1() const
{
    return d->city1;
}
QString AppModel::city2() const
{
    return d->city2;
}
QString AppModel::city3() const
{
    return d->city3;
}
QString AppModel::city4() const
{
    return d->city4;
}
QString AppModel::city5() const
{
    return d->city5;
}
QString AppModel::city6() const
{
    return d->city6;
}
QString AppModel::city7() const
{
    return d->city7;
}
QString AppModel::city8() const
{
    return d->city8;
}
QString AppModel::city9() const
{
    return d->city9;
}

void AppModel::setCity(const QString &value)
{
    d->city = value;
    emit cityChanged();
    refreshWeather();
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

    d->coord1.setLatitude(lat);
    d->count = 1;
    return lat;
}
double AppModel::sendLongitude1(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord1.setLongitude(lon);
    return lon;
}
double AppModel::sendLatitude2(double lat) //qml에서 경도값을 받아오는 set함수
{

    d->coord2.setLatitude(lat);
    d->count = 2;
    return lat;
}
double AppModel::sendLongitude2(double lon) //qml에서 위도값을 받아오는 set함수
{
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
    queryCity();
}
double AppModel::sendZoomLevel(double lev){ //추후 줌했을때 실행할 함수
    if(lev >10.0 && lev <11.0){
    }
    return lev;
}
