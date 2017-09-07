#ifndef canvas_Persistency_Provenance_TypeWithDict_h
#define canvas_Persistency_Provenance_TypeWithDict_h
// vim: set sw=2 expandtab :

#include "canvas/Utilities/TypeID.h"

#include <iosfwd>
#include <string>
#include <typeinfo>

class TClass;
class TDataType;
class TDictionary;
class TEnum;

namespace art {

TypeID
getTypeID(int);

class TypeWithDict {

public: // TYPES

  enum class Category {
    NONE, // 0
    CLASSTYPE, // 1
    ENUMTYPE, // 2
    BASICTYPE // 3
  };

public: // MEMBER FUNCTIONS -- Special Member Functions

  ~TypeWithDict() noexcept;

  TypeWithDict() noexcept;

  explicit
  TypeWithDict(std::type_info const& t);

  explicit
  TypeWithDict(TypeID const& id);

  explicit
  TypeWithDict(std::string const& name);

  TypeWithDict(TypeWithDict const&) noexcept;

  TypeWithDict(TypeWithDict&&) noexcept;

  TypeWithDict&
  operator=(TypeWithDict const&) noexcept;

  TypeWithDict&
  operator=(TypeWithDict&&) noexcept;

public: // MEMBER FUNCTIONS -- API for the user

  TDictionary*
  tDictionary() const noexcept;

  Category
  category() const noexcept;

  TypeID const&
  id() const noexcept;

  explicit
  operator bool() const noexcept;

  std::type_info const&
  typeInfo() const;

  void
  print(std::ostream& os) const;

  char const*
  name() const;

  std::string
  className() const;

  std::string
  friendlyClassName() const;

  TClass*
  tClass() const;

  TEnum*
  tEnum() const;

  TDataType*
  tDataType() const;

private: // MEMBER FUNCTIONS -- Implementation details

  static
  TDictionary*
  dictFromTypeInfo_(std::type_info const& t);

  static
  TDictionary*
  dictFromName_(std::string const& name);

  static
  Category
  categoryFromDict_(TDictionary* tDict);

  static
  TypeID
  typeIDFromDictAndCategory_(TDictionary* tDict, Category category);

private: // MEMBER DATA

  TDictionary*
  tDict_{nullptr};

  Category
  category_{Category::NONE};

  TypeID
  id_{};

};

std::string
to_string(TypeWithDict::Category category);

std::ostream&
operator<<(std::ostream& os, TypeWithDict::Category category);

std::ostream&
operator<<(std::ostream& os, TypeWithDict const& ty);

} // namespace art

// Local Variables:
// mode: c++
// End:
#endif /* canvas_Persistency_Provenance_TypeWithDict_h */
