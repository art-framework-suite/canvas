#include "canvas/Persistency/Provenance/FileIndex.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "cetlib/container_algorithms.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <cassert>
#include <iomanip>
#include <ostream>

using namespace cet;
using namespace std;

namespace {

  bool
  subRunUnspecified(art::EventID const& eID)
  {
    // This is nasty, principally because we don't want to be
    // encouraging too many people to do this. Basically, we're
    // checking whether the only reason an EventID is invalid is
    // because its subRun number is invalid.
    return (!eID.isValid()) && eID.runID().isValid() &&
           art::EventID(art::SubRunID::firstSubRun(), eID.event()).isValid();
  }

} // unnamed namespace

namespace art {

  FileIndex::EntryNumber_t constexpr FileIndex::Element::invalid;

  FileIndex::Element::Element(EventID const& eID) : Element(eID, invalid) {}

  FileIndex::Element::Element(EventID const& eID,
                              EntryNumber_t const entryNumber)
    : eventID{eID}, entry{entryNumber}
  {}

  FileIndex::EntryType
  FileIndex::Element::getEntryType() const
  {
    return eventID.isValid() ? kEvent :
                               (eventID.subRunID().isValid() ? kSubRun : kRun);
  }

  FileIndex::iterator
  FileIndex::begin()
  {
    return entries_.begin();
  }

  FileIndex::const_iterator
  FileIndex::begin() const
  {
    return entries_.begin();
  }

  FileIndex::const_iterator
  FileIndex::cbegin() const
  {
    return entries_.begin();
  }

  FileIndex::iterator
  FileIndex::end()
  {
    return entries_.end();
  }

  FileIndex::const_iterator
  FileIndex::end() const
  {
    return entries_.end();
  }

  FileIndex::const_iterator
  FileIndex::cend() const
  {
    return entries_.end();
  }

  std::vector<FileIndex::Element>::size_type
  FileIndex::size() const
  {
    return entries_.size();
  }

  bool
  FileIndex::empty() const
  {
    return entries_.empty();
  }

  bool&
  FileIndex::allInEntryOrder() const
  {
    return transients_.get().allInEntryOrder;
  }

  bool&
  FileIndex::resultCached() const
  {
    return transients_.get().resultCached;
  }

  FileIndex::SortState&
  FileIndex::sortState() const
  {
    return transients_.get().sortState;
  }

  bool
  FileIndex::contains(EventID const& id, bool exact) const
  {
    return findPosition(id, exact) != entries_.end();
  }

  bool
  FileIndex::contains(SubRunID const& id, bool exact) const
  {
    return findPosition(id, exact) != entries_.end();
  }

  bool
  FileIndex::contains(RunID const& id, bool exact) const
  {
    return findPosition(id, exact) != entries_.end();
  }

  void
  FileIndex::addEntry(EventID const& eID, EntryNumber_t const entry)
  {
    entries_.emplace_back(eID, entry);
    resultCached() = false;
    sortState() = kNotSorted;
  }

  void
  FileIndex::addEntryOnLoad(EventID const& eID, EntryNumber_t const entry)
  {
    entries_.emplace_back(eID, entry);
    resultCached() = false;
  }

  void
  FileIndex::sortBy_Run_SubRun_Event()
  {
    stable_sort_all(entries_);
    resultCached() = false;
    sortState() = kSorted_Run_SubRun_Event;
  }

  void
  FileIndex::sortBy_Run_SubRun_EventEntry()
  {
    stable_sort_all(entries_, Compare_Run_SubRun_EventEntry());
    resultCached() = false;
    sortState() = kSorted_Run_SubRun_EventEntry;
  }

  bool
  FileIndex::allEventsInEntryOrder() const
  {
    if (!resultCached()) {
      resultCached() = true;
      EntryNumber_t maxEntry{Element::invalid};
      for (auto const& e : entries_) {
        if (e.getEntryType() == kEvent) {
          if (e.entry < maxEntry) {
            allInEntryOrder() = false;
            return false;
          }
          maxEntry = e.entry;
        }
      }
      allInEntryOrder() = true;
      return true;
    }
    return allInEntryOrder();
  }

  bool
  FileIndex::eventsUniqueAndOrdered() const
  {
    auto it = cbegin();
    auto itEnd = cend();
    // Set up the iterators to point to first two events
    // (In the trivial case where there is zero or one event,
    // the set is unique and ordered by construction).
    if (it == itEnd) {
      return true;
    }
    // Step to first event
    while (it->getEntryType() != kEvent) {
      ++it;
      if (it == itEnd) {
        return true;
      }
    }
    auto itPrevious = it;
    // Step to second event
    ++it;
    if (it == itEnd) {
      return true;
    }
    while (it->getEntryType() != kEvent) {
      ++it;
      if (it == itEnd) {
        return true;
      }
    }
    for (; it != itEnd; ++it) {
      if (it->getEntryType() == kEvent) {
        if (it->eventID <= itPrevious->eventID) {
          return false;
        }
        itPrevious = it;
      }
    }
    return true; // finished and found no duplicates
  }

  FileIndex::const_iterator
  FileIndex::findPosition(EventID const& eID) const
  {
    assert(sortState() == kSorted_Run_SubRun_Event);
    Element el{eID};
    return lower_bound_all(entries_, el);
  }

  FileIndex::const_iterator
  FileIndex::findPosition(EventID const& eID, bool exact) const
  {
    assert(sortState() == kSorted_Run_SubRun_Event);
    if (subRunUnspecified(eID)) {
      return findEventForUnspecifiedSubRun(eID, exact);
    }
    const_iterator it = findPosition(eID);
    const_iterator itEnd = entries_.end();
    while (it != itEnd && it->getEntryType() != FileIndex::kEvent) {
      ++it;
    }
    if (it == itEnd) {
      return itEnd;
    }
    if (exact && (*it != eID)) {
      return itEnd;
    }
    return it;
  }

  FileIndex::const_iterator
  FileIndex::findPosition(SubRunID const& srID, bool exact) const
  {
    assert(sortState() != kNotSorted);
    const_iterator it;
    auto const invID = EventID::invalidEvent(srID);
    if (sortState() == kSorted_Run_SubRun_EventEntry) {
      Element const el{invID};
      it = lower_bound_all(entries_, el, Compare_Run_SubRun_EventEntry());
    } else {
      it = findPosition(invID);
    }
    auto const itEnd = entries_.cend();
    while (it != itEnd && it->getEntryType() != FileIndex::kSubRun) {
      ++it;
    }
    if (it == itEnd) {
      return itEnd;
    }
    if (exact && (it->eventID.subRunID() != srID)) {
      return itEnd;
    }
    return it;
  }

  FileIndex::const_iterator
  FileIndex::findPosition(RunID const& rID, bool exact) const
  {
    assert(sortState() != kNotSorted);
    const_iterator it;
    auto const invID = EventID::invalidEvent(rID);
    if (sortState() == kSorted_Run_SubRun_EventEntry) {
      Element const el{invID};
      it = lower_bound_all(entries_, el, Compare_Run_SubRun_EventEntry());
    } else {
      it = findPosition(invID);
    }
    auto const itEnd = entries_.cend();
    while (it != itEnd && it->getEntryType() != FileIndex::kRun) {
      ++it;
    }
    if (it == itEnd) {
      return itEnd;
    }
    if (exact && (it->eventID.runID() != rID)) {
      return itEnd;
    }
    return it;
  }

  FileIndex::const_iterator
  FileIndex::findSubRunOrRunPosition(SubRunID const& srID) const
  {
    assert(sortState() != kNotSorted);
    const_iterator it;
    if (sortState() == kSorted_Run_SubRun_EventEntry) {
      Element el{EventID::invalidEvent(srID)};
      it = lower_bound_all(entries_, el, Compare_Run_SubRun_EventEntry());
    } else {
      it = findPosition(EventID::invalidEvent(srID));
    }
    auto const itEnd = entries_.cend();
    while (it != itEnd && it->getEntryType() != FileIndex::kSubRun &&
           it->getEntryType() != FileIndex::kRun) {
      ++it;
    }
    return it;
  }

  void
  FileIndex::print_event_list(ostream& os) const
  {
    os << "\nPrinting the list of Runs, SubRuns, and Events stored in the root "
          "file.\n\n";
    os << setw(15) << "Run" << setw(15) << "SubRun" << setw(15) << "Event"
       << "\n";
    for (auto const& e : entries_) {
      if (e.getEntryType() == FileIndex::kEvent) {
        os << setw(15) << e.eventID.run() << setw(15) << e.eventID.subRun()
           << setw(15) << e.eventID.event() << "\n";
      } else if (e.getEntryType() == FileIndex::kSubRun) {
        os << setw(15) << e.eventID.run() << setw(15) << e.eventID.subRun()
           << setw(15) << " "
           << "\n";
      } else if (e.getEntryType() == FileIndex::kRun) {
        os << setw(15) << e.eventID.run() << setw(15) << " " << setw(15) << " "
           << "\n";
      }
    }
  }

