#include "canvas/Persistency/Provenance/Parentage.h"
#include "cetlib/MD5Digest.h"
// vim: set sw=2 expandtab :

#include <ostream>
#include <string>
#include <vector>

using namespace std;

namespace art {

  Parentage::~Parentage() = default;
  Parentage::Parentage() = default;

  Parentage::Parentage(Parentage const&) = default;
  Parentage::Parentage(Parentage&&) = default;

  Parentage& Parentage::operator=(Parentage const&) = default;
  Parentage& Parentage::operator=(Parentage&&) = default;

  Parentage::Parentage(vector<ProductID> parents) : parents_{std::move(parents)} {}

  ParentageID
  Parentage::id() const
  {
    ostringstream oss;
    for (auto const pid : parents_) {
      oss << pid << ' ';
    }
    string const stringrep{oss.str()};
    return ParentageID{cet::MD5Digest{stringrep}.digest().toString()};
  }

  vector<ProductID> const&
  Parentage::parents() const
  {
    return parents_;
  }

  ostream&
  operator<<(ostream& os, Parentage const&)
  {
    // Unimplemented
    return os;
  }

  bool
  operator==(Parentage const& a, Parentage const& b)
  {
    return a.parents() == b.parents();
  }

  bool
  operator!=(Parentage const& a, Parentage const& b)
  {
    return !(a == b);
  }

} // namespace art
