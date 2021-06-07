#pragma once
#include <sstream>
#include <string>
namespace SymEngine {
class StreamFmt {
    std::stringstream ss;
public:
    StreamFmt() {}
    ~StreamFmt() {}

    template <typename T>
    StreamFmt& operator<<(const T& val) {
        this->ss << val;
        return *this;
    }
    // std::string str() const {
    //     return this->ss.str();
    // }
    operator std::string() const
    {
        return this->ss.str();
    }
};
}
