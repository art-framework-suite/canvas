#ifndef canvas_Persistency_Provenance_thread_safe_registry_via_id_h
#define canvas_Persistency_Provenance_thread_safe_registry_via_id_h

// ===================================================================
// thread_safe_registry_via_id
//
// Provides safe access to registry, which never shrinks, but can grow
// and be read from multiple threads.
//
// This DOES NOT prevent reading during a write, nor vice versa.  If
// one needs to make sure that the contents of the registry are not
// changing during a read, then a guard must be placed around the
// registry traversal.
//
// Inefficiencies:
//
// - To insert pairs from an std::map<K,M> object, one must emplace
//   each pair one at a time.  This seems to be very inefficient;
//   however, unless we are willing to pay the consequences for
//   changing the underlying types of some of the persisted objects
//   that are represented in memory by the below registry, there may
//   not be a better solution.
// ===================================================================

#include "canvas/Persistency/Provenance/Hash.h"

#include <map>
#include <mutex>
#include <type_traits>

namespace art {
  template <typename K, typename M>
  class thread_safe_registry_via_id {
  public:
    using collection_type = std::map<K, M>;
    using value_type = typename collection_type::value_type;

    template <typename C>
    static void put(C const& container);
    static auto emplace(value_type const& value);
    static auto emplace(K const& key, M const& mapped);
    static bool empty();
    static collection_type const& get();
    static bool get(K const& key, M& mapped);
    static auto
    instance(bool cleanup = false)
    {
      std::lock_guard sentry{mutex_()};
      static collection_type* me = new collection_type{};
      if (cleanup) {
        delete me;
        me = nullptr;
        return me;
      }
      if (me == nullptr) {
        // We have been cleaned up and are now being used again.
        me = new collection_type{};
      }
      return me;
    }

  private:
    static auto&
    mutex_()
    {
      static std::recursive_mutex m{};
      return m;
    }
  };

  template <typename K, typename M>
  template <typename C>
  void
  thread_safe_registry_via_id<K, M>::put(C const& container)
  {
    std::lock_guard sentry{mutex_()};
    auto me = instance();
    for (auto const& e : container) {
      me->emplace(e);
    }
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::emplace(value_type const& value)
  {
    std::lock_guard sentry{mutex_()};
    return instance()->emplace(value);
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::emplace(K const& key, M const& mapped)
  {
    std::lock_guard sentry{mutex_()};
    return instance()->emplace(key, mapped);
  }

  template <typename K, typename M>
  bool
  thread_safe_registry_via_id<K, M>::empty()
  {
    std::lock_guard sentry{mutex_()};
    return instance()->empty();
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::get() -> collection_type const&
  {
    return *instance();
  }

  template <typename K, typename M>
  bool
  thread_safe_registry_via_id<K, M>::get(K const& k, M& mapped)
  {
    std::lock_guard sentry{mutex_()};
    auto me = instance();
    auto it = me->find(k);
    if (it != me->cend()) {
      mapped = it->second;
      return true;
    }
    return false;
  }
}

#endif /* canvas_Persistency_Provenance_thread_safe_registry_via_id_h */

// Local Variables:
// mode: c++
// End:
