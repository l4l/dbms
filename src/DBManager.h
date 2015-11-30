#ifndef DBMS_DBMANAGER_H
#define DBMS_DBMANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <list>
#include <functional>
#include "db.h"
#include "FileManager.h"


class DBManager {
public:
    static DBManager inst;
    void createTable(const std::string &name,
                     const TemplateTuple &attr);
    TemplateTuple& getAttributes(const std::string &name);
    template <class T>
    std::vector<Tuple> * select(std::string &name, T &p) {
        for (auto i = tables.begin(); i < tables.end(); ++i)
            if (i->name == name) {
                return FileManager::get(name, i->tuples, p);
            }

        return nullptr;
    }

    void insert(const std::string&, Tuple&);

private:
    DBManager();
    ~DBManager();
    /**
     * block
     */
    std::vector<Table> tables;
};


#endif //DBMS_DBMANAGER_H
