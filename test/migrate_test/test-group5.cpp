#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include "html_parser.hpp"

using namespace std;


//test41
TEST(test, classWithHyphen) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<p class='tab-nav'>Check</p>");
    vector<shared_ptr<HtmlElement>> els = doc->GetElementByClassName("tab-nav");
    ASSERT_EQ(1, els.size());
    ASSERT_EQ("Check", els[0]->text());
}

//test42
TEST(test, testGetText) {
    string reference = "<div id=div1><p>Hello</p><p>Another <b>element</b></p><div id=div2><img src=foo.png></div></div>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(reference);

    ASSERT_EQ("Hello\nAnother element\n", doc->text());
    ASSERT_EQ("Another element", doc->GetElementByTagName("p")[1]->text());
}

//test43
TEST(test, testNormalisesText) {
    string h = "<html><p>Hello<p>There.</p> \n <p>Here <b>is</b> \n s<b>om</b>e text.</html>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);
    string text = doc->text();
    ASSERT_EQ("Hello\nThere.  \nHere is  some text.", text);
}

//test44
TEST(test, testKeepsPreText) {
    string h = "<p>Hello \n \n there.</p> <div><pre>  What's \n\n  that?</pre>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);
    ASSERT_EQ("Hello   there.", doc->text());
}

//test45
TEST(test, testKeepsPreTextInCode) {
    string h = "<pre><code>code\n\ncode</code></pre>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);
    ASSERT_EQ("codecode", doc->text());
}

//test46
TEST(test, testKeepsPreTextAtDepth) {
    string h = "<pre><code><span><b>code\n\ncode</b></span></code></pre>";
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(h);
    ASSERT_EQ("codecode", doc->text());
}

//test47
TEST(test, textHasSpacesAfterBlock) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<div>One</div><div>Two</div><span>Three</span><p>Fou<i>r</i></p>");
    string text = doc->text();

    ASSERT_EQ("One\nTwoThree\nFour", text);
    ASSERT_EQ("OneTwo", parser.Parse("<span>One</span><span>Two</span>")->text());
}

//test48
TEST(test, testInnerHtml) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<div>\n <p>Hello</p> </div>");

    ASSERT_EQ("<div> <p>Hello</p> </div>", doc->GetElementByTagName("div")[0]->html());
}

//test49
TEST(test, testHtmlId) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<div id=1><p>Hello</p></div>");
    shared_ptr<HtmlElement> div = doc->GetElementById("1");

    ASSERT_EQ("Hello", div->text());
}

//test50
TEST(test, textHasSpaceAfterBlockTags) {
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse("<div>One</div>Two");
    ASSERT_EQ("One", doc->text());
}

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}