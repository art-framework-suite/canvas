#include "canvas/Persistency/Common/Sampled.h"

using namespace art;

int
main()
{
  [[maybe_unused]] Sampled<Sampled<int>> s;
}
