#include <iostream>
#include "db.h"

template <>
void String::write(std::ostream &out) const {
    size_t s = data.size();
    out.write((char*)&s, sizeof(s));
    out.write(data.c_str(), s);
}

Value *build(Type type, const std::string &value) {
    switch (type) {
        case CHAR_TYPE:
            return new String(value);
        case INT_TYPE:
        default:
            return new Int(std::atoi(value.c_str()));
    }
}

Type parse(std::string s) {
    if (s == "int")
        return INT_TYPE;
    else
        return CHAR_TYPE;
}