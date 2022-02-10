#ifndef canvas_Persistency_Common_Ptr_h
#define canvas_Persistency_Common_Ptr_h
// vim: set sw=2 expandtab :

//
//  Ptr and related functions.
//
//  A Ptr is a persistent smart pointer to an item in a collection
//  that is a data product.
//
//  How to construct a Ptr<T>:
//
//  1. Default constructor.
//       Ptr();
//
//     Constructs an invalid Ptr.
//
//  2. From a handle to a collection and an index into that collection.
//       template<typename H>
//       Ptr(H const&, key_type);
//
//  3. From a ProductID.
//       Ptr(Product ID const&, key_type, EDProductGetter const*);
//
//     Obtain the ProductID from the collection handle or the result of
//     Event::put(). Obtain the EDProductGetter from the event using the
//     ProductID.
//
//  4. From a Ptr<U> where U is a base or sub class of T.
//       Ptr(Ptr<U> const&);
//
//  5. From a ProductID and an existing resolved pointer.
//       Ptr(ProductID const&, T const*, key_type)
//
//     This signature is for expert-level or internal use only: it is a
//     pre-condition that the pointer must be the item at the index
//     specified in product with the given ID. Errors will *not* be
//     detected.
//

#include "canvas/Persistency/Common/EDProduct.h"
#include "canvas/Persistency/Common/EDProductGetter.h"
#include "canvas/Persistency/Common/GetProduct.h"
#include "canvas/Persistency/Common/RefCore.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "canvas/Persistency/Common/traits.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/ensurePointer.h"
#include "cetlib_except/demangle.h"

#include <cassert>
#include <cstddef>
#include <functional> // for std::hash
#include <list>
#include <ostream>
#include <type_traits>
#include <vector>

namespace art {

  namespace detail {

    template <typename T, typename C>
    class ItemGetter;

    template <typename T>
    class ItemGetter<T, cet::map_vector<T>>;

    template <typename T>
    class ItemGetter<std::pair<cet::map_vector_key, T>, cet::map_vector<T>>;

  } // namespace detail

  template <typename T>
  class Ptr {
  public:
    using key_type = std::size_t;
    using value_type = T;
    using const_pointer = T const*;
    using const_reference = T const&;

    // 1.
    Ptr() = default;

    template <typename H>
    Ptr(H const& handle, typename Ptr<T>::key_type key)
      : core_{handle.id(),
              detail::ItemGetter<T, typename H::element_type>{}(
                handle.product(),
                key),
              handle.productGetter()}
      , key_{key}
    {
      if (core_.isNull()) {
        throw Exception(errors::InvalidReference)
          << "Attempt to construct a Ptr from a Handle with invalid ProductID. "
             "Perhaps a\n"
             "default-constructed Ptr is what you want?";
      }
    }

    // 3.
    Ptr(ProductID const& productID,
        key_type itemKey,
        EDProductGetter const* prodGetter)
      : core_{productID, nullptr, prodGetter}, key_{itemKey}
    {}

    // 4.
    template <typename U>
    Ptr(Ptr<U> const& pu, std::enable_if_t<std::is_base_of_v<T, U>>* = nullptr)
      : core_{pu.id(), pu.get(), pu.productGetter()}, key_{pu.key()}
    {}

    template <typename U>
    Ptr(Ptr<U> const& pu, std::enable_if_t<std::is_base_of_v<U, T>>* = nullptr)
      : core_{pu.id(), static_cast<T const*>(pu.get()), nullptr}, key_{pu.key()}
    {}

    // 5. See notes above.
    Ptr(ProductID const& productID, T const* item, key_type const itemKey)
      : core_{productID, item, nullptr}, key_{itemKey}
    {}

    // =========
    // Accessors
    // =========

    T const& operator*() const
    {
      // Warning: This causes a nullptr dereference if isNull!
      return *get();
    }

    T const*
    get() const
    {
      return isNull() ? nullptr : operator->();
    }

