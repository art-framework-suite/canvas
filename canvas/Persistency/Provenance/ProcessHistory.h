#ifndef canvas_Persistency_Provenance_ProcessHistory_h
#define canvas_Persistency_Provenance_ProcessHistory_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
#include "canvas/Persistency/Provenance/ProcessHistoryID.h"
#include "canvas/Persistency/Provenance/Transient.h"

#include <iosfwd>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace art {

  // This class is a ProcessHistoryID and a vector of ProcessConfiguration.
  class ProcessHistory {
  public:
    using value_type = ProcessConfiguration;
    using collection_type = std::vector<value_type>;

    using iterator = collection_type::iterator;
    using const_iterator = collection_type::const_iterator;

    using reverse_iterator = collection_type::reverse_iterator;
    using const_reverse_iterator = collection_type::const_reverse_iterator;

    using reference = collection_type::reference;
    using const_reference = collection_type::const_reference;

    using size_type = collection_type::size_type;

    // Note: threading: The ProcessHistoryID ctor can throw!
    struct Transients {
      ProcessHistoryID phid_{};
    };

    ~ProcessHistory();
    ProcessHistory();

    // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
    explicit ProcessHistory(size_type n);
    explicit ProcessHistory(collection_type const& vec);

    // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
    // Note: We do not give the strong exception safety guarantee because
    //       data_ may be modified before the transients_ ctor throws.
    // Note: We do give the basic exception safety guarantee.
    ProcessHistory(ProcessHistory const&);

    // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
    // Note: We do not give the strong exception safety guarantee because
    //       data_ may be modified before the transients_ ctor throws.
    // Note: We do give the basic exception safety guarantee.
    ProcessHistory(ProcessHistory&&);

    // Note: We do not give the strong exception safety guarantee because
    //       data_ may be modified before the transients_ ctor throws.
    // Note: We do give the basic exception safety guarantee.
    ProcessHistory& operator=(ProcessHistory const&);

    // Note: We do not give the strong exception safety guarantee because
    //       data_ may be modified before the transients_ ctor throws.
    // Note: We do give the basic exception safety guarantee.
    ProcessHistory& operator=(ProcessHistory&&);

  public: // MEMBER FUNCTIONS
    // FIXME: Try to find a way to avoid exposing this function!
    std::recursive_mutex& get_mutex() const;

    // Note: Cannot be noexcept because the ProcessHistoryID ctor can throw!
    // Note: We do not give the strong exception safety guarantee because
    //       data_ may be modified before the transients_ ctor throws.
    // Note: We do give the basic exception safety guarantee.
    void swap(ProcessHistory& other);

    // Put the given ProcessConfiguration into the history.
    // Note: Invalidates our ProcessHistoryID!
    void push_back(const_reference t);

    // Note: threading: Any user that wants to iterate over data_ must lock the
    // mutex, Note: threading: we cannot automatically protect them because
    // modifications Note: threading: to data_ must also be matched by an insert
    // into ProcessHistoryRegistry Note: threading: which is done by
    // Principal::addToProcessHistory() and an update to Note: threading: either
    // the event principal History data member or the Run, SubRun, Note:
    // threading: or Results aux_ data member, and the only way to make all that
    // atomic Note: threading: is to do it all in a critical section.  We cannot
    // use a tbb::concurrent_vector Note: threading: because it's not just data_
    // we must protect but the ProcessHistoryRegistry Note: threading: and the
    // aux_ and/or history_ members of the principals too. Note: threading: Also
    // note there are several non-multi-threaded users of the iteration Note:
    // threading: interface as well: orderedProcessNames, gallery,
    // InfoDumperInputFile, and Note: threading: FileDumperOutput_module, not to
    // mention any user module at all that calls Note: threading:
    // processHistory() on the transaction object.

    bool empty() const;
    size_type size() const;
    size_type capacity() const;

    void reserve(size_type n);

    reference operator[](size_type i);
    const_reference operator[](size_type i) const;

    reference at(size_type i);
    const_reference at(size_type i) const;

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    collection_type const& data() const;

    ProcessHistoryID const& id() const;

    std::optional<ProcessConfiguration> getConfigurationForProcess(
      std::string const& name) const;

  private:
    collection_type data_{};
    mutable Transient<Transients> transients_{};
    // FIXME-MT: This is a recursive_mutex because sometimes we must
    //           call id() from Principal::addToProcessHistory() with
    //           the mutex already locked to stall other tasks trying
    //           to call id() or getConfigurationForProcess().  We
    //           cannot protect the iteration interface, see notes
    //           above.
    mutable std::recursive_mutex mutex_{};
  };

  using ProcessHistoryMap = std::map<ProcessHistoryID const, ProcessHistory>;

  void swap(ProcessHistory& a, ProcessHistory& b);

  bool operator==(ProcessHistory const& a, ProcessHistory const& b);
  bool operator!=(ProcessHistory const& a, ProcessHistory const& b);

  bool isAncestor(ProcessHistory const& a, ProcessHistory const& b);
  bool isDescendant(ProcessHistory const& a, ProcessHistory const& b);

  std::ostream& operator<<(std::ostream& ost, ProcessHistory const& ph);

} // namespace art

#endif /* canvas_Persistency_Provenance_ProcessHistory_h */

// Local Variables:
// mode: c++
// End:
