// vim: set sw=2 :
#include "canvas/Utilities/EventIDMatcher.h"

#include "canvas/Persistency/Provenance/EventID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Utilities/Exception.h"

#include <cstdio>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace art {

  EventIDMatcher::EventIDMatcher(std::string const& pattern)
    : pattern_(), parsed_patterns_()
  {
    if (pattern.empty()) {
      throw art::Exception(errors::LogicError)
        << "EventIDMatcher given an empty pattern!";
    }
    pattern_.push_back(pattern);
    parsed_patterns_.resize(1);
    parse_pattern();
  }

  EventIDMatcher::EventIDMatcher(std::vector<std::string> const& patterns)
    : pattern_(), parsed_patterns_()
  {
    if (patterns.size() == 0) {
      throw art::Exception(errors::LogicError)
        << "EventIDMatcher given an empty list of patterns!";
    }
    pattern_.reserve(patterns.size());
    parsed_patterns_.resize(patterns.size());
    for (auto const& val : patterns) {
      if (val.empty()) {
        throw art::Exception(errors::LogicError)
          << "EventIDMatcher given an empty pattern!";
      }
      pattern_.push_back(val);
    }
    parse_pattern();
  }

  void
  EventIDMatcher::parse_pattern()
  {
    regex pat( // matches ('*' /*wildcard*/ | digits /*single*/ | digits -
               // digits /*range*/)(',' /*list*/ | ':' /*part*/ | eol)
      "("      // 1
      "[[:blank:]]*"
      "("         // 2
      "([*])|"    // 3
      "([0-9]+)|" // 4
      "([0-9]+)"  // 5
      "[[:blank:]]*-[[:blank:]]*"
      "([0-9]+)" // 6
      ")"
      "[[:blank:]]*"
      "([,:]|$)" // 7
      ")");
    int patno = -1;
    for (auto const& given_pattern : pattern_) {
      ++patno;
      regex_iterator<string::const_iterator> I(
        given_pattern.cbegin(), given_pattern.cend(), pat);
      regex_iterator<string::const_iterator> E;
      auto prev_pos = 0L;
      auto prev_len = 0L;
      char prev_sep = '\0';
      parsed_patterns_[patno].resize(3);
      // Note: 0: run, 1: subrun, 2: event
      auto part_num = 0U;
      for (; I != E; ++I) {
        auto const& m = *I;
        char sep = '\0';
        if (m.str(7).size()) {
          sep = m.str(7)[0];
        }
        if (m.position() != (prev_pos + prev_len)) {
          // err, non-matching characters between
          ostringstream buf;
          buf << '\n';
          buf << "Illegal character in pattern near here:\n";
          buf << given_pattern;
          buf << '\n';
          for (auto i = 0L; i < (prev_pos + prev_len); ++i) {
            buf << ' ';
          }
          buf << "^\n";
          throw art::Exception(errors::LogicError) << buf.str();
        }
        if (m[3].matched) {
          // wildcard
          parsed_patterns_[patno][part_num].push_back({0U, 0U, true});
        } else if (m[4].matched) {
          // single num
          auto num = 0U;
          for (auto val : m.str(4)) {
            num = (num * 10U) + (val - '0');
          }
          parsed_patterns_[patno][part_num].push_back({num, num, false});
        } else {
          // range
          auto num_low = 0U;
          for (auto val : m.str(5)) {
            num_low = (num_low * 10U) + (val - '0');
          }
          auto num_high = 0U;
          for (auto val : m.str(6)) {
            num_high = (num_high * 10U) + (val - '0');
          }
          parsed_patterns_[patno][part_num].push_back(
            {num_low, num_high, false});
        }
        if (sep == ':') {
          if (part_num == 0U) {
            // at end of run part
          } else if (part_num == 1U) {
            // at end of subrun part
          } else if (part_num == 2U) {
            // error, event part ended with a ':'
            ostringstream buf;
            buf << '\n';
            buf
              << "Syntax error, event part of pattern ended with a ':' here:\n";
            buf << given_pattern;
            buf << '\n';
            for (auto i = 0L; i < m.position(7); ++i) {
              buf << ' ';
            }
            buf << "^\n";
            throw art::Exception(errors::LogicError) << buf.str();
          }
          ++part_num;
        } else if (sep == ',') {
          // range continues
        } else {
          // at end of event part, and end of string
        }
        prev_pos = m.position();
        prev_len = m.length();
        prev_sep = sep;
      }
      if (prev_sep != '\0') {
        // err, last match did not finish properly
        ostringstream buf;
        buf << '\n';
        printf("pep: Syntax error, near here:\n");
        buf << given_pattern;
        buf << '\n';
        for (auto i = 0L; i < (prev_pos + prev_len); ++i) {
          buf << ' ';
        }
        buf << "^\n";
        throw art::Exception(errors::LogicError) << buf.str();
      }
      if (static_cast<string::size_type>(prev_pos + prev_len) !=
          given_pattern.size()) {
        // err, did not match whole string
        ostringstream buf;
        buf << '\n';
        printf("pep: Syntax error, near here:\n");
        buf << given_pattern;
        buf << '\n';
        for (auto i = 0L; i < (prev_pos + prev_len); ++i) {
          buf << ' ';
        }
        buf << "^\n";
        throw art::Exception(errors::LogicError) << buf.str();
      }
    }
  }

  bool
  EventIDMatcher::operator()(EventID const& eid) const
  {
    return match(eid);
  }

  bool
  EventIDMatcher::match(EventID const& eid) const
  {
    if (!eid.isValid() || eid.isFlush()) {
      return false;
    }
    if (!eid.subRunID().isValid() || eid.subRunID().isFlush()) {
      return false;
    }
    if (!eid.runID().isValid() || eid.runID().isFlush()) {
      return false;
    }

    bool ret = false;
    for (auto const& parsed_pattern : parsed_patterns_) {
      for (auto i = 0U; i < 3; ++i) {
        for (auto const& val : parsed_pattern[i]) {
          if (val.wildcard) {
            // Wildcards always match
            ret = true;
            break;
          } else if (val.low == val.high) {
            // Single value match
            if (i == 0U) {
              // run
              if (eid.run() == val.low) {
                ret = true;
                break;
              }
            } else if (i == 1U) {
              // subrun
              if (eid.subRun() == val.low) {
                ret = true;
                break;
              }
            } else {
              // event
              if (eid.event() == val.low) {
                ret = true;
                break;
              }
            }
          } else {
            // Range match
            if (i == 0U) {
              // run
              if ((eid.run() >= val.low) && (eid.run() <= val.high)) {
                ret = true;
                break;
              }
            } else if (i == 1U) {
              // subrun
              if ((eid.subRun() >= val.low) && (eid.subRun() <= val.high)) {
                ret = true;
                break;
              }
            } else {
              // event
              if ((eid.event() >= val.low) && (eid.event() <= val.high)) {
                ret = true;
                break;
              }
            }
          }
        }
        if (!ret) {
          // nothing matched
          break;
        }
        if (i != 2U) {
          // Reset for next part.
          ret = false;
        }
      }
      if (ret) {
        // We matched, return immediately.
        return ret;
      }
    }
    // We never matched.
    return ret;
  }

} // namespace art
