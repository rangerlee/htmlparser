#include "html_parser.hpp"

int main() {
    std::string data("<html><body><div><span id=\"a\" class=\"x\">a</span><span id=\"b\">b</span></div></body></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(data.c_str(), data.size());

    std::vector<shared_ptr<HtmlElement>> x = doc->GetElementByClassName("x");
    shared_ptr<HtmlElement> b = doc->GetElementById("b");
    
    if(!x.empty()){
        std::cout << x[0]->GetName() << std::endl;
        std::cout << x[0]->GetAttribute("id") << std::endl;
    }
    
    if(b){
        std::cout << b->GetName() << std::endl;
        std::cout << b->GetValue() << std::endl;
    }
    
    data = "<html><meta id=\"a\" /><img src=\"b\"><br id=\"c\"><br id=\"d\"/></html>";
    doc = parser.Parse(data.c_str(), data.size());
    b = doc->GetElementById("b");
    if(b){
        shared_ptr<HtmlElement> c = b->GetElementById("c");
        if(c){
            std::cout << "wrong" << std::endl;
        } else {
            std::cout << "ok" << std::endl;
        }
    }

    data = "<html><img class=\"aa bb cc\"><br class=\"xx yy zz\"></html>";
    doc = parser.Parse(data.c_str(), data.size());
    std::vector<shared_ptr<HtmlElement>> ab = doc->GetElementByClassName("aa bb");
    std::vector<shared_ptr<HtmlElement>> abc = doc->GetElementByClassName("aa cc bb");
    std::vector<shared_ptr<HtmlElement>> xz = doc->GetElementByClassName("xx zz");
    if(ab.empty() || abc.empty() || xz.empty()){
        std::cout << "wrong" << std::endl;
    } else {
        std::cout << "ok" << std::endl;
    }

    std::vector<shared_ptr<HtmlElement>> cc = doc->SelectElement("//img[@class]");
    if(cc.empty()){
        std::cout << "wrong" << std::endl;
    } else {
        std::cout << "ok" << std::endl;
    }

    return 0;
}