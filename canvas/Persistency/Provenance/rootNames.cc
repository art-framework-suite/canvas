#include "canvas/Persistency/Provenance/rootNames.h"

#include <array>

namespace {
  using namespace std::string_literals;

  std::string const parentageTree = "Parentage";
  std::string const parentageIDBranch = "Hash";
  std::string const parentageBranch = "Description";

  std::string const metaDataTree = "MetaData";
  std::string const fileIndexTree = "FileIndex";
  std::string const eventHistory = "EventHistory";

  std::array<std::string const, art::NumBranchTypes> dataTrees = {
    {"Events"s, "SubRuns"s, "Runs"s, "Results"s}};
  std::string const eventMeta = "EventMetaData";
}

// Parentage tree.
std::string const&
art::rootNames::parentageTreeName()
{
  return parentageTree;
}

std::string const&
art::rootNames::parentageIDBranchName()
{
  return parentageIDBranch;
}

std::string const&
art::rootNames::parentageBranchName()
{
  return parentageBranch;
}

// MetaData Tree (1 entry per file)
std::string const&
art::rootNames::metaDataTreeName()
{
  return metaDataTree;
}

// FileIndex Tree
std::string const&
art::rootNames::fileIndexTreeName()
{
  return fileIndexTree;
}

// EventHistory Tree
std::string const&
art::rootNames::eventHistoryTreeName()
{
  return eventHistory;
}

// Branch on Event History Tree
std::string const&
art::rootNames::eventHistoryBranchName()
{
  return eventHistory;
}

std::string const&
art::rootNames::eventTreeName()
{
  return dataTrees[InEvent];
}

std::string const&
art::rootNames::dataTreeName(BranchType bt)
{
  return dataTrees[bt];
}

std::string const&
art::rootNames::eventMetaDataTreeName()
{
  return eventMeta;
}
