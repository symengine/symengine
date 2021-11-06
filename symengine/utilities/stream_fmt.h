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
    operator std::string() const
    {
        return this->ss.str();
    }
};
}
