#ifndef DBMS_FILEMANAGER_H
#define DBMS_FILEMANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "db.h"

size_t constexpr pageNum(size_t s);

size_t constexpr offset(size_t s);

struct Meta {
    static const size_t PAGE_SIZE = 4096;
    size_t nextBlock;
    size_t emptyPtr;

    Meta(size_t next = 0, size_t emptyPtr = size()) :
            nextBlock(next), emptyPtr(emptyPtr) { }

    constexpr static size_t size() {
        return sizeof(nextBlock) + sizeof(emptyPtr);
    }
};

struct TableInfo {
    size_t first;
    size_t last;

    size_t tuples;

    TableInfo(size_t first = 0, size_t lastPage = 0, size_t pageOffset = 0) :
            first(first), last(lastPage), tuples(0) { }
};

class FileManager {
public:
    static void init(std::string);
    /**
     * Write table info in header
     * return start page of table data
     */
    static size_t createTable(Table &);
    static size_t insertBlock(const std::string table, std::vector<Tuple>);
    static size_t insert(const std::string &, Tuple &);

    template<class T>
    static std::vector<Tuple>* get(std::string &table,
                                   const TemplateTuple &attr, T &p) {
        TableInfo &info = inst.tablesInfo[table];
        inst.moveToBlock(info.first, 0x10, false);
        std::vector<Tuple> *tuples = new std::vector<Tuple>;
        for (size_t i = 0; i < info.tuples; i++) {
            Tuple tuple = inst.getRoutine(attr);
            if (p(tuple))
                tuples->push_back(tuple);
        }
        return tuples;
    }

private:
    FileManager() {}
    ~FileManager();

    std::fstream file;
    /**
     * pages[current]
     */
    Meta *page;
    /**
     * Meta data about pages
     */
    std::vector<Meta> pages;
    /**
     * Name -> TableInfo
     */
    std::map<std::string, TableInfo> tablesInfo;

    /**
     * Store buffered pages
     * Page num -> Page
     * TODO use it
     */
//    PriorityArray<100, size_t, std::stringstream> buffer;

    static FileManager inst;

    /**
     * Adds an empty page and return its number
     */
    size_t addPage();
    /**
     * Get Meta::PAGE_SIZE bytes of page
     */
    std::string getPage(size_t);
    /**
     * Write Meta::PAGE_SIZE bytes to the page
     */
    void writePage(size_t, const char *);
    /**
     * Move file pointer to block, optional with offset
     */
    void moveToBlock(size_t, size_t offset = 0, bool isOut = true);
    /**
     * Output how much space left at specific page
     */
    size_t left(size_t) const;

    void insertRoutine(std::vector<std::string>&, const Tuple&);
    Tuple getRoutine(const TemplateTuple&) const;

    TableInfo& moveToEmpty(const std::string&);
};

#endif //DBMS_FILEMANAGER_H
