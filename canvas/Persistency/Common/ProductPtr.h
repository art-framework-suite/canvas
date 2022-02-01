#ifndef canvas_Persistency_Common_ProductPtr_h
#define canvas_Persistency_Common_ProductPtr_h
// vim: set sw=2 expandtab :

//
//  ProductPtr and related functions.
//
//  A ProductPtr is a persistent smart pointer to a data product.
//
//  How to construct a ProductPtr<T>:
//
//  1. Default constructor.
//       ProductPtr();
//
//     Constructs an invalid ProductPtr.
//
//  2. From a handle to a product.
//       template<typename H>
//       ProductPtr(H const&);
//
//  3. From a ProductID.
//       ProductPtr(ProductID const&); // Invalid ("null") ProductPtr.
//
//       ProductPtr(Product ID const&, EDProductGetter const*);
//
//     Obtain the ProductID from the handle or the result of
//     Event::put(). Obtain the EDProductGetter from the event using
//     the ProductID.
//
//  4. From a ProductPtr<U> where U is a base or sub class of T.
//       ProductPtr(ProductPtr<U> const&);
//

#include "canvas/Persistency/Common/EDProduct.h"
#include "canvas/Persistency/Common/EDProductGetter.h"
#include "canvas/Persistency/Common/RefCore.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

#include <cstddef>
#include <ostream>
#include <type_traits>

namespace art {

  template <typename T>
  class ProductPtr {
  public:
    using value_type = T;
    using const_pointer = T const*;
    using const_reference = T const&;

    // 1.
    ProductPtr() = default;

    template <typename H>
    explicit ProductPtr(H const& handle)
      : core_{handle.id(), handle.product(), nullptr}
    {
      if (core_.isNull()) {
        throw Exception(errors::InvalidReference)
          << "Attempt to construct a ProductPtr from a Handle with invalid "
             "ProductID. "
             "Perhaps a\n"
             "default-constructed ProductPtr is what you want?";
      }
    }

    // 3A.
    explicit ProductPtr(ProductID const& productID)
      : core_{productID, nullptr, nullptr}
    {}

    // 3B.
    ProductPtr(ProductID const& productID, EDProductGetter const* prodGetter)
      : core_{productID, nullptr, prodGetter}
    {}

    // 4.
    template <typename U>
    ProductPtr(ProductPtr<U> const& pu,
               std::enable_if_t<std::is_base_of_v<T, U>>* = nullptr)
      : core_{pu.id(),
              (pu.hasCache() ? static_cast<T const*>(pu.get()) : nullptr),
              pu.productGetter()}
    {}

    template <typename U>
    ProductPtr(ProductPtr<U> const& pu,
               std::enable_if_t<std::is_base_of_v<U, T>>* = nullptr)
      : core_{pu.id(), static_cast<T const*>(pu.get()), nullptr}
    {}

    //
    //  Accessors.
    //

    T const& operator*() const
    {
      // FIXME: This causes an nullptr dereference if isNull!
      // return isNull() ? nullptr : operator->();
      return *get();
    }

    T const*
    get() const
    {
      return core_.isNull() ? nullptr : operator->();
    }

    T const* operator->() const
    {
      if (core_.productPtr() == nullptr) {
        core_.setProductPtr(product_());
      }
      return reinterpret_cast<T const*>(core_.productPtr());
    }

    explicit operator bool() const { return core_.isAvailable(); }

    RefCore const&
    refCore() const noexcept
    {
      return core_;
    }

    ProductID
    id() const noexcept
    {
      return core_.id();
    }

    EDProductGetter const*
    productGetter() const noexcept
    {
      return core_.productGetter();
    }

    // Checks if collection is in memory or available
    // in the event. No type checking is done.
    bool
    isAvailable() const
    {
      return core_.isAvailable();
    }

    bool
    hasCache() const noexcept
    {
      return core_.productPtr() != nullptr;
    }

    // MUST UPDATE WHEN CLASS IS CHANGED!
    static constexpr short
    Class_Version() noexcept
    {
      return 10;
    }

  private:
    T const*
    product_() const
    {
      EDProduct const* product{nullptr};
      if (productGetter()) {
        product = productGetter()->getIt();
      }
      if (product == nullptr) {
        Exception e(errors::ProductNotFound);
        e << "A request to resolve a ProductPtr to a product of type: "
          << cet::demangle_symbol(typeid(T).name()) << " with ProductID "
          << core_.id()
          << "\ncannot be satisfied because the product cannot be found.\n";
        if (productGetter() == nullptr) {
          e << "The productGetter was not set -- are you trying to "
               "dereference a ProductPtr during mixing?\n";
        } else {
          e << "Probably the branch containing the product is not stored in "
               "the input file.\n";
        }
        throw e;
      }
      auto wrapped_product = dynamic_cast<Wrapper<T> const*>(product);
      if (wrapped_product == nullptr) {
        throw Exception{errors::ProductNotFound}
          << "A request to resolve a Product Ptr to a product of type: "
          << cet::demangle_symbol(typeid(T).name()) << " with ProductID"
          << core_.id() << "\ncannot be satisfied due to a type mismatch.\n";
      }
      return wrapped_product->product();
    }

    // Used to fetch the container product.
    RefCore core_{};
  };

  template <typename T, typename U>
  std::enable_if_t<std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                     std::is_base_of_v<U, T>,
                   bool>
  operator==(ProductPtr<T> const& lhs, ProductPtr<U> const& rhs)
  {
    return lhs.refCore() == rhs.refCore();
  }

  template <typename T, typename U>
  std::enable_if_t<std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                     std::is_base_of_v<U, T>,
                   bool>
  operator!=(ProductPtr<T> const& lhs, ProductPtr<U> const& rhs)
  {
    return !(lhs == rhs);
  }

  template <typename T, typename U>
  std::enable_if_t<std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                     std::is_base_of_v<U, T>,
                   bool>
  operator<(ProductPtr<T> const& lhs, ProductPtr<U> const& rhs)
  {
    return lhs.refCore() < rhs.refCore();
  }

  template <typename T>
  std::ostream&
  operator<<(std::ostream& os, ProductPtr<T> const& p)
  {
    os << "(" << p.id() << ")";
    return os;
  }

} // namespace art

#endif /* canvas_Persistency_Common_ProductPtr_h */

// Local Variables:
// mode: c++
// End:
