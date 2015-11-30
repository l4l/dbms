#ifndef DBMS_PARSER_H
#define DBMS_PARSER_H

#include <vector>
#include "db.h"

typedef std::vector<std::string> Strs;

/**
 * Prepare and execute a SQL
 */
void* prepare(std::string);
/**
 * Return true if string contains correct brackets
 */
bool checkBrackets(std::string&);
/**
 * Split query by spaces and block of brackets
 */
Strs decompose(std::string&);

void create(Strs&);
void insert(Strs&);
std::vector<Tuple>* select(Strs&);

#endif //DBMS_PARSER_H
