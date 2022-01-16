#-------------------------------------------------
#
# Project created by QtCreator 2012-04-21T09:28:41
#
#-------------------------------------------------

QT       += core gui network widgets sql
#QT       += core gui widgets

TARGET = mqtt_db
TEMPLATE = app

# include(qextserialport/src/qextserialport.pri)
#RESOURCES   += \
#               gcoder.qrc

SOURCES +=  main.cpp\
            LogTopicHandler.cpp \
            MQTT.cpp \
            PingTopicHandler.cpp \
            DBManager.cpp \
            QcjData/QcjHttpService.cpp \
            QcjData/QcjTcpService.cpp \
            QcjData/QcjSqlRestApi.cpp \
            QcjData/Logger.cpp \
            QcjData/LogRegistery.cpp \

HEADERS  += \
            LogTopicHandler.h \
            MQTT.h \
            PingTopicHandler.h \
            DBManager.h \
            QcjData/QcjTcpService.h \
            QcjData/QcjHttpServiceFactory.h \
            QcjData/QcjHttpService.h \
            QcjData/QcjSqlRestApi.h \
            QcjData/LogBuilder.h \
            QcjData/Logger.h \
            QcjData/LogRegistery.h \

include(./qmqtt/qmqtt.pro)
#include(./qmqtt/src/mqtt/qmqtt.pri)






