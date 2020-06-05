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

#ifndef APPMODEL_H
#define APPMODEL_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtNetwork/QNetworkReply>
#include <QtQml/QQmlListProperty>
#include <QQuickView>

#include <QtPositioning/QGeoPositionInfoSource>

//! [0]
class WeatherData : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString dayOfWeek READ dayOfWeek WRITE setDayOfWeek NOTIFY dataChanged)
    Q_PROPERTY(QString weatherIcon READ weatherIcon WRITE setWeatherIcon NOTIFY dataChanged)
    Q_PROPERTY(QString weatherDescription READ weatherDescription WRITE setWeatherDescription NOTIFY dataChanged)
    Q_PROPERTY(QString temperature READ temperature WRITE setTemperature NOTIFY dataChanged)
    //Q_PROPERTY(type name READ name WRITE setname NOTIFY nameChanged)
    //타입(type)과 읽을 때 함수(READ), 수정 시 함수(WRITE), 변경 시 호출할 시그널(NOTIFY) 작성

public:
    explicit WeatherData(QObject *parent = 0);
    WeatherData(const WeatherData &other);

    QString dayOfWeek() const;
    QString weatherIcon() const;
    QString weatherDescription() const;
    QString temperature() const;

    void setDayOfWeek(const QString &value);
    void setWeatherIcon(const QString &value);
    void setWeatherDescription(const QString &value);
    void setTemperature(const QString &value);

signals:
    void dataChanged();
    //! [0]
private:
    QString m_dayOfWeek;
    QString m_weather;
    QString m_weatherDescription;
    QString m_temperature;
    //! [1]
};
//! [1]

Q_DECLARE_METATYPE(WeatherData)

class AppModelPrivate;
//! [2]
class AppModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool hasSource READ hasSource NOTIFY readyChanged)

//    Q_PROPERTY(bool hasValidCity READ hasValidCity NOTIFY cityChanged)
    Q_PROPERTY(bool hasValidWeather READ hasValidWeather NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather1 READ hasValidWeather1 NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather2 READ hasValidWeather2 NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather3 READ hasValidWeather3 NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather4 READ hasValidWeather4 NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather5 READ hasValidWeather5 NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather6 READ hasValidWeather6 NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather7 READ hasValidWeather7 NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather8 READ hasValidWeather8 NOTIFY weatherChanged)
    Q_PROPERTY(bool hasValidWeather9 READ hasValidWeather9 NOTIFY weatherChanged)

    Q_PROPERTY(bool useGps READ useGps WRITE setUseGps NOTIFY useGpsChanged)

    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city1 READ city1 WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city2 READ city2 WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city3 READ city3 WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city4 READ city4 WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city5 READ city5 WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city6 READ city6 WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city7 READ city7 WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city8 READ city8 WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString city9 READ city9 WRITE setCity NOTIFY cityChanged)

    Q_PROPERTY(WeatherData *weather READ weather NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather1 READ weather1 NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather2 READ weather2 NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather3 READ weather3 NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather4 READ weather4 NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather5 READ weather5 NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather6 READ weather6 NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather7 READ weather7 NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather8 READ weather8 NOTIFY weatherChanged)
    Q_PROPERTY(WeatherData *weather9 READ weather9 NOTIFY weatherChanged)

    Q_PROPERTY(QQmlListProperty<WeatherData> forecast READ forecast NOTIFY weatherChanged)




public:
    explicit AppModel(QObject *pardnt = 0); //explicit 자신이 원하지 않은 형변환이 일어나지 않도록 제한하는 키워드
    ~AppModel();
    bool ready() const;
    bool hasSource() const;
    bool useGps() const;
//    bool hasValidCity() const; 도시값없으면 날씨 못불러오게 할려고 만들어놨던 함수
    bool hasValidWeather() const;
    bool hasValidWeather1() const;
    bool hasValidWeather2() const;
    bool hasValidWeather3() const;
    bool hasValidWeather4() const;
    bool hasValidWeather5() const;
    bool hasValidWeather6() const;
    bool hasValidWeather7() const;
    bool hasValidWeather8() const;
    bool hasValidWeather9() const;
    void setUseGps(bool value);
    void hadError(bool tryAgain);

    QString city() const;
    QString city1() const;
    QString city2() const;
    QString city3() const;
    QString city4() const;
    QString city5() const;
    QString city6() const;
    QString city7() const;
    QString city8() const;
    QString city9() const;



    void setCity(const QString &value);

    WeatherData *weather() const;
    WeatherData *weather1() const;
    WeatherData *weather2() const;
    WeatherData *weather3() const;
    WeatherData *weather4() const;
    WeatherData *weather5() const;
    WeatherData *weather6() const;
    WeatherData *weather7() const;
    WeatherData *weather8() const;
    WeatherData *weather9() const;
    QQmlListProperty<WeatherData> forecast() const;


public slots:
    Q_INVOKABLE void refreshWeather();
    double sendLatitude(double lat);//추가
    double sendLongitude(double lon);//추가
    double sendLatitude1(double lat);//추가
    double sendLongitude1(double lon);//추가
    double sendLatitude2(double lat);//추가
    double sendLongitude2(double lon);//추가
    double sendLatitude3(double lat);//추가
    double sendLongitude3(double lon);//추가
    double sendLatitude4(double lat);//추가
    double sendLongitude4(double lon);//추가
    double sendLatitude5(double lat);//추가
    double sendLongitude5(double lon);//추가
    double sendLatitude6(double lat);//추가
    double sendLongitude6(double lon);//추가
    double sendLatitude7(double lat);//추가
    double sendLongitude7(double lon);//추가
    double sendLatitude8(double lat);//추가
    double sendLongitude8(double lon);//추가
    double sendLatitude9(double lat);//추가
    double sendLongitude9(double lon);//추가
    double sendZoomLevel(double lev);

    void myQmlSlot();//추가




    //! [2]
private slots:
    void queryCity();
    void tidalCurrent();//추가
    void networkSessionOpened();
    void positionUpdated(QGeoPositionInfo gpsPos);
    void positionError(QGeoPositionInfoSource::Error e);
    void handleGeoNetworkData(QNetworkReply *networkReply);
    void handleWeatherNetworkData(QNetworkReply *networkReply);
    void handleForecastNetworkData(QNetworkReply *networkReply);
    void handleTidalCurrentNetworkData(QNetworkReply *networkReply);//추가


    //! [3]
signals:
    void readyChanged();
    void useGpsChanged();
    void cityChanged();
    void weatherChanged();
    void weatherChanged9();
    void weatherChanged8();
    void weatherChanged7();
    void latitudeChanged();



    //! [3]

private:
    AppModelPrivate *d;

    //! [4]
};
//! [4]

#endif // APPMODEL_H
