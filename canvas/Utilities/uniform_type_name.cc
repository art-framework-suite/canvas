#include "canvas/Utilities/uniform_type_name.h"

#include "cetlib/replace_all.h"

#include <regex>
#include <string>

namespace {
  /// \fn reformat.
  ///
  /// \brief Reformat the input string according to the provided regex
  /// and format string.
  ///
  /// \param[in,out] input The string to be manipulated.
  ///
  /// \param[in] exp The regular expression to be (repeatedly) applied.
  ///
  /// \param[format] format The specified format string for the
  /// replacement.
  void
  reformat(std::string & input,
           std::regex const & exp,
           std::string const & format)
  {
    while (1) {
      std::string const formatted = std::regex_replace(input, exp, format);
      if (formatted == input) {
        break;
      } else {
        input = formatted;
      }
    }
  }

  /// \fn removeParameter.
  ///
  /// \brief Remove the specified paramter from the input string.
  ///
  /// \param[in,out] name The string from which the parameter should be
  /// removed.
  ///
  /// \param[in] toRemove The parameter to remove from the
  /// string. Include leading comma if appropriate and trailing open
  /// angled bracket for template instantiations.
  void
  removeParameter(std::string & name, std::string const & toRemove)
  {
    auto const asize = toRemove.size();
    auto const initDepth = (toRemove.back() == '<') ? 1 : 0;
    if (asize == 0ul) {
      return;
    }
    char const * const delimiters = "<>";
    auto index = std::string::npos;
    while ((index = name.find(toRemove)) != std::string::npos) {
      int depth = initDepth;
      auto inx = index + asize;
      while ((inx = name.find_first_of(delimiters, inx)) != std::string::npos) {
        if (name[inx] == '<') {
          ++depth;
        } else {
          if (--depth == 0) {
            name.erase(index, inx + 1 - index);
            if (name[index] == ' ' &&
                (index == 0 || name[index - 1] != '>')) {
              name.erase(index, 1);
            }
            break;
          }
        }
        ++inx;
      }
    }
  }

  /// \fn constBeforeIdentifier.
  ///
  /// \brief Change all occurrences of "<type> const" in name to "const
  /// <type>."
  ///
  /// \param[in,out] name The string to manipulate.
  void
  constBeforeIdentifier(std::string & name)
  {
    std::string const toBeMoved(" const");
    auto const asize = toBeMoved.size();
    auto index = std::string::npos;
    while ((index = name.find(toBeMoved)) != std::string::npos) {
      name.erase(index, asize);
      int depth = 0;
      for (std::string::size_type inx = index - 1; inx > 0; --inx) {
        char const c = name[inx];
        if (c == '>') {
          ++depth;
        } else if (depth > 0) {
          if (c == '<') {
            --depth;
          }
        } else if (c == '<' || c == ',') {
          name.insert(inx + 1, "const ");
          break;
        }
      }
    }
  }
}

std::string
art::uniform_type_name(std::string name) {
  using namespace std::string_literals;
  // We must use the same conventions previously used by Reflex.
  // The order is important.

  // We must remove __cxx11:: from all type names. This should not have
  // implications for I/O because ROOT stores the data that STL objects
  // represent rather than doing a "dumb serialization" of the class.
  cet::replace_all(name, "__cxx11::"s, ""s);

  // No space after comma.
  cet::replace_all(name, ", "s, ","s);
  // No space before opening square bracket.
  cet::replace_all(name, " ["s, "["s);
  // Strip default allocator.
  removeParameter(name, ",std::allocator<"s);
  // Strip default comparator.
  removeParameter(name, ",std::less<"s);
  // Strip char traits.
  removeParameter(name, ",std::char_traits<"s);
  // std::basic_string<char> -> std::string
  {
    static std::regex const bs_regex("std::basic_string<char>\\s*"s);
    reformat(name, bs_regex, "std::string"s);
  }
  // Put const qualifier before identifier.
  constBeforeIdentifier(name);
  // No consecutive '>'.
  //
  // FIXME: The first time we see a type with e.g. operator>> as a
  // template argument, we could have a problem.
  cet::replace_all(name, ">>"s, "> >"s);
  // No u or l qualifiers for integers.
  {
    static std::regex const ul_regex("(.*[<,][0-9]+)[ul]l*([,>].*)"s);
    reformat(name, ul_regex, "$1$2"s);
  }
  // For ROOT 6 and beyond.
  cet::replace_all(name, "unsigned long long"s, "ULong64_t"s);
  cet::replace_all(name, "long long"s, "Long64_t"s);
  // Done.
  return name;
}
