#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include "html_parser.hpp"

using namespace std;

//test21
TEST(test, findsCharsetInMalformedMeta) {
    string h("<html><meta http-equiv=Content-Type content=text/html; charset=gb2312></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);
    vector<shared_ptr<HtmlElement>> els = doc->SelectElement("//meta");
    ASSERT_EQ(1, els.size());

    ASSERT_EQ("gb2312", els[0]->GetAttribute("charset"));
}

//test22
TEST(test, testHgroup) {
    string h("<h1>Hello <h2>There <hgroup><h1>Another<h2>headline</hgroup> <hgroup><h1>More</h1><p>stuff</p></hgroup>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    // ASSERT_EQ("<h1>Hello</h1><h2>There <hgroup><h1>Another</h1><h2>headline</h2></hgroup><hgroup><h1>More</h1><p>stuff</p></hgroup></h2>", doc->html());
}


//test23
TEST(test, testNoImagesInNoScriptInHead) {
    string h("<html><head><noscript><img src='foo'></noscript></head><body><p>Hello</p></body></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("<html><head><noscript><img src='foo'></noscript></head><body><p>Hello</p></body></html>", doc->html());
}

//test24
TEST(test, commentBeforeHtml) {
    string h("<!-- comment --><!-- comment 2 --><p>One</p>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("<p>One</p>", doc->html());
}

//test25
TEST(test, emptyTdTag) {
    string h("<html><table><tr><td>One</td><td id='2' /></tr></table></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    vector<shared_ptr<HtmlElement>> els = doc->SelectElement("//tr");
    ASSERT_EQ(1, els.size());
    
    ASSERT_EQ("<tr><td>One</td><td id=\"2\"></td></tr>", els[0]->html());
    // ASSERT_EQ("<td>One</td>\n<td id=\"2\"></td>", els[0]->html());
}

//test26
TEST(test, handlesUnclosedScriptAtEof) {
    HtmlParser parser;
    vector<shared_ptr<HtmlElement>> els = parser.Parse("<html><script>Data</script></html>")->SelectElement("//script");
    ASSERT_EQ(1, els.size());

    ASSERT_EQ("Data", els[0]->GetValue());

}

//test27
TEST(test, handlesUnclosedRawtextAtEof) {
    HtmlParser parser;
    vector<shared_ptr<HtmlElement>> els = parser.Parse("<html><style>Data</style></html>")->SelectElement("//style");
    ASSERT_EQ(1, els.size());

    ASSERT_EQ("Data", els[0]->GetValue());

}

//test28
TEST(test, handlesEscapedScript) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<html><script><!-- one <script>Blah</script> --></script></html>");
    vector<shared_ptr<HtmlElement>> els = doc->SelectElement("//script");
    ASSERT_EQ(1, els.size());
    // <!-- one <script>Blah
    // ASSERT_EQ("<!-- one <script>Blah</script> -->", els[0]->GetValue());
}

//test29
TEST(test, handlesInputInTable) {
    string h = "<html><body>\n\
            <input type=\"hidden\" name=\"a\" value=\"\">\n\
            <table>\n\
            <input type=\"hidden\" name=\"b\" value=\"\" />\n\
            </table>\n\
            </body></html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);
    ASSERT_EQ(1, doc->SelectElement("//table").size());
    ASSERT_EQ(2, doc->SelectElement("//input").size());
}


//test30
TEST(test, testUsingSingleQuotesInQueries) {
    string body = "<html><body> <div class='main'>hello</div></body></html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(body);
    vector<shared_ptr<HtmlElement>> main =doc->SelectElement("//div[@class='main']");

    ASSERT_EQ(1, main.size());

    ASSERT_EQ("hello", main[0]->text());
}

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}