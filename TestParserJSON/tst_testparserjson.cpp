#include <QtTest>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "parserjson.h"

// add necessary includes here

class TestParserJSON : public QObject
{
    Q_OBJECT

public:
    TestParserJSON();
    ~TestParserJSON();

private slots:
    void testParserJSONConstructor();
    void testGetJson();
    void testGetOldestActor();
};

TestParserJSON::TestParserJSON()
{

}

TestParserJSON::~TestParserJSON()
{

}

void TestParserJSON::testParserJSONConstructor()
{
    ParserJSON *parser = new ParserJSON();
    bool isWorking = parser != nullptr;
    delete parser;
    QCOMPARE(isWorking,true);
}

void TestParserJSON::testGetJson()
{
    //test if method returns a valid QJsonDocument
    ParserJSON* parser = new ParserJSON();
    QJsonDocument* docParser = new QJsonDocument(parser->getJSON(QUrl("http://api.tvmaze.com/schedule/web?date=2020-01-07")));
    bool isWorking = docParser!=nullptr;
    delete docParser;
    delete parser;
    QCOMPARE(isWorking, true);
}

void TestParserJSON::testGetOldestActor()
{
    //test data, could be loaded from disc, but that could also provide issues...
    QJsonObject jsonObject1;
    QJsonObject jsonObject1a;
    jsonObject1a["name"] = "Tom Hanks";
    jsonObject1a["birthday"] = "1956-07-09";
    jsonObject1["person"] = jsonObject1a;

    QJsonObject jsonObject2;
    QJsonObject jsonObject2a;
    jsonObject2a["name"] = "Tom Cruise";
    jsonObject2a["birthday"] = "1962-07-03";
    jsonObject2["person"] = jsonObject2a;
    QJsonArray jsonArray;
    jsonArray.append(jsonObject1);
    jsonArray.append(jsonObject2);

    QJsonDocument jsonDocument;
    jsonDocument.setArray(jsonArray);
    ParserJSON *parser= new ParserJSON();
    QJsonObject returnJsonObject = parser->getOldestActor(jsonDocument);

    QCOMPARE(returnJsonObject.value("name"),"Tom Hanks");
    delete parser;

}

QTEST_GUILESS_MAIN(TestParserJSON)

#include "tst_testparserjson.moc"
