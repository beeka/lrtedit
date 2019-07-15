//  This file is part of LrtEdit.
//
// LrtEdit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LrtEdit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LrtEdit.  If not, see <https://www.gnu.org/licenses/>.
#include <QtTest>

// add necessary includes here
#include "luagenerator.h"
#include "luaparser.h"

class TestLuaParser : public QObject
{
    Q_OBJECT

   public:
    TestLuaParser();
    ~TestLuaParser();

   private slots:
    void test_emptyTable();
    void test_list();
    void test_dict();
    void test_mix();
    void test_case2a();
    void test_case2b();
    void test_case2c();
    void test_setAttr();
    void test_file();
    void test_number_list();

    // TODO: generator tests should be moved out to a separate test
    void test_generator();
    void test_generator_mix();
};

using namespace LuaParser;

TestLuaParser::TestLuaParser()
{

}

TestLuaParser::~TestLuaParser()
{

}

void TestLuaParser::test_emptyTable()
{
    const NamedVariant nv = parseLuaStruct("s = {}");
    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("s"));

    QVERIFY(nv.value().canConvert<Table>());
    const Table t = nv.value().value<Table>();
    QCOMPARE(t.hash(), 0);
    QCOMPARE(t.keys().size(), 0);
}

void TestLuaParser::test_list()
{
    const NamedVariant nv = parseLuaStruct(
        "days = {\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"}");

    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("days"));

    QVERIFY(nv.value().canConvert<Table>());
    const Table t = nv.value().value<Table>();
    QCOMPARE(t.hash(), 7);
    QCOMPARE(t.keys().size(), 0);
}

void TestLuaParser::test_number_list()
{
    QSKIP("Lists of numbers don't work, but not strictly needed for LRTedit");

    const NamedVariant nv = parseLuaStruct("testing = {1, 2, 3, 4}");

    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("testing"));

    QVERIFY(nv.value().canConvert<Table>());
    const Table t = nv.value().value<Table>();
    QCOMPARE(t.hash(), 4);
    QCOMPARE(t.keys().size(), 0);
}

void TestLuaParser::test_dict()
{
    const QString s =
        ("pages = {\n"
         "  actualBookHeight = 783,\n"
         "  actualBookWidth = 909,\n"
         "}");

    const NamedVariant nv = parseLuaStruct(s);
    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("pages"));

    QVERIFY(nv.value().canConvert<Table>());
    const Table t = nv.value().value<Table>();
    QCOMPARE(t.hash(), 0);
    QCOMPARE(t.keys().size(), 2);
}

void TestLuaParser::test_mix()
{
    const QString s =
        ("polyline = {color=\"blue\", thickness=2, npoints=4,\n"
         "    {x=0,   y=0},\n"
         "    {x=-10, y=0},\n"
         "    {x=-10, y=1},\n"
         "    {x=0,   y=1}\n"
         "}");

    const NamedVariant nv = parseLuaStruct(s);
    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("polyline"));

    QVERIFY(nv.value().canConvert<Table>());
    const Table t = nv.value().value<Table>();
    QCOMPARE(t.hash(), 4);
    QCOMPARE(t.keys().size(), 3);
    // polyline.color == "blue"
    // polyline[2].x == -1
}

void TestLuaParser::test_case2a()
{
    const QString s =
        ("pages = {\n"
         "  actualBookHeight = 783,\n"
         "  actualBookWidth = 909,\n"
         "  backgrounds = {\n"
         "  },\n"
         "  bookHeight = 783,\n"
         "  bookWidth = 909,\n"
         "  covers = {\n"
         "    },\n"
         "  defaultPageId = \"A1CC9F29-8696-4006-BC99-DFBC4BB0979B\",\n"
         "  hints = {\n"
         "      bookTitle = \"$$$/BookTemplate/Custom13x11/BookTitle=Custom\",\n"
         "      hintType = \"bookOptions\",\n"
         "      paperId = \"13x11-blurb\",\n"
         "      styleName = \"custom\",\n"
         "  },\n"
         "  paperId = \"13x11-blurb\",\n"
         "}");

    const NamedVariant nv = parseLuaStruct(s);
    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("pages"));

    QVERIFY(nv.value().canConvert<Table>());
    const Table t = nv.value().value<Table>();
    QCOMPARE(t.hash(), 0);
    QCOMPARE(t.keys().size(), 9);
    QCOMPARE(t["hints"].value<Table>().keys().size(), 4);

    // Test attribute fetching (should be a separate test?)
    QCOMPARE(t.getString("hints/styleName"), QString("custom"));

    {
        Table dynamic(t);

        QCOMPARE(dynamic.getString("paperId"), QString("13x11-blurb"));
        dynamic.setString("paperId", "MyPaper");
        QCOMPARE(dynamic.getString("paperId"), QString("MyPaper"));

        QCOMPARE(dynamic.getString("hints/styleName"), QString("custom"));
        dynamic.setString("hints/styleName", "MyStyle");
        QCOMPARE(dynamic.getString("hints/styleName"), QString("MyStyle"));
    }
}

