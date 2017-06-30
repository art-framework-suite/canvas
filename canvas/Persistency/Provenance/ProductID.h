#ifndef canvas_Persistency_Provenance_ProductID_h
#define canvas_Persistency_Provenance_ProductID_h

//=====================================================================
// ProductID: A unique identifier for each EDProduct within a process.
//=====================================================================

#include "cetlib/crc32.h"

#include <iosfwd>
#include <string>

namespace art {

  class ProductID {
  public:
    using value_type = unsigned int;

  private:

    // For an invalid product name, we choose the empty string since
    // no product could be retrieved from such a specification.
    // Conveniently, the value associated with the empty string is 0.
    static constexpr value_type invalid_value() {return toID("");}

  public:

    constexpr ProductID() = default;
    explicit ProductID(std::string const& canonicalProductName);
    constexpr explicit ProductID(char const* canonicalProductName);
    constexpr explicit ProductID(value_type const value);

    void setID(std::string const& canonicalProductName) {value_ = toID(canonicalProductName);}

    constexpr bool isValid() const {return value_ != invalid_value();}
    constexpr auto value() const {return value_;}

    constexpr bool operator<(ProductID const rh) const {return value_ < rh.value_;}
    constexpr bool operator>(ProductID const rh) const {return rh < *this;}
    constexpr bool operator==(ProductID const rh) const {return value_ == rh.value_;}
    constexpr bool operator!=(ProductID const rh) const {return !(*this == rh); }

    static constexpr ProductID invalid();

    struct Hash {
      std::size_t operator()(ProductID const pid) const
      {
        return pid.value(); // since the ID is already a checksum, don't
                            // worry about further hashing
      }
    };

  private:

    static value_type toID(std::string const& branchName);
    static constexpr value_type toID(char const* canonicalProductName);
    friend class ProductIDStreamer;

    value_type value_{invalid_value()};
  };

  std::ostream&
  operator<<(std::ostream& os, ProductID const id);

  //====================================================
  // constexpr member-function implementations
  constexpr
  ProductID::ProductID(value_type const value) :
    value_{value}
  {}

  constexpr
  ProductID::ProductID(char const* canonicalProductName) :
    ProductID{toID(canonicalProductName)}
  {}

  constexpr
  ProductID::value_type
  ProductID::toID(char const* canonicalProductName)
  {
    return cet::crc32{canonicalProductName}.digest();
  }

  constexpr
  ProductID ProductID::invalid()
  {
    static_assert(invalid_value() == 0, "Invalid ProductID does not have a value of 0!");
    return ProductID{invalid_value()};
  }


}
#endif /* canvas_Persistency_Provenance_ProductID_h */

// Local Variables:
// mode: c++
// End:
