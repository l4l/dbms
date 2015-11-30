#ifndef DBMS_DB_H
#define DBMS_DB_H

#include <vector>
#include <string>
#include <functional>

enum Type {
    INT_TYPE,
    CHAR_TYPE,
};

Type parse(std::string s);

class Value {
public:
    virtual ~Value() {};

    virtual bool operator==(const Value &) const = 0;
    virtual bool operator<(const Value &) const = 0;
    virtual std::ostream& operator<<(std::ostream&) const = 0;
    /**
     * Write binary data
     */
    virtual void write(std::ostream &out) const = 0;
};

template<class T>
class InstValue : public Value {
public:
    InstValue() {};
    InstValue(T x) : data(x) { };
    ~InstValue() {};
    
    virtual bool operator==(const Value &v) const {
        return data == dynamic_cast<const InstValue<T>&>(v).data;
    }

    virtual bool operator<(const Value &v) const {
        return data < dynamic_cast<const InstValue<T>&>(v).data;
    }

    virtual std::ostream& operator<<(std::ostream& out) const {
        return out<<data;
    }
    
    virtual void write(std::ostream &out) const {
        out.write((char*)&data, sizeof(data));
    }

private:
    T data;
};

typedef InstValue<int> Int;
typedef InstValue<std::string> String;
typedef std::vector<Value*> Tuple;

template <>
void String::write(std::ostream &) const;

class ValueDetail {
public:
    Type type;
    std::string name;

    ValueDetail(Type type, std::string value) :
            type(type), name(value) { };
};

typedef std::vector<ValueDetail> TemplateTuple;

Value* build(Type, const std::string&);

struct Table {
    std::string name;
    TemplateTuple tuples;
};

#endif //DBMS_DB_H
