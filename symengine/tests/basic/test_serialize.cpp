#include <symengine/parser.h>
#include <symengine/serialize.h>
#include <cereal/archives/binary.hpp>

int main()
{
    SymEngine::RCP<const SymEngine::Basic> x = SymEngine::parse("x+y");
    std::ostringstream oss;
    cereal::BinaryOutputArchive boa{oss};
    boa(x);
    auto data = oss.str();
    std::cout << data << std::endl;

    std::istringstream iss(data);
    SymEngine::RCP<const SymEngine::Basic> y;
    {
        cereal::BinaryInputArchive iar{iss};
        iar(y);
    }
    std::cout << *y << std::endl;
    // logic to save to string and load from string omitted since it already
    // fails to compile
    return 0;
}
