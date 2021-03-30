#ifndef PARSERJSON_H
#define PARSERJSON_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

#define BIRTHDAY    "birthday"
#define EMBEDDED    "_embedded"
#define ID          "id"
#define NAME        "name"
#define PERSON      "person"
#define PREMIERED   "premiered"
#define RUNTIME     "runtime"
#define SHOW        "show"

class ParserJSON : public QObject
{
    Q_OBJECT
public:
    explicit ParserJSON(QString url_string = "http://api.tvmaze.com/schedule/web?date=2020-01-07", QObject *parent = nullptr);
    ~ParserJSON();
    QJsonDocument getJSON(QUrl url);
    //void getActorInformation();
    void displayShowData(const QJsonDocument &jsonDocument);
    void displayActorData();
    const QJsonObject &getOldestActor(const QJsonDocument &jsonDocument);
signals:
    void finished();
public slots:
    void run();
private:
    void showOldestActorData(const QJsonObject *jsonObject);
private:
    QNetworkAccessManager   *m_networkAccessManager;
    QJsonObject             *m_longestRuntimeObject;
    QJsonObject             *m_shortestRuntimeObject;
    QUrl                    *m_url;
};

#endif // PARSERJSON_H
