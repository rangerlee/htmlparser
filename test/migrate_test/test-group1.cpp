#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include "html_parser.hpp"

using namespace std;

//test1
TEST(test, dropsUnterminatedTag) {
    string h1("<p");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h1);
    vector<shared_ptr<HtmlElement>> x = doc->GetElementByTagName("p");

    ASSERT_EQ(0, x.size());
    ASSERT_EQ(true, x.empty());

    string h2("<div id=1<p id='2'");
    doc = parser.Parse(h1.c_str(), h1.size());
    ASSERT_EQ("", doc->text());
}

//test2
TEST(test, dropsUnterminatedAttribute) {
    string h1("<p id=\"foo");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h1);
    ASSERT_EQ("", doc->text());
}

//test3
TEST(test, parsesQuiteRoughAttributes) {

    string html("<p =a>OneSomething</p>Else");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);

    ASSERT_EQ("<p>OneSomething</p>", doc->html());
}

//test4
TEST(test, createsStructureFromBodySnippet) {
    string html("foo <b>bar</b> baz");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    ASSERT_EQ("", doc->text());
}

//test5
TEST(test, handlesTextArea) {
    string html("<html><textarea>Hello</textarea></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    vector<shared_ptr<HtmlElement>> els = doc->SelectElement("//textarea");
    
    ASSERT_EQ(1, els.size());

    ASSERT_EQ("Hello", els[0]->text());
    ASSERT_EQ("Hello", els[0]->GetValue());
}

//test6
TEST(test, handlesTextAfterData) {
    string h = "<html><body>pre <script>inner</script> aft</body></html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("<html><body>pre <script>inner</script> aft</body></html>", doc->html());
}

//test7
TEST(test, handlesTextTd) {
    string h = "<td>Hello<td><p>There<p>now";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("", doc->html());
}

//test8
TEST(test, handlesNestedImplicitTable) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<table><td>1</td></tr> <td>2</td></tr> <td> <table><td>3</td> <td>4</td></table> <tr><td>5</table>");

    ASSERT_EQ("<table><td>1</td> <td>2</td> <td> <table><td>3</td> <td>4</td></table> <tr><td>5</td></tr></td></table>", doc->html());
}

//test9
TEST(test, handlesImplicitCaptionClose) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<table><caption>A caption<td>One<td>Two</table>");
    ASSERT_EQ("<table><caption>A caption<td>One<td>Two</td></td></caption></table>", doc->html());
}

//test10
TEST(test, handlesKnownEmptyBlocks) {
    string h = "<div id='1' /><script src='/foo' /><div id=2><img /><img></div><a id=3 /><i /><foo /><foo>One</foo> <hr /> hr text <hr> hr text two";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("<div id=\"1\"></div><script src=\"/foo\"></script><div id=\"2\"><img></img><img></img></div><a id=\"3\"></a><i></i><foo></foo><foo>One</foo><hr></hr>", doc->html());
}

 

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}