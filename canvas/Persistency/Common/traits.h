#ifndef canvas_Persistency_Common_traits_h
#define canvas_Persistency_Common_traits_h

/*----------------------------------------------------------------------

  Definition of traits templates used in the EDM.

  ----------------------------------------------------------------------*/

#include "cetlib/detail/metaprogramming.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib/demangle.h"
#include "cetlib/detail/metaprogramming.h"
#include "cetlib/map_vector.h"

#include <deque>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

namespace art
{
  //------------------------------------------------------------
  //
  // The trait struct template key_traits<K> is used to carry
  // information relevant to the type K when used as a 'key' in
  // PtrVector and its related classes and templates.
  //
  // The general case works only for integral types K; for more
  // 'esoteric' types, one must introduce an explicit specialization.
  // That specialization must initialize the static data member
  // 'value'.

  template <class K>
  struct key_traits
  {
    using key_type = K;
    static constexpr key_type value {std::numeric_limits<typename key_traits<K>::key_type>::max()};
  };

  // Partial specialization for std::pair

  template <class U, class V>
  struct key_traits<std::pair<U,V>>
  {
    using key_type = std::pair<U,V>;
    static const key_type value;
  };

  template <class U, class V>
  typename key_traits<std::pair<U,V>>::key_type const
  key_traits<std::pair<U,V>>::value(key_traits<U>::value, key_traits<V>::value);

  // If we ever need to support instantiations of std::basic_string
  // other than std::string, this is the place to do it.

  // For value, we make a 1-character long string that contains an
  // unprintable character; we are hoping nobody ever uses such a
  // string as a legal key.
  template <>
  struct key_traits<std::string>
  {
    using key_type = std::string;
    static const key_type value;
  };

  //------------------------------------------------------------
  //
  // DoNotRecordParents is a base class. Derive your own (EDProduct)
  // class X from DoNotRecordParents when your class already keeps all
  // data that are relevant to parentage internally, and the
  // information kept by the event model would thus be redundant.
  //
  // DoNotRecordParents has no behavior; it is used at compile time to
  // influence the behavior of Event::put.
  //
  // Usage:
  //    class MyClass : public art::DoNotRecordParents { ... }
  struct DoNotRecordParents { };

  // Other is a base class. NEVER USE IT. It is for the
  // core of the event model only.
  struct Other { };

  //------------------------------------------------------------
  //
  // The trait struct template has_fillView<T> is used to
  // indicate whether or not the type T has a member function
  //
  //      void T::fillView(std::vector<void const*>&) const
  //
  // We assume the 'general case' for T is to not support fillView.
  // Classes which do support fillView must specialize this trait.
  //
  //------------------------------------------------------------

  // has_fillView
  template <typename T, typename = void>
  struct has_fillView : std::false_type {};

  template <typename T>
  struct has_fillView<T, cet::detail::enable_if_function_exists_t<void(T::*)(std::vector<void const*>&), &T::fillView>> : std::true_type {};

  template <typename T>
  struct CannotFillView {
    static void fill(T const&, std::vector<void const*>&)
    {
      throw Exception(errors::ProductDoesNotSupportViews)
        << "Product type " << cet::demangle_symbol(typeid(T).name())
        << " has no fillView() capability.\n";
    }
  };

  template <class T, typename = void>
  struct MaybeFillView : CannotFillView<T> {};

  template <typename T>
  struct MaybeFillView<T, std::enable_if_t<has_fillView<T>::value>> {
    static void fill(T const & product,
                     std::vector<void const *> & view)
    {
      product.fillView(view);
    }
  };

  template <class T, class A>
  struct MaybeFillView<std::vector<T,A>> {
    static void fill(std::vector<T> const& product,
                     std::vector<void const*>& view)
    {
      for (auto const& p : product) {
        view.push_back(&p);
      }
    }
  };

  template <class A>
  struct MaybeFillView<std::vector<bool,A>> : CannotFillView<std::vector<bool,A>> {};

  template <class T, class A>
  struct MaybeFillView<std::list<T,A>> {
    static void fill(std::list<T> const& product,
                     std::vector<void const*>& view)
    {
      for (auto const& p : product) {
        view.push_back(&p);
      }
    }
  };

  template <class T, class A>
  struct MaybeFillView<std::deque<T,A>> {
    static void fill(std::deque<T> const& product,
                     std::vector<void const*>& view)
    {
      for (auto const& p : product) {
        view.push_back(&p);
      }
    }
  };

  template <class T, class A>
  struct MaybeFillView<std::set<T,A>> {
    static void fill(std::set<T> const& product,
                     std::vector<void const*>& view)
    {
      for (auto const & p : product) {
        view.push_back(&p);
      }
    }
  };

  template <class T>
  struct MaybeFillView<cet::map_vector<T>> {
    static void fill(cet::map_vector<T> const& product,
                     std::vector<void const*>& view)
    {
      for (auto const & p : product) {
        view.push_back(&p.second);
      }
    }
  };

  //------------------------------------------------------------
  //
  // The trait struct template has_setPtr<T> is used to
  // indicate whether or not the type T has a member function
  //
  //      void T::setPtr(const std::type_info&, void const*&) const
  //
  // We assume the 'general case' for T is to not support setPtr.
  // Classes which do support setPtr must specialize this trait.
  //
  //------------------------------------------------------------

  template <class T>
  struct has_setPtr : std::false_type {};

  template <class T, class A>
  struct has_setPtr<std::vector<T,A>> : std::true_type {};

  template <class A>
  struct has_setPtr<std::vector<bool,A>> : std::false_type {};

  template <class T, class A>
  struct has_setPtr<std::list<T,A>> : std::true_type {};

  template <class T, class A>
  struct has_setPtr<std::deque<T,A>> : std::true_type {};

  template <class T, class A>
  struct has_setPtr<std::set<T,A>> : std::true_type {};

  template <class T>
  struct has_setPtr<cet::map_vector<T>> : std::true_type {};
}

#endif /* canvas_Persistency_Common_traits_h */

// Local Variables:
// mode: c++
// End:
