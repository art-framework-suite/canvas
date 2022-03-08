#include "canvas/Persistency/Provenance/Hash.h"
// vim: set sw=2 expandtab :
#include "cetlib/MD5Digest.h"
#include "cetlib/container_algorithms.h"

namespace {
  std::string const invalid{cet::MD5Result{}.compactForm()};
}

namespace art::detail {
  void
  fixup(std::string& hash)
  {
    if (hash.size() == 16) {
      // Already in compact form.
      return;
    }
    if (hash.size() == 0) {
      hash = art::detail::InvalidHash();
      return;
    }
    if (hash.size() == 32) {
      cet::MD5Result md5;
      md5.fromHexifiedString(hash);
      hash = md5.compactForm();
      return;
    }
    throw Exception(errors::LogicError)
      << "art::Hash<> instance with data in illegal state:\n"
      << hash << "\nPlease report this to the core framework developers";
  }

  std::string
  hash_to_string(std::string const& hash)
  {
    cet::MD5Result temp;
    cet::copy_all(hash, temp.bytes);
    return temp.toString();
  }

  // This string is the 16-byte, non-printable version.
  std::string const&
  InvalidHash()
  {
    return invalid;
  }
}
