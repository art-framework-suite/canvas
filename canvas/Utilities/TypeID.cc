#include "canvas/Utilities/TypeID.h"
// vim: set sw=2 expandtab :

#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/FriendlyName.h"
#include "canvas/Utilities/uniform_type_name.h"
#include "tbb/concurrent_unordered_map.h"

#include "TClass.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <typeinfo>
#include <utility>

using namespace std;

namespace art {

TypeID::
~TypeID() noexcept
{
  ti_ = nullptr;
}

TypeID::
TypeID() noexcept
  : ti_
{
  nullptr
}
{
}

TypeID::
TypeID(type_info const& ti) noexcept
  : ti_{&ti}
{
}

TypeID::
TypeID(type_info const* ti) noexcept
  : ti_{ti}
{
}

TypeID::
TypeID(TypeID const& rhs) noexcept
  : ti_{rhs.ti_}
{
}

TypeID::
TypeID(TypeID&& rhs) noexcept
  : ti_{move(rhs.ti_)}
{
}

TypeID&
TypeID::
operator=(TypeID const& rhs) noexcept
{
  if (this != &rhs) {
    ti_ = rhs.ti_;
  }
  return *this;
}

TypeID&
TypeID::
operator=(TypeID&& rhs) noexcept
{
  ti_ = move(rhs.ti_);
  return *this;
}

char const*
TypeID::
name() const
{
  return ti_->name();
}

bool
TypeID::
operator<(TypeID const& rhs) const
{
  return ti_->before(*rhs.ti_);
}

bool
TypeID::
operator==(TypeID const& rhs) const
{
  return *ti_ == *rhs.ti_;
}

TypeID::
operator bool() const
{
  return ti_ != nullptr;
}

type_info const&
TypeID::
typeInfo() const
{
  return *ti_;
}

void
TypeID::
swap(TypeID& other)
{
  std::swap(ti_, other.ti_);
}

bool
operator>(TypeID const& a, TypeID const& b)
{
  return b < a;
}

bool
operator!=(TypeID const& a, TypeID const& b)
{
  return !(a == b);
}

void
swap(TypeID& left, TypeID& right)
{
  left.swap(right);
}

void
TypeID::
print(ostream& os) const
{
  os << className();
}

string
TypeID::
className() const
{
  static tbb::concurrent_unordered_map<size_t, string> s_nameMap;
  auto hash_code = typeInfo().hash_code();
  auto entry = s_nameMap.find(hash_code);
  if (entry == s_nameMap.end()) {
    entry = s_nameMap.emplace(hash_code, uniform_type_name(typeInfo())).first;
  }
  return entry->second;
}

string
TypeID::
friendlyClassName() const
{
  return friendlyname::friendlyName(className());
}

bool
TypeID::
hasDictionary() const
{
  return TClass::HasDictionarySelection(className().c_str());
}

ostream&
operator<<(ostream& os, const TypeID& tid)
{
  tid.print(os);
  return os;
}

} // namespace art

