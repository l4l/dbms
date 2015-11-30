#include <iostream>
#include "DBManager.h"
#include "FileManager.h"
#include "parser.h"

void print(const std::vector<Tuple> *t, std::string table) {
    for (auto i = DBManager::inst.getAttributes(table).begin();
            i != DBManager::inst.getAttributes(table).end(); ++i)
        std::cout<<i->name<<'\t';
    std::cout<<'\n';
    if (!t) {
        std::cout<<"None founed";
        return;
    }
    for (auto i = t->begin(); i != t->end(); ++i) {
        for (auto j = i->begin(); j != i->end(); ++j)
            if (*j) (*j)->operator<<(std::cout) << '\t';
            else std::cout << "err\t";
        std::cout<<'\n';
    }
}

int main() {
    FileManager::init("db");
//    std::string table("mytable");
//    Table t;
//    t.name = "mytable";
//    ValueDetail a(INT_TYPE, "a");
//    ValueDetail b(CHAR_TYPE, "b");
//    t.tuples.push_back(a);
//    t.tuples.push_back(b);
//    DBManager::inst.createTable(table, {{INT_TYPE, "a"}, {CHAR_TYPE, "b"}});
//    InstValue<int> a(5);
//    InstValue<std::string> b("asd");
//    Tuple v;
//    v.push_back(new InstValue<int>(5));
//    v.push_back(new InstValue<std::string>("asd"));
//    std::vector<Tuple> tup;
//    tup.push_back(v);
    std::fstream f;
    std::string line;
    
    f.open("data/SimpleStruct.sql");
    while (std::getline(f, line))
        prepare(line);
    f.close();

    f.open("data/journals.sql");
    while (std::getline(f, line))
        prepare(line);

//    auto a = static_cast<std::vector<Tuple>*>(prepare("select * from journal;"));
//    print(a, "journal");
    std::cout<<"journals\n";

    f.open("data/urls.sql");
    while (std::getline(f, line))
        prepare(line);
    std::cout<<"urls\n";

    f.open("data/person.sql");
    while (std::getline(f, line))
        prepare(line);
    std::cout<<"persons\n";

    f.open("data/article.sql");
    while (std::getline(f, line))
        prepare(line);
    std::cout<<"article\n";

    f.open("data/articleeditor.sql");
    while (std::getline(f, line))
        prepare(line);
    std::cout<<"articleeditor\n";

    f.open("data/authorship.sql");
    while (std::getline(f, line))
        prepare(line);
    std::cout<<"authorship\n";

    f.open("data/urlarticle.sql");
    while (std::getline(f, line))
        prepare(line);
    std::cout<<"urlarticle\n";

//    FileManager::insertBlock(table, {{new InstValue<int>(5), new InstValue<std::string>("asd")},
//                                     {new InstValue<int>(6), new InstValue<std::string>("dsa")},
//                                     {new InstValue<int>(7), new InstValue<std::string>("cds")}});
//    auto f = [](const Tuple&) -> bool { return true; };
//    auto a = DBManager::inst.select(table, f);
//    for (auto i = DBManager::inst.getAttributes(table).begin();
//            i != DBManager::inst.getAttributes(table).end(); ++i)
//        std::cout<<i->name<<'\t';
//    std::cout<<'\n';
//    for (auto i = a->begin(); i != a->end(); ++i) {
//        for (auto j = i->begin(); j != i->end(); ++j)
//            if (*j) (*j)->operator<<(std::cout) << '\t';
//            else std::cout << "err\t";
//        std::cout<<'\n';
//    }
    return 0;
}