void TestLuaParser::test_case2b()
{
    const QString s =
        ("s = {\n"
         "    id = \"0BE7F379-3DC2-422B-AC14-50FA53C76C83\",\n"
         "    internalName = \"clean13x11\",\n"
         "    title = \"clean13x11\",\n"
         "    type = \"layoutStyle\",\n"
         "    value = {\n"
         "        paperId = \"13x11-blurb\",\n"
         "        resources = \"clean13x11\",\n"
         "        styleName = \"clean\",\n"
         "        templateId = \"A6B33CF4-0FA5-4F86-9A95-FED959EEA7B2\",\n"
         "        },\n"
         "    }\n");

    const NamedVariant nv = parseLuaStruct(s);
    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("s"));

    QVERIFY(nv.value().canConvert<Table>());
    const Table t = nv.value().value<Table>();
    QCOMPARE(t.hash(), 0);
    QCOMPARE(t.keys().size(), 5);
}

void TestLuaParser::test_case2c()
{
    const QString s =
        ("s = {\n"
         "    {\n"
         "        width = 495,\n"
         "        height = 495,\n"
         "        directory = \"7x7-blurb\",\n"
         "        title = ZSTR \"$$$/Layout/TemplateSizes/7x7=Small Square\",\n"
         "        title_metric = ZSTR \"$$$/Layout/TemplateSizes/7x7_metric=Small Square 18x18 cm\",\n"
         "        sizeStr = ZSTR \"$$$/Layout/TemplateSizes/Dimensions/7x7=7 x 7 in (18 x 18 cm)\",\n"
         "        imageWrap = true,\n"
         "        softcover = true,\n"
         "        dustJacket = true,\n"
         "        id = \"7x7-blurb\",\n"
         "    },\n"
         "    {\n"
         "        width = 567,\n"
         "        height = 720,\n"
         "        directory = \"8x10-blurb\",\n"
         "        title = ZSTR \"$$$/Layout/TemplateSizes/8x10=Standard Portrait\",\n"
         "        title_metric = ZSTR \"$$$/Layout/TemplateSizes/8x10_metric=Standard Portrait 20x25 cm\",\n"
         "        sizeStr = ZSTR \"$$$/Layout/TemplateSizes/Dimensions/8x10=8 x 10 in (20 x 25 cm)\",\n"
         "        imageWrap = true,\n"
         "        softcover = true,\n"
         "        dustJacket = true,\n"
         "        id = \"8x10-blurb\",\n"
         "    },\n"
         " }\n");

    const NamedVariant nv = parseLuaStruct(s);
    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("s"));

    QVERIFY(nv.value().canConvert<Table>());
    const Table t = nv.value().value<Table>();
    QCOMPARE(t.hash(), 2);
    QCOMPARE(t.keys().size(), 0);

    const Table t1 = t[1].value<Table>();
    QCOMPARE(t1.hash(), 0);
    QCOMPARE(t1.keys().size(), 10);
    QCOMPARE(t1["width"].toInt(), 495);

    const Table t2 = t[2].value<Table>();
    QCOMPARE(t2.hash(), 0);
    QCOMPARE(t2.keys().size(), 10);
    QCOMPARE(t2["height"].toInt(), 720);
}

void TestLuaParser::test_setAttr()
{
    const QString s =
        ("s = {\n"
         "    id = \"0BE7F379-3DC2-422B-AC14-50FA53C76C83\",\n"
         "    internalName = \"clean13x11\",\n"
         "    title = \"clean13x11\",\n"
         "    type = \"layoutStyle\",\n"
         "    {\n"
         "        paperId = \"13x11-blurb\",\n"
         "        resources = \"clean13x11\",\n"
         "        styleName = \"clean\",\n"
         "        templateId = \"A6B33CF4-0FA5-4F86-9A95-FED959EEA7B2\",\n"
         "    },\n"
         "  }\n");

    const NamedVariant nv = parseLuaStruct(s);
    // qDebug() << LuaGenerator::Generate(nv);
    QCOMPARE(nv.name(), QString("s"));

    QVERIFY(nv.value().canConvert<Table>());
    Table t = nv.value().value<Table>();

    QCOMPARE(t.getString("type"), QStringLiteral("layoutStyle"));
    QCOMPARE(t.getString("1/styleName"), QStringLiteral("clean"));

    t.setString("type", "changed");
    QCOMPARE(t.getString("type"), QStringLiteral("changed"));

    t.setString("1/styleName", "dirty");
    QCOMPARE(t.getString("1/styleName"), QStringLiteral("dirty"));
}

void TestLuaParser::test_file()
{
    const QString path =
        "C:\\Program Files\\Adobe\\Adobe Lightroom\\Templates\\Layout "
        "Templates\\13x11-blurb\\clean13x11\\templatePages.lua";
    if (QFile::exists(path))
    {
        const NamedVariant nv = readLuaStruct(path);
        // qDebug() << LuaGenerator::Generate(nv);
        QCOMPARE(nv.name(), QString("pages"));

        QVERIFY(nv.value().canConvert<Table>());
        const Table t = nv.value().value<Table>();
        // QCOMPARE(t.hash(), 4);
        // QCOMPARE(t.keys().size(), 3);
    }
    else
    {
        QSKIP("templatePages.lua file is not present");
    }
}

void TestLuaParser::test_generator()
{
    const QString s =
        ("transform = {\n"
         "   angle = 0,\n"
         "   height = 435.5,\n"
         "   width = 580.09771728516,\n"
         "   x = 0,\n"
         "   y = 348,\n"
         "   }");

    const NamedVariant nv = parseLuaStruct(s);
    QCOMPARE(nv.name(), QString("transform"));

    const QString t = LuaGenerator::Generate(nv);

    // Compare the output line-by-line so we can make sense of errors.
    // The line is trimmed to remove whitespace, as we generate with spaces but LR uses tabs
    const QStringList a = s.split('\n');
    const QStringList b = t.split('\n');
    for (int i = 0; i < a.size(); i++)
    {
        //        qDebug() << i << a[i].trimmed();
        //        qDebug() << i << b[i].trimmed();
        QCOMPARE(b[i].trimmed(), a[i].trimmed());
    }
}

void TestLuaParser::test_generator_mix()
{
    const QString s =
        ("polyline = {\n"
         "   {\n"
         "      x = 0,\n"
         "      y = 0,\n" /* omit comma on list printing? */
         "   },\n"
         "   {\n"
         "      x = -10,\n"
         "      y = 42,\n"
         "   },\n"
         "   color = \"blue\",\n"
         "   npoints = 4,\n"
         "   thickness = 2,\n"
         "}");

    const NamedVariant nv = parseLuaStruct(s);
    qDebug() << LuaGenerator::Generate(nv);

    QCOMPARE(nv.name(), QString("polyline"));

    const QString t = LuaGenerator::Generate(nv);

    // Compare the output line-by-line so we can make sense of errors.
    // The line is trimmed to remove whitespace, as we generate with spaces but LR uses tabs
    const QStringList a = s.split('\n');
    const QStringList b = t.split('\n');
    for (int i = 0; i < a.size(); i++)
    {
        //        qDebug() << i << a[i].trimmed();
        //        qDebug() << i << b[i].trimmed();
        QCOMPARE(b[i].trimmed(), a[i].trimmed());
    }
}

QTEST_APPLESS_MAIN(TestLuaParser)

#include "tst_testluaparser.moc"
