#include "canvas/Persistency/Provenance/Hash.h"
// vim: set sw=2 expandtab :

namespace art {
  namespace detail {

    // This string is the 16-byte, non-printable version.
    std::string const&
    InvalidHash()
    {
      static const std::string invalid = cet::MD5Result().compactForm();
      return invalid;
    }

  } // namespace detail
} // namespace art
