#include "art/Utilities/Exception.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>

int main()
{
  artZ::Exception e("BadJuju");

  char array[] = "blue";
  char* ptr = array;

  e << "arghargharghargharghargh " << 3 << ' ' << 1.5 << std::endl << std::string(" boo") << ' ' << array << ' ' << ptr;


  std::string expected =
    "---- BadJuju BEGIN\n"
    "arghargharghargharghargh 3 1.5\n boo blue blue\n"
    "---- BadJuju END\n";

  std::string result(e.explainSelf());
  assert (result == expected);

  // Check to see that the following items compile; their modification
  // of the output is tested elsewhere.
  e << std::setprecision(3);
  e << std::scientific;

  artZ::Exception e2("RealBadStuff","A"); e2 << "B";
  std::string res2("---- RealBadStuff BEGIN\nA B\n---- RealBadStuff END\n");
  assert(e2.explainSelf() == res2);

  artZ::Exception e3("RealBadStuff","A "); e3 << "B";
  std::string res3("---- RealBadStuff BEGIN\nA B\n---- RealBadStuff END\n");
  assert(e3.explainSelf() == res3);

}
