#ifndef canvas_Utilities_EventIDMatcher_h
#define canvas_Utilities_EventIDMatcher_h
// vim: set sw=2 :

#include "canvas/Persistency/Provenance/ProvenanceFwd.h"

#include <string>
#include <vector>

namespace art {

  class EventIDMatcher {
  public:
    explicit EventIDMatcher(std::string const& pattern);
    explicit EventIDMatcher(std::vector<std::string> const& patterns);

    bool operator()(EventID const&) const;
    bool match(EventID const&) const;

  private:
    void parse_pattern();

    struct PatternRangeElement {
      unsigned low;
      unsigned high;
      bool wildcard;
    };

    std::vector<std::string> pattern_;
    std::vector<std::vector<std::vector<PatternRangeElement>>> parsed_patterns_;
  };

} // namespace art

#endif /* canvas_Utilities_EventIDMatcher_h */

// Local Variables:
// mode: c++
// End:
