#include "canvas/Persistency/Common/EDProductGetter.h"
// vim: set sw=2 expandtab :

namespace art {

  EDProductGetter::~EDProductGetter() = default;
  EDProductGetter::EDProductGetter() = default;

  EDProduct const*
  EDProductGetter::getIt() const
  {
    return getIt_();
  }

  EDProduct const*
  EDProductGetter::getIt_() const
  {
    return nullptr;
  }

} // namespace art
