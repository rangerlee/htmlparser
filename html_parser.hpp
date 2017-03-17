/*
 * Copyright (c) 2017 rangerlee (rangerlee@foxmail.com)
 * Latest version available at: http://github.com/rangerlee/htmlparser.git
 *
 * A Simple html parser.
 * More information can get from README.md
 *
 */

#ifndef HTMLPARSER_HPP_
#define HTMLPARSER_HPP_

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>

#if __cplusplus <= 199711L
#include <tr1/memory>
using std::tr1::enable_shared_from_this;
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
#else

#include <memory>

using std::enable_shared_from_this;
using std::shared_ptr;
using std::weak_ptr;
#endif

/**
 * class HtmlElement
 * HTML Element struct
 */
class HtmlElement : public enable_shared_from_this<HtmlElement> {
public:
    friend class HtmlParser;

    friend class HtmlDocument;

public:
    /**
     * for children traversals.
     */
    typedef std::vector<shared_ptr<HtmlElement> >::const_iterator ChildIterator;

    const ChildIterator ChildBegin() {
        return children.begin();
    }

    const ChildIterator ChildEnd() {
        return children.end();
    }

    /**
     * for attribute traversals.
     */
    typedef std::map<std::string, std::string>::const_iterator AttributeIterator;

    const AttributeIterator AttributeBegin() {
        return attribute.begin();
    }

    const AttributeIterator AttributeEnd() {
        return attribute.end();
    }

public:
    HtmlElement() {}

    HtmlElement(shared_ptr<HtmlElement> p)
            : parent(p) {}

    std::string GetAttribute(const std::string &k) {
        if (attribute.find(k) != attribute.end()) {
            return attribute[k];
        }

        return "";
    }

    shared_ptr<HtmlElement> GetElementById(const std::string &id) {
        return GetElementById(shared_from_this(), id);
    }

    std::vector<shared_ptr<HtmlElement> > GetElementByClassName(const std::string &name) {
        std::vector<shared_ptr<HtmlElement> > result;
        HtmlElement::GetElementByClassName(shared_from_this(), name, result);
        return result;
    }

    std::vector<shared_ptr<HtmlElement> > GetElementByTagName(const std::string &name) {
        std::vector<shared_ptr<HtmlElement> > result;
        HtmlElement::GetElementByTagName(shared_from_this(), name, result);
        return result;
    }

    shared_ptr<HtmlElement> GetParent() {
        return parent.lock();
    }

    const std::string &GetValue() {
        return value;
    }

    const std::string &GetName() {
        return name;
    }

private:
    static shared_ptr<HtmlElement>
    GetElementById(const shared_ptr<HtmlElement> &element, const std::string &id) {
        for (HtmlElement::ChildIterator it = element->children.begin(); it != element->children.end(); ++it) {
            if ((*it)->GetAttribute("id") == id)
                return *it;

            shared_ptr<HtmlElement> r = GetElementById(*it, id);
            if (r)
                return r;
        }

        return shared_ptr<HtmlElement>();
    }

    static void GetElementByClassName(const shared_ptr<HtmlElement> &element, const std::string &name,
                                      std::vector<shared_ptr<HtmlElement> > &result) {
        for (HtmlElement::ChildIterator it = element->children.begin(); it != element->children.end(); ++it) {
            std::string str_t((*it)->GetAttribute("class"));
            char *temp = NULL;
            char *p = strtok_r((char *) str_t.c_str(), " ", &temp);
            while (p) {
                if (strcmp(p, name.c_str()) == 0)
                    result.push_back(*it);
                p = strtok_r(NULL, " ", &temp);
            }

            GetElementByClassName(*it, name, result);
        }
    }

    static void GetElementByTagName(const shared_ptr<HtmlElement> &element, const std::string &name,
                                    std::vector<shared_ptr<HtmlElement> > &result) {
        for (HtmlElement::ChildIterator it = element->children.begin(); it != element->children.end(); ++it) {
            if ((*it)->name == name)
                result.push_back(*it);

            GetElementByTagName(*it, name, result);
        }
    }