    T const* operator->() const
    {
      if (core_.productPtr() == nullptr) {
        EDProduct const* prod = parentProduct_();
        void const* ad = prod->getElementAddress(typeid(T), key_);
        core_.setProductPtr(ad);
      }
      return reinterpret_cast<T const*>(core_.productPtr());
    }

    explicit operator bool() const
    {
      return isNonnull() && core_.isAvailable();
    }

    ProductID
    id() const noexcept
    {
      return core_.id();
    }

    key_type
    key() const noexcept
    {
      return key_;
    }

    // Retrieve parent collection
    template <typename Collection>
    Collection const&
    parentAs() const
    {
      core_.isAvailable();
      auto product = parentProduct_();
      auto wrapped_product = dynamic_cast<Wrapper<Collection> const*>(product);
      if (wrapped_product == nullptr) {
        throw Exception(errors::ProductNotFound)
          << "A request to retrieve the parent collection of type: "
          << cet::demangle_symbol(typeid(Collection).name())
          << " with ProductID " << core_.id()
          << "\ncannot be satisfied due to a type mismatch.\n";
      }
      return *wrapped_product->product();
    }

    template <template <typename...> class Collection, typename U = T>
    Collection<U> const&
    parentAs() const
    {
      return parentAs<Collection<U>>();
    }

    RefCore const&
    refCore() const noexcept
    {
      return core_;
    }

    // Checks for valid key.
    bool
    isNonnull() const noexcept
    {
      return key_ != key_traits<key_type>::value;
    }

    // Checks for valid key.
    bool
    isNull() const noexcept
    {
      return !isNonnull();
    }

    // Checks if collection is in memory or available in the event. No
    // type checking is done.
    bool
    isAvailable() const
    {
      return core_.isAvailable();
    }

    // MUST UPDATE WHEN CLASS IS CHANGED!
    static constexpr short
    Class_Version() noexcept
    {
      return 10;
    }

    EDProductGetter const*
    productGetter() const noexcept
    {
      return core_.productGetter();
    }

  private:
    EDProduct const*
    parentProduct_() const
    {
      EDProduct const* product{nullptr};
      if (productGetter()) {
        product = productGetter()->getIt();
      }
      if (product == nullptr) {
        Exception e(errors::ProductNotFound);
        e << "A request to resolve an Ptr to a product containing items of "
             "type: "
          << cet::demangle_symbol(typeid(T).name()) << " with ProductID "
          << core_.id()
          << "\ncannot be satisfied because the product cannot be found.\n";
        if (productGetter() == nullptr) {
          e << "The productGetter was not set -- are you trying to "
               "dereference a Ptr during mixing?\n";
        } else {
          e << "Probably the branch containing the product is not stored in "
               "the input file.\n";
        }
        throw e;
      }
      return product;
    }

    // Used to fetch the container product.
    mutable RefCore core_{};

    // Index into the container product.
    key_type key_{key_traits<key_type>::value};
  };

  template <typename T, typename U>
  std::enable_if_t<std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                     std::is_base_of_v<U, T>,
                   bool>
  operator==(Ptr<T> const& lhs, Ptr<U> const& rhs)
  {
    return lhs.refCore() == rhs.refCore() and lhs.key() == rhs.key();
  }

  template <typename T, typename U>
  std::enable_if_t<std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                     std::is_base_of_v<U, T>,
                   bool>
  operator!=(Ptr<T> const& lhs, Ptr<U> const& rhs)
  {
    return !(lhs == rhs);
  }

  template <typename T, typename U>
  std::enable_if_t<std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                     std::is_base_of_v<U, T>,
                   bool>
  operator<(Ptr<T> const& lhs, Ptr<U> const& rhs)
  {
    // The ordering of integer keys guarantees that the ordering of Ptrs within
    // a collection will be identical to the ordering of the referenced objects
    // in the collection.
    return lhs.refCore() == rhs.refCore() ? lhs.key() < rhs.key() :
                                            lhs.refCore() < rhs.refCore();
  }

