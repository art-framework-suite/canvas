#ifndef canvas_Utilities_TypeID_h
#define canvas_Utilities_TypeID_h
// vim: set sw=2 expandtab :

//
// TypeID: A unique identifier for a C++ type.
//
// The identifier is unique within an entire program (not really,
// it is supposed to be, but gcc violates the standard requirement
// that a type_info should have a unique address for performance
// reasons) but cannot be persisted across invocations of the program.
//

#include <iosfwd>
#include <string>
#include <typeinfo>
#include <utility>

namespace art {

class TypeID {

public: // MEMBER FUNCTIONS -- Special Member Functions

  ~TypeID() noexcept;

  TypeID() noexcept;

  explicit
  TypeID(std::type_info const&) noexcept;

  explicit
  TypeID(std::type_info const*) noexcept;

  template<typename T>
  explicit
  TypeID(T const& val) noexcept;

  TypeID(TypeID const&) noexcept;

  TypeID(TypeID&&) noexcept;

  TypeID&
  operator=(TypeID const&) noexcept;

  TypeID&
  operator=(TypeID&&) noexcept;

public: // MEMBER FUNCTIONS -- API for the user

  void
  print(std::ostream&) const;

  char const*
  name() const;

  std::string
  className() const;

  std::string
  friendlyClassName() const;

  bool
  hasDictionary() const;

  bool
  operator<(TypeID const&) const;

  bool
  operator==(TypeID const&) const;

  explicit
  operator bool() const;

  std::type_info const&
  typeInfo() const;

  void
  swap(TypeID&);

private: // MEMBER DATA

  std::type_info const*
  ti_{nullptr};

};

template <typename T>
TypeID::
TypeID(T const& val) noexcept
  : ti_{&typeid(val)}
{
}

bool
operator>(TypeID const&, TypeID const&);

bool
operator!=(TypeID const&, TypeID const&);

std::ostream&
operator<<(std::ostream&, TypeID const&);

void
swap(TypeID&, TypeID&);

} // namespace art

// Local Variables:
// mode: c++
// End:
#endif /* canvas_Utilities_TypeID_h */
