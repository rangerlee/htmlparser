#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include "html_parser.hpp"

using namespace std;

//test51
TEST(test, textHasSpaceBetweenDivAndCenterTags) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<div>One</div><div>Two</div><center>Three</center><center>Four</center>");
    ASSERT_EQ("One\nTwoThreeFour", doc->text());
}

//test52
TEST(test, prettyprintBrInBlock) {
    string html = "<div><br> </div>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    ASSERT_EQ("<div><br></br> </div>", doc->html()); 
}

//test53
TEST(test, prettyprintBrWhenNotFirstChild) {
    string h = "<div>\n\
             <p><br>\n  Foo</p>\n\
             <br>\n\
            </div>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);
    ASSERT_EQ("<div>             <p><br></br>  Foo</p>             <br></br>            </div>", doc->html()); 
}

//test54
TEST(test, noDanglingSpaceAfterCustomElement) {
    string html = "<bar><p/>\n</bar>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    ASSERT_EQ("<bar><p></p></bar>", doc->html()); 

    html = "<foo>\n  <bar />\n</foo>";
    doc = parser.Parse(html);

    ASSERT_EQ("<foo>  <bar></bar></foo>", doc->html()); 
}

//test55
TEST(test, spanInBlockTrims) {
    string html = "<p>Lorem ipsum</p>\n<span>Thanks</span>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    string outHtml = doc->html();
    ASSERT_EQ("<p>Lorem ipsum</p><span>Thanks</span>", outHtml); 
}

//test56
TEST(test, rubyInline) {
    string html = "<ruby>T<rp>(</rp><rtc>!</rtc><rt>)</rt></ruby>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(html);
    ASSERT_EQ(html, doc->html()); 
}

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}