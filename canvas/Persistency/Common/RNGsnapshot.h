#ifndef canvas_Persistency_Common_RNGsnapshot_h
#define canvas_Persistency_Common_RNGsnapshot_h
// vim: set sw=2 expandtab :

//
// A data product holding saved state from/for the
// RandomNumberGenerator
//
//
// Notes:
//
// CLHEP specifies that the state of any of its engines will be returned
// as a vector<unsigned long>.  However, the size of such a long is
// machine-dependent.  If unsigned long is an 8 byte variable, only the
// least significant 4 bytes are filled and the most significant 4 bytes
// are zero.  We choose to store only the useful 32 bits, making the
// assumption (verified at compile-time) that unsigned int is always a
// 32-bit type.
//
// We would prefer to be more explicit about this by using a standard
// typedef such as saved_t32_t.  However, ROOT would have issues with this
// since ROOT sees only the underlying type, and not the typedef proper.
// Thus values written on one system and read on another would be
// problematic whenever the two systems did not agree on the underlying
// type.
//

#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace art {

  class RNGsnapshot {

  public:
    static_assert(std::numeric_limits<unsigned>::digits == 32,
                  "std::numeric_limits<unsigned>::digits != 32");
    static_assert(sizeof(unsigned) <= sizeof(unsigned long),
                  "sizeof(unsigned) > sizeof(unsigned long)");

    RNGsnapshot() = default;

    explicit RNGsnapshot(std::string const& ekind,
                         std::string const& label,
                         std::vector<unsigned long> const& est);

    // -- Access
    std::string const&
    ekind() const
    {
      return engine_kind_;
    }

    std::string const&
    label() const
    {
      return label_;
    }

    std::vector<unsigned> const&
    state() const
    {
      return state_;
    }

    // -- Save/restore
    void saveFrom(std::string const&,
                  std::string const&,
                  std::vector<unsigned long> const&);

    std::vector<unsigned long> restoreState() const;

  private:
    std::string engine_kind_{};

    std::string label_{};

    std::vector<unsigned> state_{};
  };

} // namespace art

#endif /* canvas_Persistency_Common_RNGsnapshot_h */

// Local Variables:
// mode: c++
// End:
