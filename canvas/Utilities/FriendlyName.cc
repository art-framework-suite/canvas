#include "canvas/Utilities/FriendlyName.h"
// vim: set sw=2 expandtab :

#include "canvas/Utilities/Exception.h"

#include <map>
#include <mutex>
#include <regex>
#include <string>

// NOTE: This should probably be rewritten so that we break the class
// name into a tree where the template arguments are the node.  On the
// way down the tree we look for '<' or ',' and on the way up (caused by
// finding a '>') we can apply the transformation to the output string
// based on the class name for the templated class.  Up front we'd
// register a class name to a transformation function (which would
// probably take a std::vector<std::string> which holds the results of
// the node transformations).

namespace {

  std::regex const reAllSpaces{" +"};
  std::regex const reAssns{"art::Assns"};
  std::regex const reBeginSpace{"^ +"};
  std::regex const reComma{","};
  std::regex const reEndSpace{" +$"};
  std::regex const reParens{"(\\(|\\))"};
  std::regex const reFirstTwoArgs{"^([^,]+),([^,]+)"};
  std::regex const reLong{"long "};
  std::regex const reLongLong{"Long64_t"};
  std::regex const reMapVector{"cet::map_vector"};
  std::regex const reMapVectorKey{"cet::map_vector_key"};
  std::regex const reString{"(?:std::basic_string<char>|std::string)"};
  std::regex const reTemplateArgs{"([^<]*)<(.*)>$"};
  std::regex const reTemplateClass{"([^<>,]+<[^<>]*>)"};
  std::regex const reULongLong{"ULong64_t"};
  std::regex const reUnsigned{"unsigned "};
  std::regex const reVector{"std::vector"};
  std::regex const reWrapper{"art::Wrapper<(.*)>"};

  std::string const emptyString{};

  std::string
  removeExtraSpaces(std::string const& in)
  {
    return std::regex_replace(std::regex_replace(in, reBeginSpace, emptyString),
                              reEndSpace,
                              emptyString);
  }

  std::string
  removeAllSpaces(std::string const& in)
  {
    return std::regex_replace(in, reAllSpaces, emptyString);
  }

  std::string
  escapeParens(std::string const& in)
  {
    return std::regex_replace(in, reParens, "\\$1");
  }

  std::string
  standardRenames(std::string const& in)
  {
    std::string name{std::regex_replace(in, reWrapper, "$1")};
    name = std::regex_replace(name, reString, "String");
    name = std::regex_replace(name, reUnsigned, "u");
    name = std::regex_replace(name, reLong, "l");
    name = std::regex_replace(name, reULongLong, "ull");
    name = std::regex_replace(name, reLongLong, "ll");
    name = std::regex_replace(name, reVector, "s");
    name = std::regex_replace(name, reMapVectorKey, "mvk");
    name = std::regex_replace(name, reMapVector, "mv");
    return name;
  }

  // Declaration required here because handleTemplateArguments and
  // subFriendlyName call each other.
  std::string handleTemplateArguments(std::string const&, std::string const&);
  std::string
  subFriendlyName(std::string const& iFullName)
  {
    std::string result{removeExtraSpaces(iFullName)};
    std::smatch theMatch;
    if (std::regex_match(result, theMatch, reTemplateArgs)) {
      std::string const cMatch{theMatch.str(1)};
      std::string const aMatch{theMatch.str(2)};
      std::string const theSub{handleTemplateArguments(cMatch, aMatch)};
      // If a type (e.g.) A was declared in an anonymous namespace,
      // the demangled typename can be (anonymous namespace)::A.  The
      // parentheses must be escaped so that they do not interfere
      // with the regex library.
      std::regex const eMatch{std::string{"^"} + escapeParens(cMatch) + '<' +
                              escapeParens(aMatch) + '>'};
      result = std::regex_replace(result, eMatch, theSub + cMatch);
    }
    return result;
  }

  void
  maybeSwapFirstTwoArgs(std::string& result)
  {
    std::smatch theMatch;
    if (std::regex_search(result, theMatch, reFirstTwoArgs) &&
        (theMatch.str(1) > theMatch.str(2))) {
      result = std::regex_replace(result, reFirstTwoArgs, "$2,$1");
    }
  }

  std::string
  handleTemplateArguments(std::string const& cName, std::string const& tArgs)
  {
    std::string result{removeExtraSpaces(tArgs)};
    bool shouldStop{false};
    while (!shouldStop) {
      if (std::string::npos != result.find_first_of("<")) {
        std::smatch theMatch;
        if (std::regex_search(result, theMatch, reTemplateClass)) {
          std::string const templateClass{theMatch.str(1)};
          std::string const friendlierName{
            removeAllSpaces(subFriendlyName(templateClass))};
          result = std::regex_replace(
            result, std::regex(templateClass), friendlierName);
        } else {
          throw art::Exception(art::errors::LogicError)
            << "No template match for \"" << result << '"';
        }
      } else {
        shouldStop = true;
      }
    }
    if (std::regex_match(cName, reAssns)) {
      maybeSwapFirstTwoArgs(result);
    }
    result = std::regex_replace(result, reComma, emptyString);
    return result;
  }

} // unnamed namespace

std::string
art::friendlyname::friendlyName(std::string const& iFullName)
{
  static std::recursive_mutex s_mutex;
  static std::map<std::string, std::string> s_nameMap;
  std::lock_guard sentry{s_mutex};
  auto entry = s_nameMap.find(iFullName);
  if (entry == s_nameMap.end()) {
    entry =
      s_nameMap.emplace(iFullName, subFriendlyName(standardRenames(iFullName)))
        .first;
  }
  return entry->second;
}
