#ifndef canvas_Utilities_TypeID_h
#define canvas_Utilities_TypeID_h
// vim: set sw=2:

//
// TypeID: A unique identifier for a C++ type.
//
// The identifier is unique within an entire program, but cannot be
// persisted across invocations of the program.
//

#include "canvas/Utilities/fwd.h"
#include <iosfwd>
#include <string>
#include <typeinfo>

namespace art {
  class TypeID;

  bool operator > (TypeID const &, TypeID const &);
  bool operator != (TypeID const &, TypeID const &);
  std::ostream & operator << (std::ostream &, TypeID const &);
  void swap(TypeID &, TypeID &);
}

class art::TypeID {
public:
  TypeID();

  explicit
  TypeID(std::type_info const &);

  explicit
  TypeID(std::type_info const *);

  template<typename T>
  explicit
  TypeID(T const & val);

  // Print out the name of the type, using the reflection class name.
  void print(std::ostream &) const;

  // Returned C-style string owned by system; do not delete[] it.
  // This is the (horrible, mangled, platform-dependent) name of the type.
  char const * name() const;

  std::string className() const;

  std::string friendlyClassName() const;

  // Does ROOT have access to dictionary information for this type?
  bool hasDictionary() const;

  bool operator < (TypeID const & rhs) const;

  bool operator==(TypeID const& rhs) const;

  // Are we valid?
  explicit
  operator bool() const;

  // Access the typeinfo.
  std::type_info const & typeInfo() const;

  void swap(TypeID & other);

private:

  // NOTE: since (a) the compiler generates the type_infos, and
  // (b) they have a lifetime good for the entire application,
  // we do not have to delete it.
  // We use a pointer rather than a reference so that assignment will work
  std::type_info const* ti_;

};

inline
art::TypeID::TypeID()
  : ti_()
{
}

inline
art::TypeID::TypeID(std::type_info const & ti)
  : ti_(&ti)
{
}

inline
art::TypeID::TypeID(std::type_info const * ti)
  : ti_(ti)
{
}

template <typename T>
inline
art::TypeID::TypeID(T const& val)
    : ti_(&typeid(val))
{
}

inline
char const *
art::TypeID::name() const
{
  return ti_->name();
}

inline
bool
art::TypeID::operator < (TypeID const & rhs) const
{
  return ti_->before(*rhs.ti_);
}

inline
bool
art::TypeID::operator == (TypeID const & rhs) const
{
  return *ti_ == *rhs.ti_;
}

inline
art::TypeID::operator bool() const
{
  return ti_ != nullptr;
}

inline
std::type_info const &
art::TypeID::typeInfo() const
{
  return *ti_;
}

inline
void
art::TypeID::swap(TypeID & other)
{
  using std::swap;
  swap(ti_, other.ti_);
}

inline
bool
art::operator > (TypeID const & a, TypeID const & b)
{
  return b < a;
}

inline
bool
art::operator != (TypeID const & a, TypeID const & b)
{
  return !(a == b);
}

inline
void
art::swap (TypeID & left, TypeID & right)
{
  left.swap(right);
}

// Local Variables:
// mode: c++
// End:
#endif /* canvas_Utilities_TypeID_h */
