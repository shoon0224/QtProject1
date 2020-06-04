#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>
#include <QLoggingCategory>
#include "appmodel.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    qDebug() << "main함수 실행";

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    qmlRegisterType<WeatherData>("WeatherInfo", 1, 0, "WeatherData");
    qmlRegisterType<AppModel>("WeatherInfo", 1, 0, "AppModel");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
//추가
//        return -1;
//    AppModel *event=new AppModel();
//    QObject *root=engine.rootObjects()[0];
//    event->setWindow(qobject_cast<QQuickWindow *>(root));
//    if(engine.rootObjects().isEmpty())



    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    //main.cpp에서는 main.qml를 로드한다.

    return app.exec();
}