    void Parse(const std::string &attr) {
        size_t index = 0;
        std::string k;
        std::string v;

        enum ParseAttrState {
            PARSE_ATTR_KEY,
            PARSE_ATTR_VALUE,
            PARSE_ATTR_KEY_END,
            PARSE_ATTR_VALUE_END,
        };

        ParseAttrState state = PARSE_ATTR_KEY;

        while (attr.size() > index) {
            char input = attr.at(index);
            char split;
            switch (state) {
                case PARSE_ATTR_KEY: {
                    if (input == '\t' || input == '\r' || input == '\n') {
                    } else if (input == '\'' || input == '"') {
                        std::cerr << "WARN : attribute unexpected " << input << std::endl;
                    } else if (input == ' ') {
                        if (k.size()) {
                            state = PARSE_ATTR_KEY_END;
                        }
                    } else if (input == '=') {
                        state = PARSE_ATTR_VALUE;
                    } else {
                        k.append(attr.c_str() + index, 1);
                    }
                }
                    break;

                case PARSE_ATTR_VALUE: {
                    if (input == '\t' || input == '\r' || input == '\n') {

                    } else if (input == '\'' || input == '"') {
                        split = input;
                        state = PARSE_ATTR_VALUE_END;
                    } else {
                        v.append(attr.c_str() + index, 1);
                    }
                }
                    break;

                case PARSE_ATTR_KEY_END: {
                    if (input == '\t' || input == '\r' || input == '\n' || input == ' ') {

                    } else if (input == '=') {
                        state = PARSE_ATTR_VALUE;
                    } else {
                        attribute[k] = v;
                        k.clear();
                        v.clear();
                        state = PARSE_ATTR_KEY;
                        k.append(attr.c_str() + index, 1);
                    }
                }
                    break;

                case PARSE_ATTR_VALUE_END: {
                    if (input == split) {
                        attribute[k] = v;
                        k.clear();
                        v.clear();
                        state = PARSE_ATTR_KEY;
                    } else {
                        v.append(attr.c_str() + index, 1);
                    }
                }
                    break;
            }

            index++;
        }

        //trim
        if (!value.empty()) {
            value.erase(0, value.find_first_not_of(" "));
            value.erase(value.find_last_not_of(" ") + 1);
        }
    }

private:
    std::string name;
    std::string value;
    std::map<std::string, std::string> attribute;
    weak_ptr<HtmlElement> parent;
    std::vector<shared_ptr<HtmlElement> > children;
};

/**
 * class HtmlDocument
 * Html Doc struct
 */
class HtmlDocument {
public:
    HtmlDocument(shared_ptr<HtmlElement> &root)
            : root_(root) {}

    shared_ptr<HtmlElement> GetElementById(const std::string &id) {
        return HtmlElement::GetElementById(root_, id);
    }

    std::vector<shared_ptr<HtmlElement> > GetElementByClassName(const std::string &name) {
        std::vector<shared_ptr<HtmlElement> > result;
        HtmlElement::GetElementByClassName(root_, name, result);
        return result;
    }

    std::vector<shared_ptr<HtmlElement> > GetElementByTagName(const std::string &name) {
        std::vector<shared_ptr<HtmlElement> > result;
        HtmlElement::GetElementByTagName(root_, name, result);
        return result;
    }

private:
    shared_ptr<HtmlElement> root_;
};

/**
 * class HtmlParser
 * html parser and only one interface
 */
class HtmlParser {
public:
    /**
     * parse html by C-Style data
     * @param data
     * @param len
     * @return html document object
     */
    shared_ptr<HtmlDocument> Parse(const char *data, size_t len) {
        stream_ = data;
        length_ = len;
        size_t index = 0;
        root_.reset(new HtmlElement());
        while (length_ > index) {
            char input = stream_[index];
            if (input == '\r' || input == '\n' || input == '\t' || input == ' ') {
                index++;
            } else if (input == '<') {
                index = ParseElement(index, root_);
            } else {
                break;
            }
        }

        return shared_ptr<HtmlDocument>(new HtmlDocument(root_));
    }