  FileIndex::const_iterator
  FileIndex::findEventForUnspecifiedSubRun(EventID const& eID, bool exact) const
  {
    RunID const& runID = eID.runID();
    EventNumber_t event = eID.event();
    SubRunID last_subRunID;
    // Try to find the event.
    auto const firstEvent =
      findPosition(EventID::firstEvent(SubRunID::firstSubRun(runID)), false);
    const_iterator it = firstEvent;
    const_iterator const itEnd = entries_.end();
    if (it == itEnd) {
      return it;
    }

    // Starting with it, jump to the first event of each subrun until
    // we find either:
    //
    // 1. The next run.
    // 2. An event number higher than we want.
    // 3. The end of the file index.
    while ((it != itEnd) && (it->eventID.runID() == runID) &&
           (it->eventID.event() < event)) {
      last_subRunID = it->eventID.subRunID();
      // Get the first event in the next subrun.
      it =
        findPosition(EventID::firstEvent(it->eventID.subRunID().next()), false);
    }
    const_iterator result = itEnd;
    if ((it != itEnd) && (it->eventID.runID() == runID) &&
        (it->eventID.event() == event)) {
      // We started on the correct event.
      result = it;
    } else if (last_subRunID.isValid()) {
      // Find the event in the last subrun.
      result = findPosition(EventID(last_subRunID, event), exact);
    }
    if (result == itEnd) {
      // Did not find anything.
      mf::LogWarning("FileIndex")
        << "Could not find incompletely specified event " << eID
        << " with smart algorithm:\n"
        << "Assuming pathological file structure (event selection?) and\n"
        << "trying again (inefficient).\n"
        << "NOTE: this will find only the event with matching event number "
        << "and the\n"
        << "      lowest subrun number: any others are inaccessible via this "
        << "method.";
      SubRunID trySubRun{SubRunID::firstSubRun(runID)};
      // Try to find the highest subrun number in this run.
      const_iterator findIt(firstEvent);
      SubRunID lastSubRunInRun{trySubRun};
      for (; findIt != itEnd && findIt->eventID.runID() == runID;
           findIt =
             findPosition(EventID::firstEvent(lastSubRunInRun.next()), false)) {
        lastSubRunInRun = findIt->eventID.subRunID();
      }
      // Now loop through each subrun looking for an exact match to our event.
      while ((findIt = findPosition(EventID(trySubRun, event), true)) ==
               itEnd &&
             trySubRun < lastSubRunInRun) {
        trySubRun = trySubRun.next();
      }
      result = findIt;
    }
    return result;
  }

  bool
  operator<(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return lh.eventID < rh.eventID;
  }

  bool
  operator>(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return rh < lh;
  }

  bool
  operator>=(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return !(lh < rh);
  }

  bool
  operator<=(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return !(rh < lh);
  }

  bool
  operator==(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return !(lh < rh || rh < lh);
  }

  bool
  operator!=(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return !(lh == rh);
  }

  bool
  operator==(FileIndex const& lh, FileIndex const& rh)
  {
    return lh.entries_ == rh.entries_;
  }

  bool
  operator!=(FileIndex const& lh, FileIndex const& rh)
  {
    return !(lh == rh);
  }

  bool
  Compare_Run_SubRun_EventEntry::operator()(FileIndex::Element const& lh,
                                            FileIndex::Element const& rh)
  {
    if (lh.eventID.subRunID() == rh.eventID.subRunID()) {
      if ((!lh.eventID.isValid()) && (!rh.eventID.isValid())) {
        return false;
      } else if (!lh.eventID.isValid()) {
        return true;
      } else if (!rh.eventID.isValid()) {
        return false;
      }
      return lh.entry < rh.entry;
    }
    return lh.eventID.subRunID() < rh.eventID.subRunID();
  }

  ostream&
  operator<<(ostream& os, FileIndex::Element const& el)
  {
    os << el.eventID << ": entry# " << el.entry;
    return os;
  }

  ostream&
  operator<<(ostream& os, FileIndex const& fileIndex)
  {
    os << "\nPrinting FileIndex contents.  This includes a list of all Runs, "
          "SubRuns\n"
       << "and Events stored in the root file.\n\n";
    os << setw(15) << "Run" << setw(15) << "SubRun" << setw(15) << "Event"
       << setw(15) << "TTree Entry"
       << "\n";
    for (auto const& e : fileIndex) {
      if (e.getEntryType() == FileIndex::kEvent) {
        os << setw(15) << e.eventID.run() << setw(15) << e.eventID.subRun()
           << setw(15) << e.eventID.event() << setw(15) << e.entry << "\n";
      } else if (e.getEntryType() == FileIndex::kSubRun) {
        os << setw(15) << e.eventID.run() << setw(15) << e.eventID.subRun()
           << setw(15) << " " << setw(15) << e.entry << "  (SubRun)"
           << "\n";
      } else if (e.getEntryType() == FileIndex::kRun) {
        os << setw(15) << e.eventID.run() << setw(15) << " " << setw(15) << " "
           << setw(15) << e.entry << "  (Run)"
           << "\n";
      }
    }
    return os;
  }

} // namespace art
