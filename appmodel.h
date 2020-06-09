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

class WeatherData : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString weatherIcon READ weatherIcon WRITE setWeatherIcon NOTIFY dataChanged)
    Q_PROPERTY(QString weatherDescription READ weatherDescription WRITE setWeatherDescription NOTIFY dataChanged)
    Q_PROPERTY(QString temperature READ temperature WRITE setTemperature NOTIFY dataChanged)

public:
    explicit WeatherData(QObject *parent = 0);
    WeatherData(const WeatherData &other);

    QString weatherIcon() const;
    QString weatherDescription() const;
    QString temperature() const;

    void setWeatherIcon(const QString &value);
    void setWeatherDescription(const QString &value);
    void setTemperature(const QString &value);

signals:
    void dataChanged();
private:
    QString m_weather;
    QString m_weatherDescription;
    QString m_temperature;

};


Q_DECLARE_METATYPE(WeatherData)

class AppModelPrivate;
//! [2]
class AppModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
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

public:
    explicit AppModel(QObject *pardnt = 0); //explicit 자신이 원하지 않은 형변환이 일어나지 않도록 제한하는 키워드
    ~AppModel();
    bool ready() const;
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

public slots:
    double sendLatitude(double lat);
    double sendLongitude(double lon);
    void myQmlSlot1();
    void myQmlSlot2();
    void myQmlSlot3();
    void myQmlSlot4();
    void myQmlSlot5();
    void myQmlSlot6();
    void myQmlSlot7();
    void myQmlSlot8();
    void myQmlSlot9();

private slots:
    void queryWeather1();
    void queryWeather2();
    void queryWeather3();
    void queryWeather4();
    void queryWeather5();
    void queryWeather6();
    void queryWeather7();
    void queryWeather8();
    void queryWeather9();
    void networkSessionOpened();
    void handleWeatherNetworkData1(QNetworkReply *networkReply);
    void handleWeatherNetworkData2(QNetworkReply *networkReply);
    void handleWeatherNetworkData3(QNetworkReply *networkReply);
    void handleWeatherNetworkData4(QNetworkReply *networkReply);
    void handleWeatherNetworkData5(QNetworkReply *networkReply);
    void handleWeatherNetworkData6(QNetworkReply *networkReply);
    void handleWeatherNetworkData7(QNetworkReply *networkReply);
    void handleWeatherNetworkData8(QNetworkReply *networkReply);
    void handleWeatherNetworkData9(QNetworkReply *networkReply);



signals:
    void readyChanged();
    void weatherChanged();


private:
    AppModelPrivate *d;

};


#endif // APPMODEL_H
