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
    m_weather(other.m_weather),
    m_weatherDescription(other.m_weatherDescription),
    m_temperature(other.m_temperature)
{
}

static QString niceTemperatureString(double t)
{
    return QString::number(qRound(t-ZERO_KELVIN)) + QChar(0xB0);
}

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
    QGeoPositionInfoSource *src;
    QGeoCoordinate coord;
    QString longitude, latitude;
    QNetworkAccessManager *nam;
    QNetworkSession *ns;
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


AppModel::AppModel(QObject *parent) : QObject(parent), d(new AppModelPrivate)
{
    d->nam = new QNetworkAccessManager(this);

    QNetworkConfigurationManager ncm;
    d->ns = new QNetworkSession(ncm.defaultConfiguration(), this);
    connect(d->ns, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    if (d->ns->isOpen())
        this->networkSessionOpened();
    d->ns->open();
}

AppModel::~AppModel()
{
    d->ns->close();
    if (d->src)
        d->src->stopUpdates();
    delete d;
}

void AppModel::networkSessionOpened()
{
    d->src = QGeoPositionInfoSource::createDefaultSource(this);
    if (d->src) {
        d->src->startUpdates();
    }
}

void AppModel::queryWeather1()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData1(rep); });
}
void AppModel::queryWeather2()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData2(rep); });
}
void AppModel::queryWeather3()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData3(rep); });
}
void AppModel::queryWeather4()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData4(rep); });
}
void AppModel::queryWeather5()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData5(rep); });
}
void AppModel::queryWeather6()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData6(rep); });
}
void AppModel::queryWeather7()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData7(rep); });
}
void AppModel::queryWeather8()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData8(rep); });
}
void AppModel::queryWeather9()
{
    d->latitude.setNum(d->coord.latitude());
    d->longitude.setNum(d->coord.longitude());
    QUrl url("http://api.openweathermap.org/data/2.5/weather?");
    QUrlQuery query;
    query.addQueryItem("lat", d->latitude);
    query.addQueryItem("lon", d->longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("APPID", d->app_ident);
    url.setQuery(query);
    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() {  handleWeatherNetworkData9(rep); });
}

void AppModel::handleWeatherNetworkData1(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now1.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now1.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now1.setTemperature(niceTemperatureString(valMain.toDouble()));
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();
    }
    networkReply->deleteLater();
}
void AppModel::handleWeatherNetworkData2(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now2.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now2.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now2.setTemperature(niceTemperatureString(valMain.toDouble()));
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();
    }
    networkReply->deleteLater();
}
void AppModel::handleWeatherNetworkData3(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now3.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now3.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now3.setTemperature(niceTemperatureString(valMain.toDouble()));
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();
    }
    networkReply->deleteLater();
}
void AppModel::handleWeatherNetworkData4(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now4.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now4.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now4.setTemperature(niceTemperatureString(valMain.toDouble()));
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();
    }
    networkReply->deleteLater();
}
void AppModel::handleWeatherNetworkData5(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now5.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now5.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now5.setTemperature(niceTemperatureString(valMain.toDouble()));
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();
    }
    networkReply->deleteLater();
}
void AppModel::handleWeatherNetworkData6(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now6.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now6.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now6.setTemperature(niceTemperatureString(valMain.toDouble()));
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();
    }
    networkReply->deleteLater();
}
void AppModel::handleWeatherNetworkData7(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now7.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now7.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now7.setTemperature(niceTemperatureString(valMain.toDouble()));
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();
    }
    networkReply->deleteLater();
}
void AppModel::handleWeatherNetworkData8(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now8.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now8.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now8.setTemperature(niceTemperatureString(valMain.toDouble()));
        }
        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }
        emit weatherChanged();
    }
    networkReply->deleteLater();
}
void AppModel::handleWeatherNetworkData9(QNetworkReply *networkReply)
{
    if (!networkReply){
        return;
    }
    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue valWeather;
            QJsonValue valMain;
                valWeather = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = valWeather.toArray();
                valWeather = weatherArray.at(0);
                tempObject = valWeather.toObject();
                d->now9.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now9.setWeatherIcon(tempObject.value("icon").toString());
                valMain = obj.value(QStringLiteral("main"));
                tempObject = valMain.toObject();
                valMain = tempObject.value(QStringLiteral("temp"));
                d->now9.setTemperature(niceTemperatureString(valMain.toDouble()));
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
    return (!(d->now5.weatherIcon().isEmpty()) && (d->now5.weatherIcon().size() > 1) && d->now5.weatherIcon() != "");
}
bool AppModel::hasValidWeather1() const
{
    return (!(d->now1.weatherIcon().isEmpty()) && (d->now1.weatherIcon().size() > 1) && d->now1.weatherIcon() != "");
}
bool AppModel::hasValidWeather2() const
{
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
    return (!(d->now8.weatherIcon().isEmpty()) && (d->now8.weatherIcon().size() > 1) && d->now8.weatherIcon() != "");
}
bool AppModel::hasValidWeather9() const
{
    return (!(d->now9.weatherIcon().isEmpty()) && (d->now9.weatherIcon().size() > 1) && d->now9.weatherIcon() != "");
}

WeatherData *AppModel::weather() const
{
    return &(d->now5);
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

bool AppModel::ready() const
{
    qCDebug(requestsLog) << "ready 함수";
    return d->ready;
}

double AppModel::sendLatitude(double lat) //qml에서 경도값을 받아오는 set함수
{
    d->coord.setLatitude(lat);
    return lat;
}
double AppModel::sendLongitude(double lon) //qml에서 위도값을 받아오는 set함수
{
    d->coord.setLongitude(lon);
    return lon;
}


void AppModel::myQmlSlot1()
{
    queryWeather1();
}
void AppModel::myQmlSlot2()
{
    queryWeather2();
}
void AppModel::myQmlSlot3()
{
    queryWeather3();
}
void AppModel::myQmlSlot4()
{
    queryWeather4();
}
void AppModel::myQmlSlot5()
{
    queryWeather5();
}
void AppModel::myQmlSlot6()
{
    queryWeather6();
}
void AppModel::myQmlSlot7()
{
    queryWeather7();
}
void AppModel::myQmlSlot8()
{
    queryWeather8();
}
void AppModel::myQmlSlot9()
{
    queryWeather9();
}