    /**
     * parse html by string data
     * @param data
     * @return html document object
     */
    shared_ptr<HtmlDocument> Parse(const std::string &data) {
        return Parse(data.data(), data.size());
    }

private:
    size_t ParseElement(size_t index, shared_ptr<HtmlElement> &element) {
        while (length_ > index) {
            char input = stream_[index + 1];
            if (input == '!') {
                if (strncmp(stream_ + index, "<!--", 4) == 0) {
                    return SkipUntil(index + 2, "-->");
                } else {
                    return SkipUntil(index + 2, '>');
                }
            } else if (input == '/') {
                return SkipUntil(index, '>');
            } else if (input == '?') {
                return SkipUntil(index, "?>");
            }

            shared_ptr<HtmlElement> self(new HtmlElement(element));

            enum ParseElementState {
                PARSE_ELEMENT_TAG,
                PARSE_ELEMENT_ATTR,
                PARSE_ELEMENT_VALUE,
                PARSE_ELEMENT_TAG_END
            };

            ParseElementState state = PARSE_ELEMENT_TAG;
            index++;
            char split = 0;
            std::string attr;

            while (length_ > index) {
                switch (state) {
                    case PARSE_ELEMENT_TAG: {
                        char input = stream_[index];
                        if (input == ' ' || input == '\r' || input == '\n' || input == '\t') {
                            if (!self->name.empty()) {
                                state = PARSE_ELEMENT_ATTR;
                            }
                            index++;
                        } else if (input == '/') {
                            self->Parse(attr);
                            element->children.push_back(self);
                            return SkipUntil(index, '>');
                        } else if (input == '>') {
                            state = PARSE_ELEMENT_VALUE;
                            index++;
                        } else {
                            self->name.append(stream_ + index, 1);
                            index++;
                        }
                    }
                        break;

                    case PARSE_ELEMENT_ATTR: {
                        char input = stream_[index];
                        if (input == '>') {
                            if (stream_[index - 1] == '/') {
                                attr.erase(attr.size() - 1);
                                self->Parse(attr);
                                element->children.push_back(self);
                                return ++index;
                            }
                            state = PARSE_ELEMENT_VALUE;
                            index++;
                        } else {
                            attr.append(stream_ + index, 1);
                            index++;
                        }
                    }
                        break;

                    case PARSE_ELEMENT_VALUE: {
                        if (self->name == "script" || self->name == "noscript" || self->name == "style") {
                            std::string close = "</" + self->name + ">";

                            size_t pre = index;
                            index = SkipUntil(index, close.c_str());
                            if (index > (pre + close.size()))
                                self->value.append(stream_ + pre, index - pre - close.size());

                            self->Parse(attr);
                            element->children.push_back(self);
                            return index;
                        }

                        char input = stream_[index];
                        if (input == '<') {
                            if (stream_[index + 1] == '/') {
                                state = PARSE_ELEMENT_TAG_END;
                            } else {
                                index = ParseElement(index, self);
                            }
                        } else if (input != '\r' && input != '\n' && input != '\t') {
                            self->value.append(stream_ + index, 1);
                            index++;
                        } else {
                            index++;
                        }
                    }
                        break;

                    case PARSE_ELEMENT_TAG_END: {
                        index += 2;
                        std::string selfname = self->name + ">";
                        if (strncmp(stream_ + index, selfname.c_str(), selfname.size())) {
                            int pre = index;
                            index = SkipUntil(index, ">");
                            std::string value;
                            if (index > (pre + 1))
                                value.append(stream_ + pre, index - pre - 1);
                            else
                                value.append(stream_ + pre, index - pre);

                            shared_ptr<HtmlElement> parent = self->GetParent();
                            while (parent) {
                                if (parent->name == value) {
                                    std::cerr << "WARN : element not closed <" << self->name << "> " << std::endl;
                                    return pre - 2;
                                }

                                parent = parent->GetParent();
                            }

                            std::cerr << "WARN : unexpected closed element </" << value << "> for <" << self->name
                                      << ">" << std::endl;
                            state = PARSE_ELEMENT_VALUE;
                        } else {
                            self->Parse(attr);
                            element->children.push_back(self);
                            return SkipUntil(index, '>');
                        }
                    }
                        break;
                }
            }
        }
    }

    size_t SkipUntil(size_t index, const char *data) {
        while (length_ > index) {
            if (strncmp(stream_ + index, data, strlen(data)) == 0) {
                return index + strlen(data);
            } else {
                index++;
            }
        }
    }

    size_t SkipUntil(size_t index, const char data) {
        while (length_ > index) {
            if (stream_[index] == data) {
                return ++index;
            } else {
                index++;
            }
        }
    }

private:
    const char *stream_;
    size_t length_;
    shared_ptr<HtmlElement> root_;
};

#endif
