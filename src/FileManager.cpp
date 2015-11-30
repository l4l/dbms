#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <assert.h>
#include "FileManager.h"

FileManager FileManager::inst;

inline size_t size(std::stringstream &s) {
    s.seekp(0, std::ios::end);
    return s.tellp();
}

Value *read(const Type &t, std::istream &in) {
    switch (t) {
        case CHAR_TYPE: {
            size_t size = 0;
            in.read((char*)&size, sizeof(size));
            if (size > 1024)
                return new String("");
            char buf[size];
            in.read(buf, size);
            return new String(std::string(buf));
        }
        case INT_TYPE:
        default: {
            int data;
            in.read((char*)&data, sizeof(data));
            return new Int(data);
        }
    }
}

template <class T>
void write(std::stringstream &s, const T t) {
    s.write((char*)&t, sizeof(t));
}

inline bool exist(const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

FileManager::~FileManager() {
    file.close();
}

void FileManager::init(std::string file) {
    bool e = exist(file);
    auto flags = std::fstream::out | std::fstream::in | std::ios::binary;
    if (!e) flags |= std::ios::trunc;
    inst.file.open(file.c_str(), flags);
    inst.pages.clear();

    if (!e) {
        Meta m;
        inst.pages.push_back(m);
        inst.file.write((char*)&m.nextBlock, sizeof(m.nextBlock));
        inst.file.write((char*)&m.emptyPtr, sizeof(m.emptyPtr));
        std::string s(Meta::PAGE_SIZE - Meta::size(), '\0');
        inst.file<<s;
        return;
    }

    char meta[Meta::PAGE_SIZE];
    inst.file.read(meta, Meta::PAGE_SIZE);

    std::stringstream s(meta);
    size_t next, empty;
    s.read((char*)&next, sizeof(next));
    s.read((char*)&empty, sizeof(empty));
    Meta m(next, empty);
    inst.pages.push_back(m);

    inst.moveToBlock(0, Meta::size(), false);
    //TODO read & return tables
}

size_t FileManager::createTable(Table &t) {
    std::stringstream s;
    write(s, t.name.size());
    s<<t.name;
    write(s, t.tuples.size());
    for (auto i = t.tuples.begin(); i < t.tuples.end(); ++i) {
        write(s, i->type);
        if (i->type == CHAR_TYPE)
            write(s, i->name
                      .size());
        write(s, i->name);
    }
    inst.moveToBlock(0, inst.pages[0].emptyPtr);
    //TODO allow resizing meta page
    inst.file<<s.rdbuf();
    inst.pages[0].emptyPtr += size(s);
    size_t page = inst.addPage();
    TableInfo info(page, page);
    inst.tablesInfo[t.name] = info;
    return page;
}

size_t FileManager::insertBlock(const std::string table, std::vector<Tuple> tuples) {
    TableInfo &info = inst.moveToEmpty(table);
    std::vector<std::string> entries;
    for (auto i = tuples.begin(); i != tuples.end(); ++i) {
        // TODO write by tuple neither one attribute
        inst.insertRoutine(entries, *i);
        info.tuples++;
    }
    char buf[Meta::PAGE_SIZE];
    size_t page = pageNum(inst.file.tellp());

    inst.getPage(page).copy(buf, Meta::PAGE_SIZE, 0);

    size_t &j = inst.page->emptyPtr;

    for (auto i = entries.begin(); i != entries.end(); ++i) {
        if (inst.left(page) < i->size()) {
            inst.writePage(page, buf);
            info.last = page = inst.addPage();
            inst.moveToBlock(page);
            std::fill(buf, buf + Meta::PAGE_SIZE, 0);
            j = 0;
        }
        std::copy(i->begin(), i->end(), buf + j);
        j += i->size();
    }
    inst.writePage(page, buf);

    //TODO write meta to header
    info.last = pageNum(inst.file.tellp()) - 1;
    return 0;
}

size_t FileManager::insert(const std::string &table, Tuple &t) {
    return insertBlock(table, {t});
}

void FileManager::insertRoutine(std::vector<std::string>& out, const Tuple &tuple) {
    std::stringstream s;
    for (auto i = tuple.begin(); i != tuple.end(); ++i) {
        (*i)->write(s);
        out.push_back(s.str());
        s.str("");
    }
}

Tuple FileManager::getRoutine(const TemplateTuple &attr) const {
    Tuple tuple;
    std::istream* f = const_cast<std::fstream*> (&file);
    for (auto i = attr.begin(); i != attr.end(); ++i)
        tuple.push_back(read(i->type, *f));
    return tuple;
}

void FileManager::moveToBlock(size_t page, size_t offset, bool isOut) {
    size_t pos = page * Meta::PAGE_SIZE + offset;
    if (isOut) file.seekp(pos);
    else file.seekg(pos);
    inst.page = &pages[page];
}

size_t FileManager::addPage() {
    auto s = pages.size();
    file.seekp(Meta::PAGE_SIZE * (s + 1) - 1);
    file.write("", 1);
    Meta m;
    pages.push_back(m);
    return s;
}

size_t FileManager::left(size_t t) const {
    if (Meta::PAGE_SIZE <= pages[t].emptyPtr)
        return 0;
    else
        return Meta::PAGE_SIZE - pages[t].emptyPtr;
}

size_t constexpr pageNum(size_t s) {
    return s / Meta::PAGE_SIZE;
}

size_t constexpr offset(size_t s) {
    return s % Meta::PAGE_SIZE;
}

TableInfo& FileManager::moveToEmpty(const std::string &table) {
    TableInfo &info = inst.tablesInfo[table];
    inst.moveToBlock(info.last, pages[info.last].emptyPtr);
    return info;
}

std::string FileManager::getPage(size_t page) {
    moveToBlock(page, 0, false);
    char buf[Meta::PAGE_SIZE];
    file.read(buf, Meta::PAGE_SIZE);
    return std::string(buf);
}

void FileManager::writePage(size_t page, const char *str) {
    moveToBlock(page);
    file.write(str, Meta::PAGE_SIZE);
}