  // Fill a vector of Ptrs from a persistent collection.
  // Alternatively, construct a PtrVector.
  template <typename T, typename H>
  void
  fill_ptr_vector(std::vector<Ptr<T>>& ptrs, H const& h)
  {
    for (std::size_t i = 0, sz = h->size(); i != sz; ++i) {
      ptrs.emplace_back(h, i);
    }
  }

  // Fill a list of Ptrs from a persistent collection.
  template <typename T, typename H>
  void
  fill_ptr_list(std::list<Ptr<T>>& ptrs, H const& h)
  {
    for (std::size_t i = 0, sz = h->size(); i != sz; ++i) {
      ptrs.emplace_back(h, i);
    }
  }

  template <typename T>
  std::ostream&
  operator<<(std::ostream& os, Ptr<T> const& p)
  {
    os << "(" << p.id() << ", " << p.key() << ")";
    return os;
  }

  namespace detail {

    // Specialize EnsurePointer for Ptr.
    template <typename TO, typename PTRVAL>
    struct EnsurePointer<TO, Ptr<PTRVAL>> {
      TO
      operator()(Ptr<PTRVAL>& from) const
      {
        return addr<TO, PTRVAL const>(*from);
      }

      TO
      operator()(Ptr<PTRVAL> const& from) const
      {
        return addr<TO, PTRVAL const>(*from);
      }
    };

    // FIXME: The code of ItemGetter, including specializations, would
    // be completely unnecessary if Handle were to provide access to the
    // setPtr() function of wrapper. As it is, some container-specific
    // code is duplicated between here and Wrapper, leading to
    // multiple points of maintenance (and failure).

    template <typename T, typename C>
    class ItemGetter {
    public:
      T const*
      operator()(C const* product, typename Ptr<T>::key_type iKey) const
      {
        assert(product != nullptr);
        auto it = product->begin();
        advance(it, iKey);
        return detail::GetProduct<C>::address(it);
      }
    };

    template <typename T>
    class ItemGetter<T, cet::map_vector<T>> {
    public:
      T const*
      operator()(cet::map_vector<T> const* product,
                 typename Ptr<T>::key_type key) const
      {
        assert(product != nullptr);
        return product->getOrNull(cet::map_vector_key{key});
      }
    };

    template <typename T>
    class ItemGetter<std::pair<cet::map_vector_key, T>, cet::map_vector<T>> {
    public:
      std::pair<cet::map_vector_key, T> const*
      operator()(cet::map_vector<T> const* product,
                 typename Ptr<T>::key_type key) const
      {
        assert(product != nullptr);
        auto it = product->find(cet::map_vector_key{key});
        if (it == product->end()) {
          return nullptr;
        }
        return &*it;
      }
    };

    template <typename T>
    struct NotMapVector : std::true_type {
      using type = typename T::value_type;
    };

    template <typename T>
    struct NotMapVector<cet::map_vector<T>> : std::false_type {};

    template <typename T>
    using not_map_vector_t = typename NotMapVector<T>::type;

  } // namespace detail

  // Deduction guide for handles.  We do not support CTAD for
  // cet::map_vector as it is possible to construct a Ptr to the
  // map_vector's value_type and mapped_type.  We don't want the
  // default deduction behavior to surprise users of cet::map_vector.
  template <typename H, typename T>
  Ptr(H, T)->Ptr<detail::not_map_vector_t<typename H::element_type>>;

} // namespace art

// Specialization of std::hash for art::Ptr
namespace std {
  template <typename T>
  struct hash<art::Ptr<T>> {
    using ptr_t = art::Ptr<T>;
    using key_t = typename ptr_t::key_type;

    size_t
    operator()(ptr_t const& p) const noexcept
    {
      return std::hash<art::ProductID>{}(p.id()) ^ std::hash<key_t>{}(p.key());
    }
  };
}

#endif /* canvas_Persistency_Common_Ptr_h */

// Local Variables:
// mode: c++
// End:
