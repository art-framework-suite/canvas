#define BOOST_TEST_MODULE (FileIndex_t)
#include "boost/test/unit_test.hpp"
#include "canvas/Persistency/Provenance/FileIndex.h"

namespace art {
  std::ostream&
  boost_test_print_type(std::ostream& os, FileIndex::iterator it)
  {
    return os << &it;
  }
  std::ostream&
  boost_test_print_type(std::ostream& os, FileIndex::const_iterator it)
  {
    return os << &it;
  }
}

using art::EventID;
using art::RunID;
using art::SubRunID;

BOOST_AUTO_TEST_SUITE(FileIndex_t)

BOOST_AUTO_TEST_CASE(constructAndInsertTest)
{
  art::FileIndex fileIndex;
  BOOST_TEST(fileIndex.empty());
  BOOST_TEST(fileIndex.size() == 0);
  BOOST_TEST(fileIndex.begin() == fileIndex.end());

  fileIndex.addEntry(EventID(100, 101, 102), 1);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(200, 201)), 2);
  fileIndex.addEntry(EventID(300, 0, 1), 3);
  fileIndex.addEntry(EventID::invalidEvent(RunID(300)), 3);
  fileIndex.addEntry(EventID(100, 101, 103), 3);

  BOOST_TEST(!fileIndex.empty());
  BOOST_TEST(fileIndex.size() == 5);

  auto it = fileIndex.begin();
  BOOST_TEST(it->eventID == EventID(100, 101, 102));
  BOOST_TEST(it->entry == 1);
  BOOST_TEST(it->getEntryType() == art::FileIndex::kEvent);

  ++it;
  BOOST_TEST(it->getEntryType() == art::FileIndex::kSubRun);

  ++it;
  BOOST_TEST(it->getEntryType() == art::FileIndex::kEvent);

  ++it;
  BOOST_TEST(it->eventID == EventID::invalidEvent(RunID(300)));
  BOOST_TEST(it->entry == 3);
  BOOST_TEST(it->getEntryType() == art::FileIndex::kRun);

  ++it;
  ++it;
  BOOST_TEST(it == fileIndex.end());

  BOOST_TEST(fileIndex.allEventsInEntryOrder() == true);
  BOOST_TEST(fileIndex.allEventsInEntryOrder() == true);
}

BOOST_AUTO_TEST_CASE(eventSortAndSearchTest)
{
  // Note this contains some illegal duplicates. There is nothing to
  // prevent these from occurring so we handle this using a
  // stable_sort for now ... They will not bother the FileIndex.

  art::FileIndex fileIndex;
  fileIndex.addEntry(EventID(3, 3, 2), 5);
  fileIndex.addEntry(EventID(3, 3, 1), 4);
  fileIndex.addEntry(EventID(3, 3, 3), 3);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(3, 3)), 6);
  fileIndex.addEntry(EventID::invalidEvent(RunID(3)), 7);
  fileIndex.addEntry(EventID::invalidEvent(RunID(1)), 8);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(3, 1)), 9);
  fileIndex.addEntry(EventID::invalidEvent(RunID(3)), 1);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(3, 3)), 1);
  fileIndex.addEntry(EventID(3, 3, 1), 1);
  fileIndex.addEntry(EventID(1, 2, 2), 2);
  fileIndex.addEntry(EventID(1, 2, 4), 1);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 2);
  fileIndex.addEntry(EventID(1, 2, 1), 2);
  fileIndex.addEntry(EventID::invalidEvent(RunID(6)), 9);
  fileIndex.addEntry(EventID::invalidEvent(RunID(9)), 10);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(6, 1)), 10);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(6, 3)), 11);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(6, 4)), 12);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(6, 5)), 13);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(6, 8)), 14);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(6, 9)), 15);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(9, 1)), 16);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(9, 2)), 17);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(6, 0)), 9);
  fileIndex.addEntry(EventID(6, 0, 4), 12);
  fileIndex.addEntry(EventID(6, 1, 5), 13);
  fileIndex.addEntry(EventID(6, 1, 6), 14);
  fileIndex.addEntry(EventID(6, 1, 7), 15);
  fileIndex.addEntry(EventID(6, 3, 9), 16);
  fileIndex.addEntry(EventID(6, 5, 13), 17);
  fileIndex.addEntry(EventID(6, 5, 14), 18);
  fileIndex.addEntry(EventID(6, 8, 15), 19);
  fileIndex.addEntry(EventID(6, 9, 5), 20);
  fileIndex.addEntry(EventID(6, 9, 25), 21);
  fileIndex.addEntry(EventID(6, 9, 25), 27);
  fileIndex.addEntry(EventID(9, 1, 5), 22);
  fileIndex.addEntry(EventID(9, 2, 5), 23);
  fileIndex.sortBy_Run_SubRun_Event();

  art::FileIndex shouldBe;
  shouldBe.addEntry(EventID::invalidEvent(RunID(1)), 8);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 2);
  shouldBe.addEntry(EventID(1, 2, 1), 2);
  shouldBe.addEntry(EventID(1, 2, 2), 2);
  shouldBe.addEntry(EventID(1, 2, 4), 1);
  shouldBe.addEntry(EventID::invalidEvent(RunID(3)), 7);
  shouldBe.addEntry(EventID::invalidEvent(RunID(3)), 1);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(3, 1)), 9);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(3, 3)), 6);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(3, 3)), 1);
  shouldBe.addEntry(EventID(3, 3, 1), 4);
  shouldBe.addEntry(EventID(3, 3, 1), 1);
  shouldBe.addEntry(EventID(3, 3, 2), 5);
  shouldBe.addEntry(EventID(3, 3, 3), 3);
  shouldBe.addEntry(EventID::invalidEvent(RunID(6)), 9);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(6, 0)), 9);
  shouldBe.addEntry(EventID(6, 0, 4), 12);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(6, 1)), 10);
  shouldBe.addEntry(EventID(6, 1, 5), 13);
  shouldBe.addEntry(EventID(6, 1, 6), 14);
  shouldBe.addEntry(EventID(6, 1, 7), 15);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(6, 3)), 11);
  shouldBe.addEntry(EventID(6, 3, 9), 16);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(6, 4)), 12);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(6, 5)), 13);
  shouldBe.addEntry(EventID(6, 5, 13), 17);
  shouldBe.addEntry(EventID(6, 5, 14), 18);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(6, 8)), 14);
  shouldBe.addEntry(EventID(6, 8, 15), 19);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(6, 9)), 15);
  shouldBe.addEntry(EventID(6, 9, 5), 20);
  shouldBe.addEntry(EventID(6, 9, 25), 21);
  shouldBe.addEntry(EventID(6, 9, 25), 27);
  shouldBe.addEntry(EventID::invalidEvent(RunID(9)), 10);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(9, 1)), 16);
  shouldBe.addEntry(EventID(9, 1, 5), 22);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(9, 2)), 17);
  shouldBe.addEntry(EventID(9, 2, 5), 23);

  BOOST_TEST(fileIndex == shouldBe);

  BOOST_TEST(fileIndex.allEventsInEntryOrder() == false);
  BOOST_TEST(fileIndex.allEventsInEntryOrder() == false);

  auto it = fileIndex.findPosition(EventID(3, 3, 2));
  BOOST_TEST((it - fileIndex.begin()) == 12);
  BOOST_TEST(it->eventID == EventID(3, 3, 2));
  BOOST_TEST(it->entry == 5);

  it = fileIndex.findPosition(EventID(3, 3, 7));
  BOOST_TEST((it - fileIndex.begin()) == 14);

  it = fileIndex.findPosition(EventID(1, 2, 3));
  BOOST_TEST((it - fileIndex.begin()) == 4);

  it = fileIndex.findPosition(EventID::invalidEvent(SubRunID(3, 3)));
  BOOST_TEST((it - fileIndex.begin()) == 8);

  it = fileIndex.findPosition(EventID::invalidEvent(SubRunID(1, 1)));
  BOOST_TEST((it - fileIndex.begin()) == 1);

  it = fileIndex.findPosition(EventID::invalidEvent(RunID(1)));
  BOOST_TEST((it - fileIndex.begin()) == 0);

  it = fileIndex.findPosition(EventID::invalidEvent(RunID(2)));
  BOOST_TEST((it - fileIndex.begin()) == 5);

  it = fileIndex.findPosition(EventID(2, 0, 1));
  BOOST_TEST((it - fileIndex.begin()) == 5);

  it = fileIndex.findPosition(EventID(1, 2, 3));
  BOOST_TEST((it - fileIndex.begin()) == 4);

  it = fileIndex.findPosition(EventID(3, 0, 3));
  BOOST_TEST((it - fileIndex.begin()) == 7);

  it = fileIndex.findPosition(EventID(3, 3, 2), true);
  BOOST_TEST((it - fileIndex.begin()) == 12);
  BOOST_TEST(fileIndex.contains(EventID(3, 3, 2), true));

  it = fileIndex.findPosition(EventID(1, 2, 3), true);
  BOOST_TEST(it == fileIndex.end());
  BOOST_TEST(!fileIndex.contains(EventID(1, 2, 3), true));

  it = fileIndex.findPosition(EventID(1, 2, 3), false);
  BOOST_TEST((it - fileIndex.begin()) == 4);
  BOOST_TEST(fileIndex.contains(EventID(1, 2, 3), false));

  it = fileIndex.findPosition(EventID(3, 0, 1), true);
  BOOST_TEST(it == fileIndex.end());

  it = fileIndex.findPosition(SubRunID(3, 1), true);
  BOOST_TEST((it - fileIndex.begin()) == 7);

  it = fileIndex.findPosition(SubRunID(3, 2), true);
  BOOST_TEST(it == fileIndex.end());

  it = fileIndex.findPosition(SubRunID(3, 1), false);
  BOOST_TEST((it - fileIndex.begin()) == 7);

  it = fileIndex.findPosition(SubRunID(3, 2), false);
  BOOST_TEST((it - fileIndex.begin()) == 8);

  BOOST_TEST(fileIndex.contains(SubRunID(3, 3), true));
  BOOST_TEST(!fileIndex.contains(SubRunID(2, 3), true));

  it = fileIndex.findPosition(RunID(3), true);
  BOOST_TEST((it - fileIndex.begin()) == 5);

  it = fileIndex.findPosition(RunID(2), true);
  BOOST_TEST(it == fileIndex.end());

  it = fileIndex.findPosition(RunID(2), false);
  BOOST_TEST((it - fileIndex.begin()) == 5);

  BOOST_TEST(fileIndex.contains(RunID(3), true));
  BOOST_TEST(!fileIndex.contains(RunID(2), true));

  it = fileIndex.findSubRunOrRunPosition(SubRunID(1, 2));
  BOOST_TEST((it - fileIndex.begin()) == 1);

  it = fileIndex.findSubRunOrRunPosition(SubRunID::invalidSubRun(RunID(3)));
  BOOST_TEST((it - fileIndex.begin()) == 5);

  it = fileIndex.findSubRunOrRunPosition(SubRunID(3, 4));
  BOOST_TEST((it - fileIndex.begin()) == 14);

  it = fileIndex.findSubRunOrRunPosition(SubRunID(3, 2));
  BOOST_TEST((it - fileIndex.begin()) == 8);

  it = fileIndex.findSubRunOrRunPosition(SubRunID(2, 1));
  BOOST_TEST((it - fileIndex.begin()) == 5);

  // Search for event without using subrun number.
  it =
    fileIndex.findPosition(EventID(SubRunID::invalidSubRun(RunID(1)), 4), true);
  BOOST_TEST((it - fileIndex.begin()) == 4);

  it =
    fileIndex.findPosition(EventID(SubRunID::invalidSubRun(RunID(1)), 5), true);
  BOOST_TEST(it == fileIndex.end());

  it = fileIndex.findPosition(EventID(SubRunID::invalidSubRun(RunID(1)), 5),
                              false);
  BOOST_TEST((it - fileIndex.begin()) == 10);

  it = fileIndex.findPosition(EventID(SubRunID::invalidSubRun(RunID(6)), 4),
                              false);
  BOOST_TEST((it - fileIndex.begin()) == 16);

  it = fileIndex.findPosition(EventID(SubRunID::invalidSubRun(RunID(6)), 8),
                              false);
  BOOST_TEST((it - fileIndex.begin()) == 22);

  it = fileIndex.findPosition(EventID(SubRunID::invalidSubRun(RunID(6)), 25),
                              false);
  BOOST_TEST((it - fileIndex.begin()) == 31);
}

BOOST_AUTO_TEST_CASE(eventEntrySortAndSearchTest)
{

  // Note this contains some illegal duplicates
  // For the moment there is nothing to prevent
  // these from occurring so we handle this using
  // a stable_sort for now ... They will not bother
  // the FileIndex.

  art::FileIndex fileIndex;
  fileIndex.addEntry(EventID(3, 3, 2), 5);
  fileIndex.addEntry(EventID(3, 3, 1), 4);
  fileIndex.addEntry(EventID(3, 3, 3), 3);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(3, 3)), 6);
  fileIndex.addEntry(EventID::invalidEvent(RunID(3)), 7);
  fileIndex.addEntry(EventID::invalidEvent(RunID(1)), 8);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(3, 1)), 9);
  fileIndex.addEntry(EventID::invalidEvent(RunID(3)), 1);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(3, 3)), 1);
  fileIndex.addEntry(EventID(3, 3, 1), 1);
  fileIndex.addEntry(EventID(1, 2, 2), 2);
  fileIndex.addEntry(EventID(1, 2, 4), 1);
  fileIndex.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 2);
  fileIndex.addEntry(EventID(1, 2, 1), 2);

  fileIndex.sortBy_Run_SubRun_EventEntry();

  art::FileIndex shouldBe;
  shouldBe.addEntry(EventID::invalidEvent(RunID(1)), 8);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 2);
  shouldBe.addEntry(EventID(1, 2, 4), 1);
  shouldBe.addEntry(EventID(1, 2, 2), 2);
  shouldBe.addEntry(EventID(1, 2, 1), 2);
  shouldBe.addEntry(EventID::invalidEvent(RunID(3)), 7);
  shouldBe.addEntry(EventID::invalidEvent(RunID(3)), 1);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(3, 1)), 9);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(3, 3)), 6);
  shouldBe.addEntry(EventID::invalidEvent(SubRunID(3, 3)), 1);
  shouldBe.addEntry(EventID(3, 3, 1), 1);
  shouldBe.addEntry(EventID(3, 3, 3), 3);
  shouldBe.addEntry(EventID(3, 3, 1), 4);
  shouldBe.addEntry(EventID(3, 3, 2), 5);

  BOOST_TEST(fileIndex == shouldBe);

  auto it = fileIndex.findPosition(SubRunID(3, 1), true);
  BOOST_TEST((it - fileIndex.begin()) == 7);

  it = fileIndex.findPosition(SubRunID(3, 2), true);
  BOOST_TEST(it == fileIndex.end());

  it = fileIndex.findPosition(SubRunID(3, 1), false);
  BOOST_TEST((it - fileIndex.begin()) == 7);

  it = fileIndex.findPosition(SubRunID(3, 2), false);
  BOOST_TEST((it - fileIndex.begin()) == 8);

  it = fileIndex.findPosition(RunID(3), true);
  BOOST_TEST((it - fileIndex.begin()) == 5);

  it = fileIndex.findPosition(RunID(2), true);
  BOOST_TEST(it == fileIndex.end());

  it = fileIndex.findPosition(RunID(2), false);
  BOOST_TEST((it - fileIndex.begin()) == 5);

  it = fileIndex.findSubRunOrRunPosition(SubRunID(1, 2));
  BOOST_TEST((it - fileIndex.begin()) == 1);

  it = fileIndex.findSubRunOrRunPosition(SubRunID::invalidSubRun(RunID(3)));
  BOOST_TEST((it - fileIndex.begin()) == 5);

  it = fileIndex.findSubRunOrRunPosition(SubRunID(3, 4));
  BOOST_TEST(it == fileIndex.end());

  it = fileIndex.findSubRunOrRunPosition(SubRunID(3, 2));
  BOOST_TEST((it - fileIndex.begin()) == 8);

  it = fileIndex.findSubRunOrRunPosition(SubRunID(2, 1));
  BOOST_TEST((it - fileIndex.begin()) == 5);
}

BOOST_AUTO_TEST_CASE(eventsUniqueAndOrderedTest)
{
  // Test the different cases

  // Nothing in the FileIndex
  art::FileIndex fileIndex;
  BOOST_TEST(fileIndex.eventsUniqueAndOrdered());

  // No events
  art::FileIndex fileIndex1;
  fileIndex1.addEntry(EventID::invalidEvent(RunID(1)), 1);
  fileIndex1.addEntry(EventID::invalidEvent(SubRunID(1, 1)), 1);
  BOOST_TEST(fileIndex1.eventsUniqueAndOrdered());

  // One event and nothing after it
  art::FileIndex fileIndex2;
  fileIndex2.addEntry(EventID::invalidEvent(RunID(1)), 1);
  fileIndex2.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 1);
  fileIndex2.addEntry(EventID(1, 2, 1), 1);
  BOOST_TEST(fileIndex2.eventsUniqueAndOrdered());

  // One event with a run after it
  art::FileIndex fileIndex3;
  fileIndex3.addEntry(EventID::invalidEvent(RunID(1)), 1);
  fileIndex3.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 1);
  fileIndex3.addEntry(EventID(1, 2, 1), 1);
  fileIndex3.addEntry(EventID::invalidEvent(RunID(2)), 2);
  BOOST_TEST(fileIndex3.eventsUniqueAndOrdered());

  // Two events
  art::FileIndex fileIndex4;
  fileIndex4.addEntry(EventID::invalidEvent(RunID(1)), 1);
  fileIndex4.addEntry(EventID::invalidEvent(SubRunID(1, 1)), 1);
  fileIndex4.addEntry(EventID(1, 1, 1), 1);
  fileIndex4.addEntry(EventID::invalidEvent(RunID(2)), 2);
  fileIndex4.addEntry(EventID::invalidEvent(SubRunID(2, 1)), 2);
  fileIndex4.addEntry(EventID(2, 1, 1), 2);
  BOOST_TEST(fileIndex4.eventsUniqueAndOrdered());

  // Two events, same run and event number
  art::FileIndex fileIndex5;
  fileIndex5.addEntry(EventID::invalidEvent(RunID(1)), 1);
  fileIndex5.addEntry(EventID::invalidEvent(SubRunID(1, 1)), 1);
  fileIndex5.addEntry(EventID(1, 1, 1), 1);
  fileIndex5.addEntry(EventID::invalidEvent(RunID(1)), 2);
  fileIndex5.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 2);
  fileIndex5.addEntry(EventID(1, 2, 1), 2);
  BOOST_TEST(fileIndex5.eventsUniqueAndOrdered());

  // Not ordered by run
  art::FileIndex fileIndex6;
  fileIndex6.addEntry(EventID::invalidEvent(RunID(1)), 1);
  fileIndex6.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 1);
  fileIndex6.addEntry(EventID(1, 2, 1), 1);
  fileIndex6.addEntry(EventID::invalidEvent(RunID(2)), 2);
  fileIndex6.addEntry(EventID::invalidEvent(SubRunID(2, 1)), 2);
  fileIndex6.addEntry(EventID(2, 1, 1), 2);
  fileIndex6.addEntry(EventID::invalidEvent(RunID(1)), 3);
  fileIndex6.addEntry(EventID::invalidEvent(SubRunID(1, 3)), 3);
  fileIndex6.addEntry(EventID(1, 3, 1), 3);
  BOOST_TEST(!fileIndex6.eventsUniqueAndOrdered());

  // Not ordered by event
  art::FileIndex fileIndex7;
  fileIndex7.addEntry(EventID::invalidEvent(RunID(1)), 1);
  fileIndex7.addEntry(EventID::invalidEvent(SubRunID(1, 2)), 1);
  fileIndex7.addEntry(EventID(1, 2, 1), 1);
  fileIndex7.addEntry(EventID::invalidEvent(RunID(2)), 2);
  fileIndex7.addEntry(EventID::invalidEvent(SubRunID(2, 1)), 2);
  fileIndex7.addEntry(EventID(2, 1, 2), 2);
  fileIndex7.addEntry(EventID::invalidEvent(SubRunID(2, 3)), 3);
  fileIndex7.addEntry(EventID(2, 3, 1), 3);
  BOOST_TEST(fileIndex7.eventsUniqueAndOrdered());

  // OK, ordered by event and unique
  art::FileIndex fileIndex8;
  fileIndex8.addEntry(EventID::invalidEvent(RunID(1)), 1);
  fileIndex8.addEntry(EventID::invalidEvent(SubRunID(1, 1)), 1);
  fileIndex8.addEntry(EventID(1, 1, 1), 1);
  fileIndex8.addEntry(EventID(1, 1, 2), 1);
  fileIndex8.addEntry(EventID(1, 1, 3), 1);
  fileIndex8.addEntry(EventID(1, 1, 4), 1);
  fileIndex8.addEntry(EventID::invalidEvent(RunID(2)), 2);
  fileIndex8.addEntry(EventID::invalidEvent(SubRunID(2, 1)), 2);
  fileIndex8.addEntry(EventID(2, 1, 1), 2);
  fileIndex8.addEntry(EventID::invalidEvent(SubRunID(2, 3)), 3);
  fileIndex8.addEntry(EventID(2, 3, 2), 3);
  fileIndex8.addEntry(EventID(2, 3, 3), 3);
  fileIndex8.addEntry(EventID(2, 3, 4), 3);
  fileIndex8.addEntry(EventID(2, 3, 5), 3);
  fileIndex8.addEntry(EventID(2, 3, 6), 3);
  BOOST_TEST(fileIndex8.eventsUniqueAndOrdered());
}

BOOST_AUTO_TEST_SUITE_END()
