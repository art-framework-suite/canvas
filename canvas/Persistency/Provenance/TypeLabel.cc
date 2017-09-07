#include "canvas/Persistency/Provenance/TypeLabel.h"
// vim: set sw=2 expandtab :

#include "canvas/Utilities/TypeID.h"

#include <string>
#include <tuple>

using namespace std;

namespace art {

TypeLabel::
TypeLabel(TypeID const& itemtype, string const& instanceName, string const& emulatedMod /* = {} */)
  : typeID_{itemtype}
  , productInstanceName_{instanceName}
  , emulatedModule_{emulatedMod}
{
}

TypeID const&
TypeLabel::
typeID() const
{
  return typeID_;
}

//string
//TypeLabel::
//className() const
//{
//  return typeID_.className();
//}

//string
//TypeLabel::
//friendlyClassName() const
//{
//  return typeID_.friendlyClassName();
//}

string const&
TypeLabel::
productInstanceName() const
{
  return productInstanceName_;
}

string const&
TypeLabel::
emulatedModule() const
{
  return emulatedModule_;
}

//bool
//TypeLabel::
//hasEmulatedModule() const
//{
//  return !emulatedModule_.empty();
//}

// Types with the same friendlyClassName are in the same equivalence
// class for the purposes of this comparison.
bool
operator<(TypeLabel const& a, TypeLabel const& b)
{
  auto const& a_class_name = a.typeID().className();
  auto const& b_class_name = b.typeID().className();
  return tie(a.emulatedModule_, a.productInstanceName_, a_class_name) <
         tie(b.emulatedModule_, b.productInstanceName_, b_class_name);
}

} // namespace art

