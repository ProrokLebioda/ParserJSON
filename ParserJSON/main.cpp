#include <QCoreApplication>
#include <QTimer>
#include "parserjson.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString url_string("http://api.tvmaze.com/schedule/web?date=2020-01-07");// provide default URL

    if (argc >1) //let's assume that user passes url as a console parametere when launching
        url_string = QString(argv[1]);
    ParserJSON *parserJson = new ParserJSON(url_string,&a);
    ParserJSON::connect(parserJson,&ParserJSON::finished,&a,&QCoreApplication::quit);

    QTimer::singleShot(0,parserJson,&ParserJSON::run);

    return a.exec();
}
