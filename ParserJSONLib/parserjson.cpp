#include "parserjson.h"
#include <exception>
#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QEventLoop>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>


ParserJSON::ParserJSON(QString url_string, QObject *parent) : QObject(parent)
{
    m_networkAccessManager = new QNetworkAccessManager(this);
    m_longestRuntimeObject = new QJsonObject();
    m_shortestRuntimeObject = new QJsonObject();
    m_url = new QUrl(url_string);
}

ParserJSON::~ParserJSON()
{
    delete m_longestRuntimeObject;
    delete m_shortestRuntimeObject;
    delete m_url;
}

QJsonDocument ParserJSON::getJSON(QUrl url)
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    // the HTTP request
    QNetworkRequest req(url);
    QNetworkReply *reply = m_networkAccessManager->get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called
    if (reply->error() != QNetworkReply::NoError)
    {
        m_networkAccessManager->clearAccessCache();
        qDebug() << "Failure" << reply->errorString();
        delete reply;
        return QJsonDocument();
    }
    else
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
        delete reply;
        return jsonDocument;
    }
}

void ParserJSON::displayShowData(const QJsonDocument &jsonDocument)
{
    QMap<int,int> premieresMap;
    int longestRuntime = 0;
    int shortestRuntime =std::numeric_limits<int>::max();

    if (jsonDocument.isArray())
    {
        QJsonArray jsonArray = jsonDocument.array();

        for (int i = 0; i<jsonArray.count(); i++)
        {
            //get premiered, that is nested in _embedded.show
            QJsonObject jsonObject2 = jsonArray[i].toObject();
            QJsonObject jsonObjectEmbedded = jsonObject2.value(EMBEDDED).toObject();
            QJsonObject jsonObjectShow = jsonObjectEmbedded.value(SHOW).toObject();
            if (!jsonObjectShow.value(PREMIERED).isNull())
            {
                //count premieres per year
                QString premieredString= jsonObjectShow.value(PREMIERED).toString();
                int year = premieredString.split("-").at(0).toInt();
                premieresMap[year]++;

                //find longest and shortest runtime (seems like from entire JSON, not per year... perhaps it should be)
                if (!jsonObjectShow.value(RUNTIME).isNull())
                {
                    int showRuntime = jsonObjectShow.value(RUNTIME).toInt();
                    if (showRuntime > longestRuntime)
                    {
                        longestRuntime = showRuntime;
                        *m_longestRuntimeObject = jsonObjectShow;
                    }

                    if (showRuntime < shortestRuntime)
                    {
                        shortestRuntime = showRuntime;
                        *m_shortestRuntimeObject = jsonObjectShow;
                    }
                }
            }
            else
            {
                qDebug() << jsonObject2.value(NAME).toString() << " "<< i << " status: null";
            }
        }
        qDebug() << "Premieres per year:";
        QMap<int,int>::iterator it;
        for(it = premieresMap.begin();it != premieresMap.end(); it++)
        {
            qDebug() << it.key()<< ": " << it.value();
        }

        qDebug() << "Longest runtime: " << m_longestRuntimeObject->value(RUNTIME).toInt() << "min, Shortest runtime: " << m_shortestRuntimeObject->value(RUNTIME).toInt() << "min";
    }
}

void ParserJSON::displayActorData()
{
    //longest running show's actor
    showOldestActorData(m_longestRuntimeObject);

    //shortest running show's actor
    showOldestActorData(m_shortestRuntimeObject);
}

void ParserJSON::run()
{
    //executes each step from task
    displayShowData(getJSON(*m_url));
    displayActorData();

    std::cout << "Press ENTER"<<std::endl;
    std::cin.get();
    emit finished();
}

const QJsonObject &ParserJSON::getOldestActor(const QJsonDocument &jsonDocument)
{
    QJsonObject oldestActor;
    try {
        if (jsonDocument.isNull())
            throw "JsonDocument is empty, cannot continue";
    }  catch (const char* message) {
        qDebug() << message;
        return oldestActor;
    }

    if (jsonDocument.isArray())
    {
        QDate oldestActorBirthdayDate;

        QJsonArray jsonArray = jsonDocument.array();
        for (int i = 0; i<jsonArray.count(); i++)
        {
            QJsonObject jsonObject= jsonArray[i].toObject();
            QJsonObject jsonObjectPerson = jsonObject.value(PERSON).toObject();
            QString actorBirthdayString = jsonObjectPerson.value(BIRTHDAY).toString().remove("-");
            QDate actorBirthdayDate = QDate::fromString(actorBirthdayString,"yyyyMMdd");
            if (oldestActorBirthdayDate.isNull()||oldestActor.isEmpty())
            {
                oldestActorBirthdayDate = actorBirthdayDate;
                oldestActor = jsonObjectPerson;
            }

            if(actorBirthdayDate.daysTo(oldestActorBirthdayDate) > 0)
            {
                oldestActorBirthdayDate=actorBirthdayDate;
                oldestActor = jsonObjectPerson;
            }
        }
        return oldestActor;

    }

    return oldestActor;
}

void ParserJSON::showOldestActorData(const QJsonObject *jsonObject)
{
    try {
        if(jsonObject->isEmpty())
        {
            throw "Exception! QJsonObject for show is empty!";
        }

        int idInt = jsonObject->value(ID).toInt();
        QString idStr=QString::number(idInt);
        QUrl url("http://api.tvmaze.com/shows/"+idStr+"/cast");

        QJsonObject oldestActor = getOldestActor(getJSON(url));

        try {
            if (oldestActor.isEmpty())
            {
                throw "Exception! Oldest actor object is empty, cannot show correct data";
            }
            qDebug()<<"Oldest actor in " << jsonObject->value(NAME).toString() << ": ";
            //birthday could just return value from JSON but let's change how it looks
            QString actorBirthdayString = oldestActor.value(BIRTHDAY).toString().remove("-");
            QDate actorBirthdayDate = QDate::fromString(actorBirthdayString,"yyyyMMdd");
            qDebug()<<"Name: " << oldestActor.value(NAME).toString() << ", birthday: " << actorBirthdayDate.toString();

        }  catch (const char* message) {
            qDebug() << message;
        }
    }  catch (const char* message) {
        qDebug() << message;
    }
}
