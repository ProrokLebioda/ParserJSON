TEMPLATE = subdirs
SUBDIRS += ParserJSONLib\
    ParserJSON \
    TestParserJSON



ParserJSON.depends = ParserJSONLib
TestParserJSON.depends = ParserJSONLib
