#include "canvas/Persistency/Common/PtrVector.h"

using namespace art;

int main(){
  const PtrVector<std::vector<int>>& pc1 {};
  PtrVector<std::vector<char>> pc = PtrVector<std::vector<char>>(pc1); 
}
