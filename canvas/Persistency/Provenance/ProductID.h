#ifndef canvas_Persistency_Provenance_ProductID_h
#define canvas_Persistency_Provenance_ProductID_h

//=====================================================================
// ProductID: A unique identifier for each EDProduct within a process.
//=====================================================================

#include <iosfwd>
#include <string>

namespace art {

  class ProductID {
  public:

    typedef unsigned int value_type;

    ProductID() = default;
    explicit ProductID(std::string const& canonicalProductName);
    explicit ProductID(value_type const value);

    void setID(std::string const& canonicalProductName) {value_ = toID(canonicalProductName);}
    bool isValid() const {return value_ != 0;}
    auto value() const {return value_;}

    bool operator<(ProductID const rh) const {return value_ < rh.value_;}
    bool operator>(ProductID const rh) const {return rh < *this;}
    bool operator==(ProductID const rh) const {return value_ == rh.value_;}
    bool operator!=(ProductID const rh) const {return !(*this == rh); }

    struct Hash {
      std::size_t operator()(ProductID const pid) const
      {
        return pid.value(); // since the ID is already a checksum, don't
                            // worry about further hashing
      }
    };

  private:
    static value_type toID(std::string const& branchName);
    friend class ProductIDStreamer;
    value_type value_{};
  };

  std::ostream&
  operator<<(std::ostream& os, ProductID const id);
}
#endif /* canvas_Persistency_Provenance_ProductID_h */

// Local Variables:
// mode: c++
// End:
