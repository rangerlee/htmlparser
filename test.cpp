#include "html_parser.hpp"

int main() {
	std::string data("<html><head></head><body><div><span id=\"a\" class=\"x\">a</span><span id=\"b\">b</span></div></body></html>");
    HtmlParser parser;
    shared_ptr<HtmlDocument> doc = parser.Parse(data.c_str(), data.size());

    std::vector<std::shared_ptr<HtmlElement>> x = doc->GetElementByClassName("x");
    std::vector<std::shared_ptr<HtmlElement>> b = doc->GetElementById("b");
    
	if(x){
		std::cout << x->GetName() << std::endl;
		std::cout << x->GetAttribute("id") << std::endl;
	}
	
	if(b){
		std::cout << b->GetName() << std::endl;
		std::cout << b->GetValue() << std::endl;
	}

    return 0;
}