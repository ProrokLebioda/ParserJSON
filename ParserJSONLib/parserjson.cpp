#include "parserjson.h"

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
    QObject::connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
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

        //m_jsonDocument = new QJsonDocument(jsonDocument);
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
            QJsonObject jsonObjectEmbedded = jsonObject2.value("_embedded").toObject();
            QJsonObject jsonObjectShow = jsonObjectEmbedded.value("show").toObject();
            if (!jsonObjectShow.value("premiered").isNull())
            {
                //count premieres per year
                QString premieredString= jsonObjectShow.value("premiered").toString();
                int year = premieredString.split("-").at(0).toInt();
                premieresMap[year]++;

                //find longest and shortest runtime (seems like from entire JSON, not per year... perhaps it should be)
                if (!jsonObjectShow.value("runtime").isNull())
                {
                    int showRuntime = jsonObjectShow.value("runtime").toInt();
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
                qDebug() << jsonObject2.value("name").toString() << " "<< i << " status: null";
            }
        }
        qDebug() << "Premieres per year:";
        QMap<int,int>::iterator it;
        for(it = premieresMap.begin();it != premieresMap.end(); it++)
        {
            qDebug() << it.key()<< ": " << it.value();
        }

        qDebug() << "Longest runtime: " << m_longestRuntimeObject->value("runtime").toInt() << "min, Shortest runtime: " << m_shortestRuntimeObject->value("runtime").toInt() << "min";
    }
}

void ParserJSON::displayActorData()
{
    //longest running show's actor
    requestActorData(m_longestRuntimeObject);

    //shortest running show's actor
    requestActorData(m_shortestRuntimeObject);
}

void ParserJSON::run()
{
    //executes each step from task
    getJSON(*m_url);
    displayShowData(getJSON(*m_url));
    displayActorData();
    emit finished();
}

void ParserJSON::getOldestActor(const QJsonDocument &jsonDocument)
{
    if (jsonDocument.isArray())
    {
        QDate oldestActorBirthdayDate;
        QJsonObject oldestActor;
        QJsonArray jsonArray = jsonDocument.array();
        for (int i = 0; i<jsonArray.count(); i++)
        {
            QJsonObject jsonObject= jsonArray[i].toObject();
            QJsonObject jsonObjectPerson = jsonObject.value("person").toObject();
            QString actorBirthdayString = jsonObjectPerson.value("birthday").toString().remove("-");
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
        //birthday could just return value from JSON but let's change how it looks
        QString actorBirthdayString = oldestActor.value("birthday").toString().remove("-");
        QDate actorBirthdayDate = QDate::fromString(actorBirthdayString,"yyyyMMdd");
        qDebug()<<"Name: " << oldestActor.value("name").toString() << ", birthday: " << actorBirthdayDate.toString();
    }
}

void ParserJSON::requestActorData(const QJsonObject *jsonObject)
{
    //longest running
    int idInt = jsonObject->value("id").toInt();
    QString idStr=QString::number(idInt);
    QUrl url("http://api.tvmaze.com/shows/"+idStr+"/cast");
    qDebug()<<"Oldest actor in " << jsonObject->value("name").toString() << ": ";
    getOldestActor(getJSON(url));
}
