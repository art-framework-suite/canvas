#ifndef canvas_Utilities_InputTag_h
#define canvas_Utilities_InputTag_h
// vim: set sw=2 expandtab :

#include <iosfwd>
#include <string>

namespace boost {

class any;

} // namespace boost

namespace art {

class InputTag {

public: // MEMBER FUNCTIONS -- Special Member Functions

  ~InputTag();

  InputTag();

  InputTag(std::string const& label, std::string const& instance, std::string const& processName = std::string());

  InputTag(char const* label, char const* instance, char const* processName = "");

  InputTag(std::string const&);

  InputTag(char const*);

  InputTag(InputTag const&);

  InputTag(InputTag&&);

  InputTag&
  operator=(InputTag const&);

  InputTag&
  operator=(InputTag&&);

public: // MEMBER FUNCTIONS -- API for the user

  bool
  operator==(InputTag const&) const;

  std::string const&
  label() const;

  std::string const&
  instance() const;

  std::string const&
  process() const;

  std::string
  encode() const;

private: // MEMBER DATA

  std::string
  label_{};

  std::string
  instance_{};

  std::string
  process_{};

};

bool
operator!=(InputTag const&, InputTag const&);

void
decode(boost::any const&, InputTag&);

std::ostream&
operator<<(std::ostream&, InputTag const&);

} // namespace art

#endif /* canvas_Utilities_InputTag_h */

// Local Variables:
// mode: c++
// End:
