#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include "html_parser.hpp"

using namespace std;

//test31
TEST(test, testSupportsNonAsciiTags) {
    string body = "<html><a進捗推移グラフ>Yes</a進捗推移グラフ><bрусский-тэг>Correct</<bрусский-тэг></html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(body);
    vector<shared_ptr<HtmlElement>> els =doc->SelectElement("//a進捗推移グラフ");

    ASSERT_EQ(1, els.size());

    ASSERT_EQ("Yes", els[0]->text());
    els =doc->SelectElement("//bрусский-тэг");

    ASSERT_EQ(1, els.size());
    ASSERT_EQ("Correct", els[0]->text());
}

//test32
TEST(test, testSupportsPartiallyNonAsciiTags) {
    string body = "<html><div>Check</divá></html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(body);
    vector<shared_ptr<HtmlElement>> els =doc->SelectElement("//div");

    ASSERT_EQ(1, els.size());

    ASSERT_EQ("Check", els[0]->text());
}

//test33
TEST(test, preSkipsFirstNewline) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<html><pre>\n\nOne\nTwo\n</pre></html>");
    vector<shared_ptr<HtmlElement>> pre =doc->SelectElement("//pre");

    ASSERT_EQ(1, pre.size());

    ASSERT_EQ("OneTwo", pre[0]->text());
}

//test34
TEST(test, testNoSpuriousSpace) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<p>Just<a>One</a><a>Two</a></p>");

    ASSERT_EQ("JustOneTwo", doc->text());
}
 
//test35
TEST(test, testH20) {
    string html = "H<sub>2</sub>O";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<p>" + html+"</p>");
    ASSERT_EQ("H2O", doc->text());
}

//test36
TEST(test, testUNewlines) {
    string html = "t<u>es</u>t <b>on</b> <i>f</i><u>ir</u>e";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<p>" + html+"</p>");
    ASSERT_EQ("test on fire", doc->text());
}

//test37
TEST(test, testFarsi) {
    string text = "نیمه\u200Cشب";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<p>" + text+"</p>");
    ASSERT_EQ(text, doc->text());
}

//test38
TEST(test, mergeHtmlAttributesFromBody) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<html id=1 class=foo><body><html class=bar data=x><p>One");
    ASSERT_EQ("", doc->html());
}

//test39
TEST(test, text) {
    string h = "<div><p>Hello<p>there<p>world</div>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);
    ASSERT_EQ("Hello\nthere\nworld", doc->text());
}

//test40
TEST(test, html) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<html><div><p>Hello</p></div><div><p>There</p></div></html>");

    vector<shared_ptr<HtmlElement>> divs =doc->SelectElement("//div");

    ASSERT_EQ(false, divs.empty());

    ASSERT_EQ("<div><p>Hello</p></div>", divs[0]->html());
}

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}