#ifndef canvas_Persistency_Provenance_Transient_h
#define canvas_Persistency_Provenance_Transient_h
// vim: set sw=2 expandtab :

//
//  We give all instantiations of this template a 'transient="true"'
//  attribute for the value_ data member to tell ROOT to never write
//  it to disk.  We also set all instantiations (by hand!) to have a
//  custom streamer (TransientStreamer) that makes sure if the ROOT
//  I/O buffer is reused for reading that the data member _value is
//  reinitialized with a default-constructed T.
//


namespace art {

template<typename T>
class Transient {

public: // TYPES

  typedef T value_type;

public: // MEMBER FUNCTIONS -- Special Member Functions

  ~Transient() = default;

  Transient(T value = T())
    : value_(value)
  {
  }

  Transient(Transient const&) = default;

  Transient(Transient&&) = default;

  Transient&
  operator=(Transient const&) = default;

  Transient&
  operator=(Transient&&) = default;

public: // MEMBER FUNCTIONS -- API for the user

  Transient&
  operator=(T rh)
  {
    value_ = rh;
    return *this;
  }

  operator T() const
  {
    return value_;
  }

  T const&
  get() const noexcept
  {
    return value_;
  }

  T&
  get() noexcept
  {
    return value_;
  }

private: // MEMBER DATA

  T
  value_;

};

} // namespace art

#endif /* canvas_Persistency_Provenance_Transient_h */

// Local Variables:
// mode: c++
// End:
