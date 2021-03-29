TEMPLATE = subdirs
SUBDIRS += ParserJSON \
    TestParserJSON\
    ParserJSONLib


ParserJSON.depends = ParserJSONLib
TestParserJSON.depends = ParserJSONLib
