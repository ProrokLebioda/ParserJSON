#include <QtTest>

// add necessary includes here

class TestParserJSON : public QObject
{
    Q_OBJECT

public:
    TestParserJSON();
    ~TestParserJSON();

private slots:
    void test_case1();

};

TestParserJSON::TestParserJSON()
{

}

TestParserJSON::~TestParserJSON()
{

}

void TestParserJSON::test_case1()
{

}

QTEST_APPLESS_MAIN(TestParserJSON)

#include "tst_testparserjson.moc"
