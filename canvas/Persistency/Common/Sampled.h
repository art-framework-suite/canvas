#ifndef canvas_Persistency_Common_Sampled_h
#define canvas_Persistency_Common_Sampled_h

// ==============================================================================
// Sampled template
//
// The Sampled template associates dataset names to values of type T
// for that dataset.  It is intended to be used to gather different
// instances of a (sub)run data product that have been written to
// separate files.
//
// A Sampled<T> object is created by providing an input tag
// corresponding to the original product signature of the gathered
// products.  This input tag can be accessed by calling the
// Sampled<T>::originalInputTag() member function.  The input, along
// with the type T can be used to disambiguate between sampled
// products for a given run or subrun.  Since this object is intended
// to be retrieved within the context of a run or subrun, we do not
// additionally specify the BranchType.
//
// To access the stored values for products of the type 'int', module
// label 'm1', an empty instance name, and process name 'MakeInts',
// follow the pattern:
//
//   InputTag const tag_to_retrieve{"m1::SampledFromMakeInts"};
//   auto const& sampledInts = r.getValidHandle<Sampled<int>>(tag_to_retrieve);
//
//   for (auto const& [dataset, nums] : sampledInts) {
//     cet::for_all(nums, [](int const i) { ... });
//   }
//
// N.B. To access sampled products with the process name 'MakeInts',
//      the provided process name while retrieving the corresponding
//      sampled product is 'SampledFromMakeInts', where the
//      'SampledFrom' *must be prepended* to the original process
//      name.  The art::sampled_from("MakeInts") function can be used
//      for this purpose.
// ==============================================================================

#include "canvas/Persistency/Common/fwd.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/InputTag.h"
#include "cetlib_except/demangle.h"

#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace art {

  template <typename T>
  class Sampled {
    using container_t = std::map<std::string, std::vector<T>>;

  public:
    using const_iterator = typename container_t::const_iterator;

    Sampled() = default;
    explicit Sampled(InputTag const& tag) noexcept(false);

    InputTag const&
    originalInputTag() const
    {
      return tag_;
    }

    const_iterator
    begin() const
    {
      return products_.begin();
    }

    const_iterator
    end() const
    {
      return products_.end();
    }

    const_iterator
    cbegin() const
    {
      return products_.cbegin();
    }

    const_iterator
    cend() const
    {
      return products_.cend();
    }

    bool
    empty() const
    {
      return products_.empty();
    }

    void
    insert(std::string const& dataset, T&& value)
    {
      products_[dataset].push_back(std::forward<T>(value));
    }

    // MUST UPDATE WHEN CLASS IS CHANGED!
    static short
    Class_Version()
    {
      return 10;
    }

  private:
    InputTag tag_{};
    container_t products_{};
  };

  inline auto
  sampled_from(std::string process_name)
  {
    return process_name.insert(0, "SampledFrom");
  }

  // Implementation below
  template <typename T>
  Sampled<T>::Sampled(InputTag const& tag) noexcept(false) : tag_{tag}
  {
    // Due to Ptr reseeding issues, Assns or Ptr types (or
    // containers thereof) are not supported.  This checking cannot
    // be done at compile time because creating a Wrapper requires a
    // Sampled instantiation, even if that instantiation will never
    // be used.
    auto const type_name = cet::demangle_symbol(typeid(*this).name());
    if (type_name.find("art::Assns") != std::string::npos ||
        type_name.find("art::Ptr") != std::string::npos) {
      throw Exception{errors::LogicError}
        << "An attempt was made to create the type "
        << cet::demangle_symbol(typeid(T).name())
        << ".\n"
           "This is not allowed.  Please contact artists@fnal.gov for "
           "guidance.";
    }
  }
}

#endif /* canvas_Persistency_Common_Sampled_h */

// Local Variables:
// mode: c++
// End:
