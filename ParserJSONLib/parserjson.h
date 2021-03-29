#ifndef PARSERJSON_H
#define PARSERJSON_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>


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
signals:
    void finished();
public slots:
    void run();
private:
    void getOldestActor(const QJsonDocument &jsonDocument);
    void requestActorData(const QJsonObject *jsonObject);
private:
    QNetworkAccessManager   *m_networkAccessManager;
    QJsonObject             *m_longestRuntimeObject;
    QJsonObject             *m_shortestRuntimeObject;
    QUrl                    *m_url;
};

#endif // PARSERJSON_H
