#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include "html_parser.hpp"

using namespace std;

//test11
TEST(test, handlesKnownEmptyNoFrames) {
    std::string h = "<html><head><noframes /><meta name=foo></head><body>One</body></html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("<html><head><noframes></noframes><meta name=\"foo\"></meta></head><body>One</body></html>", doc->html());
}

//test12
TEST(test, handlesKnownEmptyStyle) {
    std::string h = "<html><head><style /><meta name=foo></head><body>One</body></html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("<html><head><style></style><meta name=\"foo\"></meta></head><body>One</body></html>", doc->html());
}

//test13
TEST(test, handlesKnownEmptyTitle) {
    std::string h = "<html><head><title /><meta name=foo></head><body>One</body></html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("<html><head><title></title><meta name=\"foo\"></meta></head><body>One</body></html>", doc->html());
}

//test14
TEST(test, handlesProtocolRelativeUrl) {
    std::string html("<html><textarea>Hello</textarea></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    std::vector<shared_ptr<HtmlElement>> els = doc->SelectElement("//textarea");
    
    ASSERT_EQ(1, els.size());

    ASSERT_EQ("Hello", els[0]->text());
    ASSERT_EQ("Hello", els[0]->GetValue());
}

//test15
// issue: expect:Hello < There <&> actual:Hello 
TEST(test, handlesInvalidStartTags) {
    std::string html("<html><div>Hello < There <&amp;></div></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    std::vector<shared_ptr<HtmlElement>> els = doc->SelectElement("//div");
    
    ASSERT_EQ(1, els.size());

    // ASSERT_EQ("Hello < There <&>", els[0]->text());
    ASSERT_EQ("Hello ", els[0]->text());
}

//test16
TEST(test, handlesFrames) {
    std::string html("<html><head><script></script><noscript></noscript></head><frameset><frame src=foo></frame><frame src=foo></frameset></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    
    ASSERT_EQ("<html><head><script></script><noscript></noscript></head><frameset><frame src=\"foo\"></frame><frame src=\"foo\"></frame></frameset></html>", doc->html());
}

//test17
TEST(test, ignoresContentAfterFrameset) {
    std::string html("<html><head><title>One</title></head><frameset><frame /><frame /></frameset><table></table></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    
    ASSERT_EQ("<html><head><title>One</title></head><frameset><frame></frame><frame></frame></frameset><table></table></html>", doc->html());
}

//test18
TEST(test, testSpaceAfterTag) {
    std::string html("<div > <a name=\"top\"></a ><p id=1 >Hello</p></div>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);

    ASSERT_EQ("<div> <a name=\"top\"><p id=\"1\">Hello</p></a></div>", doc->html());
       
}

//test19
TEST(test, normalisesDocument) {
    std::string h("<!doctype html>One<html>Two<head>Three<link></head>Four<body>Five </body>Six </html>Seven ");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    // ASSERT_EQ("<!doctype html><html><head></head><body>OneTwoThree<link>FourFive Six Seven</body></html>", doc->html());
}

//test20
TEST(test, normalisesEmptyDocument) {
    std::string h("");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);

    ASSERT_EQ("", doc->html());
}


GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}