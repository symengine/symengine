#include <symengine/basic.h>

using SymEngine::RCP;
using SymEngine::Basic;

namespace SymEngine
{
  RCP<const Basic> apart(const RCP<const Basic> &f)
  {
    if (is_a_Atom(*f))
    {
      return f;
    }
    //work pending here
    return f;
  }
}
