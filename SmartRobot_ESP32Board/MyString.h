#ifndef mString
#define mString
#include <cstdlib>
#include <cstring>
#include <cstdio>
#pragma warning(disable : 4996)

static constexpr unsigned int capacity = 1000;

class MyString {
public:
    MyString() : mLength(0), data("") {}
    MyString(const char* info) : mLength(strlen(info)) {
        std::strcpy(data, info);
    }
    MyString& operator=(const char* m) {
        if (strlen(m) > size() - 1) return *this;

        std::strcpy(data, m);
        mLength = strlen(m);
        return *this;
    }
    MyString& operator=(const MyString& other) {
        if (other.length() > this->size() - 1) return *this; //Can't copy it because there isn't space

        std::strcpy(data, other.data);
        mLength = other.length();
        return *this;
    }
    MyString& operator=(const int i) {
        char buf[10];
        std::sprintf(buf, "%d", i);
        if (strlen(buf) > size() - 1) return *this;
        std::strcpy(data, buf);
        mLength = strlen(buf);
		return *this;
    }
    MyString& operator=(const double d) {
        char buf[10];
        std::sprintf(buf, "%.2f", d);
        if (strlen(buf) > size() - 1) return *this;
        std::strcpy(data, buf);
        mLength = strlen(buf);
    }

    operator const char* () const {
        return data;
    }

    void operator+=(const char* m) {
        concat(m);
    }

    void operator+=(const MyString& other) {
        concat(other);
    }

    void operator+=(const double d) {
        concat(d);
    }

    void operator+=(const int i) {
        concat(i);
    }

    void operator+=(const char c) {
        concat(c);
    }

    const char* operator+(const char* m) {
        char ret[capacity];

        std::strcpy(ret, data);
        std::strcat(ret, m);
        return ret;
    }

    const char* operator+(const MyString& other) {
        char ret[capacity];

        std::strcpy(ret, data);
        std::strcat(ret, other.data);

        return ret;
    }

    const bool operator==(const MyString& other) {
        return std::strcmp(data, other.data) == 0;
    }

    const bool operator==(const char* other) {
        return std::strcmp(data, other) == 0;
    }

    const bool operator!=(const MyString& other) {
        return !((*this) == other);
    }

    const bool operator!=(const char* other) {
        return !((*this) == other);
    }

    void concat(const double d) {
        char buf[10];
        std::sprintf(buf, "%.2f", d);
        if (mLength + strlen(buf) >= size() - 1);

        std::strcat(data, buf);
        mLength += strlen(buf);
    }

    void concat(const int i) {
        char buf[10];
        std::sprintf(buf, "%d", i);
        if (mLength + strlen(buf) >= size() - 1) return;
        std::strcat(data, buf);
        mLength += strlen(buf);
    }

    void concat(const MyString& other) {
        if (mLength + other.length() > size() - 1) return;

        std::strcat(data, other.data);
        mLength += other.length();
    }

    void concat(const char* m) {
        if (strlen(m) + mLength > size()) return;

        std::strcat(data, m);
        mLength += strlen(m);
    }

    void concat(const char c) {
        if (mLength != size() - 1) {
            data[mLength] = c;
            ++mLength;
            data[mLength] = '\0'; // Null-terminate the string
        }
    }

    MyString substring(unsigned int start, unsigned int length = capacity) const {
        MyString newStr;

        if (start + length > mLength) length = mLength - start;
        for (unsigned int i = start; i < start + length; ++i) {
            newStr.concat(this->data[i]);
        }
        return newStr;
    }

    unsigned int indexOf(const char c, unsigned int startIndex = 0) const {
        for (unsigned int i = startIndex; i < mLength; ++i) {
            if (data[i] == c) 
                return i;
        }

        return 4294967295;
    }

    const char charAt(unsigned int index) const {
        if (index > mLength) return '\0';

        return data[index];
    }

    void clear() {
        mLength = 0;
        std::strcpy(data, "");
    }

    void pop_back(){
        if(mLength > 0){
            --mLength;
            data[mLength] = '\0';
        }
    }

    const char last() const {
        if(mLength == 0) return '\0';
        return data[mLength - 1];
    }

    const double toDouble() const {
        return std::atof(data);
    }

    const int toInt() const {
        return std::atoi(data);
    }

    const char* c_str() const{
        return data;
    }

    constexpr unsigned int length() const { return mLength; }
    constexpr unsigned int size() const { return capacity; }
private:
    char data[capacity];
    unsigned int mLength;
};

#endif //mString