#include "canvas/Persistency/Provenance/ProcessHistory.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/Transient.h"
#include "cetlib/MD5Digest.h"
#include "cetlib/container_algorithms.h"
#include "fhiclcpp/ParameterSetID.h"

#include <iterator>
#include <ostream>
#include <utility>

using namespace cet;
using namespace std;

namespace art {

  ProcessHistory::~ProcessHistory() = default;
  ProcessHistory::ProcessHistory() = default;

  // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
  ProcessHistory::ProcessHistory(size_type const n) : data_(n) {}

  // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
  ProcessHistory::ProcessHistory(collection_type const& vec) : data_(vec) {}

  // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
  // Note: We do not give the strong exception safety guarantee because
  //       data_ may be modified before the transients_ ctor throws.
  // Note: We do give the basic exception safety guarantee.
  ProcessHistory::ProcessHistory(ProcessHistory const& rhs)
    : data_(rhs.data_), transients_{rhs.transients_}
  {}

  // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
  // Note: We do not give the strong exception safety guarantee because
  //       data_ may be modified before the transients_ ctor throws.
  // Note: We do give the basic exception safety guarantee.
  ProcessHistory::ProcessHistory(ProcessHistory&& rhs)
    : data_(std::move(rhs.data_)), transients_{std::move(rhs.transients_)}
  {}

  // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
  // Note: We do not give the strong exception safety guarantee because
  //       data_ may be modified before the transients_ ctor throws.
  // Note: We do give the basic exception safety guarantee.
  ProcessHistory&
  ProcessHistory::operator=(ProcessHistory const& rhs)
  {
    if (this != &rhs) {
      data_ = rhs.data_;
      transients_ = rhs.transients_;
    }
    return *this;
  }

  // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
  // Note: We do not give the strong exception safety guarantee because
  //       data_ may be modified before the transients_ ctor throws.
  // Note: We do give the basic exception safety guarantee.
  ProcessHistory&
  ProcessHistory::operator=(ProcessHistory&& rhs)
  {
    data_ = std::move(rhs.data_);
    transients_ = std::move(rhs.transients_);
    return *this;
  }

  std::recursive_mutex&
  ProcessHistory::get_mutex() const
  {
    return mutex_;
  }

  // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
  // Note: We do not give the strong exception safety guarantee because
  //       data_ may be modified before the transients_ ctor throws.
  // Note: We do give the basic exception safety guarantee.
  void
  ProcessHistory::swap(ProcessHistory& other)
  {
    data_.swap(other.data_);
    transients_.get().phid_.swap(other.transients_.get().phid_);
  }

  // Put the given ProcessConfiguration into the history.
  // Note: Invalidates our ProcessHistoryID!
  void
  ProcessHistory::push_back(const_reference t)
  {
    data_.push_back(t);
    transients_.get().phid_ = ProcessHistoryID();
  }

  bool
  ProcessHistory::empty() const
  {
    return data_.empty();
  }

  ProcessHistory::size_type
  ProcessHistory::size() const
  {
    return data_.size();
  }

  ProcessHistory::size_type
  ProcessHistory::capacity() const
  {
    return data_.capacity();
  }

  void
  ProcessHistory::reserve(size_type n)
  {
    data_.reserve(n);
  }

  ProcessHistory::reference ProcessHistory::operator[](size_type i)
  {
    return data_[i];
  }

  ProcessHistory::const_reference ProcessHistory::operator[](size_type i) const
  {
    return data_[i];
  }

  ProcessHistory::reference
  ProcessHistory::at(size_type i)
  {
    return data_.at(i);
  }

  ProcessHistory::const_reference
  ProcessHistory::at(size_type i) const
  {
    return data_.at(i);
  }

  ProcessHistory::const_iterator
  ProcessHistory::begin() const
  {
    return data_.begin();
  }

  ProcessHistory::const_iterator
  ProcessHistory::end() const
  {
    return data_.end();
  }

  ProcessHistory::const_iterator
  ProcessHistory::cbegin() const
  {
    return data_.cbegin();
  }

  ProcessHistory::const_iterator
  ProcessHistory::cend() const
  {
    return data_.cend();
  }

  ProcessHistory::const_reverse_iterator
  ProcessHistory::rbegin() const
  {
    return data_.rbegin();
  }

  ProcessHistory::const_reverse_iterator
  ProcessHistory::rend() const
  {
    return data_.rend();
  }

  ProcessHistory::const_reverse_iterator
  ProcessHistory::crbegin() const
  {
    return data_.crbegin();
  }

  ProcessHistory::const_reverse_iterator
  ProcessHistory::crend() const
  {
    return data_.crend();
  }

  ProcessHistory::collection_type const&
  ProcessHistory::data() const
  {
    return data_;
  }

  ProcessHistoryID const&
  ProcessHistory::id() const
  {
    // Note: threading: We may be called by Principal::addProcessEntry()
    // Note: threading: with the mutex already locked, so we use
    // a recursive mutex.
    std::lock_guard sentry{mutex_};
    if (transients_.get().phid_.isValid()) {
      return transients_.get().phid_;
    }
    // This implementation is ripe for optimization.
    // We do not use operator<< because it does not write out everything.
    ostringstream oss;
    for (auto I = data_.begin(), E = data_.end(); I != E; ++I) {
      oss << I->processName() << ' ' << I->parameterSetID() << ' '
          << I->releaseVersion() << ' '
          << ' '; // retain extra spaces for backwards compatibility
    }
    string stringrep = oss.str();
    cet::MD5Digest md5alg(stringrep);
    ProcessHistoryID tmp(md5alg.digest().toString());
    transients_.get().phid_.swap(tmp);
    return transients_.get().phid_;
  }

  std::optional<ProcessConfiguration>
  ProcessHistory::getConfigurationForProcess(string const& name) const
  {
    std::lock_guard sentry{mutex_};
    for (const_iterator i = data_.begin(), e = data_.end(); i != e; ++i) {
      if (i->processName() == name) {
        return std::make_optional(*i);
      }
    }
    return std::nullopt;
  }

  void
  swap(ProcessHistory& a, ProcessHistory& b)
  {
    a.swap(b);
  }

  bool
  operator==(ProcessHistory const& a, ProcessHistory const& b)
  {
    return a.data() == b.data();
  }

  bool
  operator!=(ProcessHistory const& a, ProcessHistory const& b)
  {
    return !(a == b);
  }

  bool
  isAncestor(ProcessHistory const& a, ProcessHistory const& b)
  {
    if (a.size() >= b.size()) {
      return false;
    }
    for (auto itA = a.data().cbegin(),
              itB = b.data().cbegin(),
              itAEnd = a.data().cend();
         itA != itAEnd;
         ++itA, ++itB) {
      if (*itA != *itB) {
        return false;
      }
    }
    return true;
  }

  bool
  isDescendant(ProcessHistory const& a, ProcessHistory const& b)
  {
    return isAncestor(b, a);
  }

  ostream&
  operator<<(ostream& ost, ProcessHistory const& ph)
  {
    ost << "Process History = ";
    copy_all(ph, ostream_iterator<ProcessHistory::value_type>(ost, ";"));
    return ost;
  }

} // namespace art
