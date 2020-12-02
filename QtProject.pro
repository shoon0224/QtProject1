TEMPLATE = app
#TEMPLATE는 프로젝트 파일의 타입을 정한다.
#여기서 'app'은 독립적인 어플리케이션이며 default값이다. 생략하면 app이 기본이다.
#----------------------------------------------------------------------------------------------

#TARGET = QtProject
#실행파일의 이름이다. 보통 gcc컴파일 시 -o 옵션 뒤에 붙는 이름이다.
#생략 시 .pro 파일 앞에 붙는 이름이 default값이 된다.

#----------------------------------------------------------------------------------------------

QT += core network positioning qml quick
QT += widgets
requires(qtConfig(bearermanagement))
#프로젝트에 사용될 Qt의 모듈을 지정한다. default값은 core와 gui이다.
#이는 QtGui 모듈을 포함한다는 의미
#QT에 지정되는 모듈은 대략 core gui network opengl sql svg xml xmlpatterns qt3support 등이 있다.
#Qt Creator에서 Qt 모듈을 클릭하여 설정하는데 설정 시 .pro 파일에 선택된 모듈들이 포함되는 것을 볼 수 있다.

#----------------------------------------------------------------------------------------------

CONFIG += c++11
#CONFIG 변수는 빌드시 다양한 옵션들을 제어하기 위해 사용된다.
# https://saengjja.tistory.com/145?category=705849 (더 자세한 정보는 여기)

#----------------------------------------------------------------------------------------------

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#Define.gcc 컴파일 시 -D 옵션뒤에 붙는 옵션

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#----------------------------------------------------------------------------------------------

SOURCES += \
        appmodel.cpp \
        main.cpp
#소스파일이다.

#----------------------------------------------------------------------------------------------

RESOURCES += qml.qrc
#rcc에 의해 처리되는 실행파일내에 포함되는 리소스파일을 정의한 xml파일 .qrc를 지정한다.

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#HEADERS
HEADERS += \
    appmodel.h
#헤더파일을 적는다.


