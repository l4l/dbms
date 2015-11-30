#include <iostream>
#include <sstream>
#include <regex>
#include <assert.h>
#include "parser.h"
#include "DBManager.h"

void flushBuf(Strs &v, std::stringstream &buf);

void* prepare(std::string query) {
    std::regex spaces("\\s{2,}");

    char buf[query.size()] = {'\0'};
    std::regex_replace(buf, query.begin(), query.end(), spaces, " ");
    query = std::string(buf);

    if (!checkBrackets(query))
        throw "Wrong brackets";

    Strs a = decompose(query);

    {
        int i = 0;
        while (a[0][i]) {
            a[0][i] = std::tolower(a[0][i]);
            i++;
        }
    }

    if (a[0] == "insert") {
        insert(a);
    } else if (a[0] == "create") {
        create(a);
    } else if (a[0] == "select") {
        return select(a);
    } else {
        std::cerr<<"Unknown query type:";
        for (auto i = a.begin(); i != a.end(); ++i)
            std::cerr<<" "<<*i;
        std::cerr<<std::endl;
    }
    return nullptr;
}

bool checkBrackets(std::string &s) {
    size_t l = 0, r = 0;
    for (auto i = s.begin(); i != s.end(); ++i) {
        if (*i == '(') l++;
        else if (*i == ')') r++;
        if (l < r) return false;
    }
    return l == r;
}

Strs decompose(std::string &query) {
    Strs v;
    std::stringstream buf;
    bool isQuote = false;
    bool isBracket = false;
    for (auto i = 0; i < query.size(); ++i) {
        if (query[i] == ';') {
            flushBuf(v, buf);
            break;
        }

        if (query[i] == '\'' || query[i] == '\"')
            isQuote = !isQuote;

        if (!isQuote) {
            if (!isBracket) {
                if (query[i] == '(') {
                    isBracket = true;
                    flushBuf(v, buf);
                } else
                    if (query[i] == ' ') flushBuf(v, buf);
                    else buf<<query[i];
            } else
                if (query[i] == ')') {
                    flushBuf(v, buf);
                    isBracket = false;
                } else buf<<query[i];
        } else buf<<query[i];
    }
    return v;
}

void create(Strs &strs) {
    const std::string table = strs[2];
    Strs args;
    std::string s = strs[3] + ',';
    while (s.size() > 1) {
        size_t comma = s.find(',', 0);
        size_t start = s[0] == ' ' ? 1 : 0;
        args.push_back(s.substr(start, comma - start));
        if (s.find(',', comma + 1) == std::string::npos)
            break;
        s = std::string(s.substr(comma + start + 1));
    }

    TemplateTuple tuple;
    for (auto i = args.begin(); i != args.end(); ++i) {
        size_t space = i->find(' ', 1);
        tuple.push_back({parse(i->substr(space + 1, i->size())), i->substr(0, space)});
    }

    DBManager::inst.createTable(table, tuple);
}

void insert(Strs &strs) {
    const std::string table = strs[2];
    Tuple t;
    Strs args;
    std::string s = strs[4];
    bool isQuote = false;
    std::stringstream buf;
    for (auto i = s.begin(); i != s.end(); ++i) {
        if (!isQuote && (*i == ',' || *i == ' '))
            ;
        else if (*i == '\'' || *i == '\"') {
            flushBuf(args, buf);
            isQuote = !isQuote;
        } else {
            buf<<*i;
        }
    }
    auto attr = DBManager::inst.getAttributes(table);
    assert(attr.size() == args.size());
    for (auto i = 0; i < args.size(); ++i)
        t.push_back(build(attr[i].type, args[i]));
    DBManager::inst.insert(table, t);
}

std::vector<Tuple>* select(Strs &strs) {
    std::string table = strs[3];
    auto f = [](const Tuple&) -> bool { return true; };
    return DBManager::inst.select(table, f);
}

void flushBuf(Strs &v, std::stringstream &buf) {
    std::string s(buf.str());
    if (s.size() > 0) v.push_back(s);
    buf.str("");
}
