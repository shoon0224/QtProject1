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

/*
 *This application uses http://openweathermap.org/api
 **/

#define ZERO_KELVIN 273.15

Q_LOGGING_CATEGORY(requestsLog,"wapp.requests")

WeatherData::WeatherData(QObject *parent) :
        QObject(parent)
{
    qDebug(requestsLog) << "WeatherData 함수 실행";
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
/* WeatherData의 set함수들*/
/* ************************************************************** */
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
/* *******************여기까지 *WeatherData Q_PROPERTY 구현문들***************************** */
class AppModelPrivate // 아래에 선언된 포인터변수 d로 참조되는 멤버들
{
public:
    static const int baseMsBeforeNewRequest = 1; // 5 s, increased after each missing answer up to 10x (원래 값은 5 *1000 5초였다 0.001초로 줄임 바로반응위해)
    QGeoPositionInfoSource *src;//위치 소스
    QGeoCoordinate coord; //좌표
    QString longitude, latitude; //경도 위
    QString city; //도시
    QNetworkAccessManager *nam;// 네트워크연결관리자
    QNetworkSession *ns;//네트워크세션
    WeatherData now; //날씨데이터
    QList<WeatherData*> forecast; //예보
    QQmlListProperty<WeatherData> *fcProp;//예보속성
    bool ready;
    bool useGps; 
    QElapsedTimer throttle; // 경과한시간
    int nErrors;
    int minMsBeforeNewRequest;
    QTimer delayedCityRequestTimer;
    QTimer requestNewWeatherTimer;
    QString app_ident;

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
    qCDebug(requestsLog) << "forcastAppend함수 실행";
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
    qDebug(requestsLog) << "appmodel 함수 실행";
//! [0]
    d->fcProp = new QQmlListProperty<WeatherData>(this, d,
                                                          forecastAppend,
                                                          forecastCount,
                                                          forecastAt,
                                                          forecastClear);

    connect(&d->delayedCityRequestTimer, SIGNAL(timeout()), this, SLOT(queryCity()));//timeout은 QTimer class에 시그널로 선언되어있다.
    connect(&d->requestNewWeatherTimer, SIGNAL(timeout()), this, SLOT(refreshWeather()));
    d->requestNewWeatherTimer.start();


//! [1]
    // make sure we have an active network session
    d->nam = new QNetworkAccessManager(this);

    QNetworkConfigurationManager ncm; //네트워크설정관리자
    d->ns = new QNetworkSession(ncm.defaultConfiguration(), this);
    connect(d->ns, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    // the session may be already open. if it is, run the slot directly
    if (d->ns->isOpen())
        this->networkSessionOpened();
    // 네트워크 필요하다고 시스템에게 말해라.
    d->ns->open();
}
//! [1]

AppModel::~AppModel()
{
    d->ns->close();
    if (d->src)
        d->src->stopUpdates();
    delete d;
    qDebug(requestsLog) << "appmodel 소멸자함수 실행";
}

//! [2]
void AppModel::networkSessionOpened()
{
    qDebug(requestsLog) << "networkSessionOpend 함수 실행";
    d->src = QGeoPositionInfoSource::createDefaultSource(this);

    if (d->src) {
        d->useGps = true;
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
void AppModel::positionUpdated(QGeoPositionInfo) //여기 gpspos안에 내 현재위치 좌표값이 담겨있다 이 파라미터값을 qml에서 받아온 좌표를 넣으면 될것같다.
{
    qCDebug(requestsLog) << "PositionUpdated함수 실행";
//    d->coord = gpsPos.coordinate(); 이 코드를 추석처리하지 않으면 계속 gps값이 덮어 씌워짐

    if (!(d->useGps))
        return;
    qDebug(requestsLog) << "coord값: " <<d->coord ;
    queryCity();
}
//! [3]

void AppModel::queryCity()
{
    qCDebug(requestsLog) << "queryCity함수 실행";
    //don't update more often then once a minute
    //to keep load on server low
    //서버 로드를 낮게 유지하기 위해, 1분에 한번이상 업데이트 하지마시오
    if (d->throttle.isValid() && d->throttle.elapsed() < d->minMsBeforeNewRequest ) {
        //isValid는 데이터가 유효한지 안한지 booltype으로 판단한다.
        qCDebug(requestsLog) << "delaying query of city";
        if (!d->delayedCityRequestTimer.isActive())
            d->delayedCityRequestTimer.start();
        return ;
    }

    qDebug(requestsLog) << "requested query of city";
    d->throttle.start();
    d->minMsBeforeNewRequest = (d->nErrors + 1) * d->baseMsBeforeNewRequest;
    QString latitude, longitude;
    longitude.setNum(d->coord.longitude());
    latitude.setNum(d->coord.latitude());


    qDebug(requestsLog) << "latitude출력: " <<latitude ;
    qDebug(requestsLog) << "longitude출력: " <<longitude ;

    /* latitude() 함수의 경로를 따라가보면 함수뒤에 const가 붙은 구문이 있는데 이건 이 함수 안에서는 어떤 변수도 바꿀 수 없음을 뜻한다. */
    /* 함수가 클래스 멤버인 경우에만 const 키워드를 함수 뒤에 삽입 할 수 있으며 해당 함수가 속한 객체의 멤버 변수를 변경 할 수 없다는 뜻이다.*/
    /* 또한 const가 붙은 함수 내에서는 const가 붙은 다른 함수를 제외한 일반 함수는 호출하지 못한다.*/

    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", latitude); // key="lat", value=latitude --> 이값이 위에서 넣은 latitude
    query.addQueryItem("lon", longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    qCDebug(requestsLog) << "submitting request";

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right away
    connect(rep, &QNetworkReply::finished, this, [this, rep]() { handleGeoNetworkData(rep); });

    return ;
}

void AppModel::positionError(QGeoPositionInfoSource::Error e)
{
    qCDebug(requestsLog) << "positionError함수 실행";
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
    qCDebug(requestsLog) << "hadError함수 실행";
    qCDebug(requestsLog) << "hadError, will " << (tryAgain ? "" : "not ") << "rety";
    d->throttle.start();
    if (d->nErrors < 10)
        ++d->nErrors;
    d->minMsBeforeNewRequest = (d->nErrors + 1) * d->baseMsBeforeNewRequest;
    if (tryAgain)
        d->delayedCityRequestTimer.start();
}

void AppModel::handleGeoNetworkData(QNetworkReply *networkReply)
{
    qCDebug(requestsLog) << "handleGeoNetworkData함수 실행";
    if (!networkReply) {
        hadError(false); // should retry?
        return;
    }
    if (!networkReply->error()) {
        d->nErrors = 0;
        if (!d->throttle.isValid()) // isValid 데이터가 유효하지 않은 경우 0, 그렇지 않은경우 1을 반환한다.
            d->throttle.start();
        d->minMsBeforeNewRequest = d->baseMsBeforeNewRequest;
        //convert coordinates to city name //좌표를 도시이름으로 변환하는 코드
        /****************************************************************************/
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        qDebug(requestsLog) << "document 값: " << document;
        QJsonObject jo = document.object();
        QJsonValue jv = jo.value(QStringLiteral("name"));

        const QString city = jv.toString();
        qCDebug(requestsLog) << "got city: " << city;
        if (city != d->city) {
            d->city = city;
            emit cityChanged();
            refreshWeather();
        }
    }
    hadError(true);// 원래 else로 있었음 그래야 if문 종료하고 다시 도시 검색실행을 하지않음

    networkReply->deleteLater();
}

void AppModel::refreshWeather()
{
    qCDebug(requestsLog) << "refreshWeather함수 실행";
    if (d->city.isEmpty()) { //isEmpty의 트루폴스 여부는 어떻게 아는가?
        qCDebug(requestsLog) << "refreshing weather skipped (no city)";
        return;
    }
    qCDebug(requestsLog) << "날씨 새로고침(refreshWeather함수 실행)";
    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;

    query.addQueryItem("q", d->city);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    qDebug(requestsLog) << "도시출력 :" << d->city ;
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right away //시그널을 바로 연결
    connect(rep, &QNetworkReply::finished, this, [this, rep]() { handleWeatherNetworkData(rep); });
}

static QString niceTemperatureString(double t)
{
    qCDebug(requestsLog) << "niceTemperatureString함수 실행";
    return QString::number(qRound(t-ZERO_KELVIN)) + QChar(0xB0);
}

void AppModel::handleWeatherNetworkData(QNetworkReply *networkReply)
{
    qCDebug(requestsLog) << "handleWeatherNetworkData함수 실행";
    qCDebug(requestsLog) << "got weather network data";
    if (!networkReply)
        return;

    if (!networkReply->error()) {
        foreach (WeatherData *inf, d->forecast)
            delete inf;
        d->forecast.clear();

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
                d->now.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now.setWeatherIcon(tempObject.value("icon").toString());
            }
            if (obj.contains(QStringLiteral("main"))) {
                val = obj.value(QStringLiteral("main"));
                tempObject = val.toObject();
                val = tempObject.value(QStringLiteral("temp"));
                d->now.setTemperature(niceTemperatureString(val.toDouble()));
            }
        }
    }
    networkReply->deleteLater();

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
}

void AppModel::handleForecastNetworkData(QNetworkReply *networkReply)
{
    qCDebug(requestsLog) << "handleForecasetNetworkData함수 실행";
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

        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }

        emit weatherChanged();
    }
    networkReply->deleteLater();
}

bool AppModel::hasValidCity() const
{
    qCDebug(requestsLog) << "hasValidCity함수 실행";
    return (!(d->city.isEmpty()) && d->city.size() > 1 && d->city != ""); //도시값이 비어있지 않고 도시크기값이 1보다 크고 도시의 스트링값이 비어있지 않으면 참을 리턴 즉 존재함을 보여주는 함수이다.
}

bool AppModel::hasValidWeather() const
{
    qCDebug(requestsLog) << "hasValidWeather함수 실행";
    return hasValidCity() && (!(d->now.weatherIcon().isEmpty()) && //isEmpty 개체가 갖고있는 문자열이 비어 있는지 조사한다.
                             (d->now.weatherIcon().size() > 1) &&
                              d->now.weatherIcon() != "");
}


WeatherData *AppModel::weather() const
{
    return &(d->now);
}

QQmlListProperty<WeatherData> AppModel::forecast() const
{
    return *(d->fcProp);
}

bool AppModel::ready() const
{
    qCDebug(requestsLog) << "ready함수 실행";
    return d->ready;
}

bool AppModel::hasSource() const
{
    qCDebug(requestsLog) << "hasSource함수 실행";
    return (d->src != NULL);
}

bool AppModel::useGps() const
{
    qCDebug(requestsLog) << "useGps함수 실행";
    return d->useGps;
}

void AppModel::setUseGps(bool value)
{
   qCDebug(requestsLog) << "setUseGps함수 실행";
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
    qCDebug(requestsLog) << "city함수 실행";
    return d->city;
}


void AppModel::setCity(const QString &value)
{
    qCDebug(requestsLog) << "setCity함수 실행";
    d->city = value;
    emit cityChanged();
    refreshWeather();
}

double AppModel::sendLatitude(double lat) //qml에서 경도값을 받아오는 set함수
{
    qDebug() <<"sendLatitude의 호출값"<< lat;
    d->coord.setLatitude(lat);
    qDebug() <<"coord객체의 latitude함수로 들어간 lat값" <<d->coord.latitude();
    return lat;
}
double AppModel::sendLongitude(double lon) //qml에서 위도값을 받아오는 set함수
{
    qDebug() <<"sendLongitude의 호출값"<< lon;
    d->coord.setLongitude(lon);
    qDebug() <<"coord객체의 longitude함수로 들어간 lon값" << d->coord.longitude();
    return lon;
}
