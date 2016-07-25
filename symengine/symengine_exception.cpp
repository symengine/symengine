#include <iostream>
#include <exception>
 
using namespace std;

namespace symengine
{
 
class DivisionByZero: public exception
{
  virtual const char* what() const throw()
  {
    return "Division By Zero";
  }
};

}
