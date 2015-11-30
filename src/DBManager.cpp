#include <iostream>
#include "DBManager.h"
#include "FileManager.h"

DBManager DBManager::inst;

const std::string DEFAULT_FILENAME = "db";

DBManager::DBManager() {}

DBManager::~DBManager() {}

void DBManager::createTable(const std::string &name,
                            const TemplateTuple &attr) {
    for (auto i = tables.begin(); i != tables.end(); i++)
        if (i->name == name) {
            std::cerr<<"Table exist!";
            return;
        }

    Table t;
    t.name = name;
    t.tuples = attr;
    tables.push_back(t);
    FileManager::createTable(t);
}

TemplateTuple &DBManager::getAttributes(const std::string &name) {
    for (auto i = tables.begin(); i != tables.end(); i++)
        if (i->name == name) {
            return i->tuples;
        }
    throw 123;
}

void DBManager::insert(const std::string &table, Tuple &tuple) {
    FileManager::insert(table, tuple);
}
