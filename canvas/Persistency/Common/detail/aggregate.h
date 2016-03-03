#ifndef canvas_Persistency_Common_detail_aggregate_h
#define canvas_Persistency_Common_detail_aggregate_h

#include "cetlib/map_vector.h"

#include <array>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace CLHEP {
  class HepVector;
  class Hep2Vector;
  class Hep3Vector;
  class HepLorentzVector;
  class HepMatrix;
  class HepSymMatrix;
}

namespace art {
  namespace detail {

    template <typename T>
    std::enable_if_t<!std::is_arithmetic<T>::value>
    aggregate(T& p, T const& other);

    template <typename T>
    std::enable_if_t<std::is_arithmetic<T>::value>
    aggregate(T& p, T const& other);

    template <typename T>
    void aggregate(std::vector<T>& p, std::vector<T> const& other);

    template <typename T>
    void aggregate(std::list<T>& p, std::list<T> const& other);

    template <typename T>
    void aggregate(std::deque<T>& p, std::deque<T> const& other);

    template <typename K, typename V>
    void aggregate(std::map<K,V>& p, std::map<K,V> const& other);

    template <typename K, typename V>
    void aggregate(std::multimap<K,V>& p, std::multimap<K,V> const& other);

    template <typename T>
    void aggregate(std::set<T>& p, std::set<T> const& other);

    template <typename F, typename S>
    void aggregate(std::pair<F,S>&, std::pair<F,S> const&);

    // std::array not supported by ROOT6
    template <typename T, size_t N>
    void aggregate(std::array<T,N>& p, std::array<T,N> const& other);

    // std::tuple not supported by ROOT6
    template <typename ... ARGS>
    void aggregate(std::tuple<ARGS...>&, std::tuple<ARGS...> const&);

    template <typename T>
    void aggregate(cet::map_vector<T>&, cet::map_vector<T> const&);

    [[noreturn]] void aggregate(std::string&, std::string const&);

    void aggregate(CLHEP::HepVector& p, CLHEP::HepVector const& other);
    void aggregate(CLHEP::Hep2Vector& p, CLHEP::Hep2Vector const& other);
    void aggregate(CLHEP::Hep3Vector& p, CLHEP::Hep3Vector const& other);
    void aggregate(CLHEP::HepLorentzVector& p, CLHEP::HepLorentzVector const& other);
    void aggregate(CLHEP::HepMatrix& p, CLHEP::HepMatrix const& other);
    void aggregate(CLHEP::HepSymMatrix& p, CLHEP::HepSymMatrix const& other);

    template <typename T>
    [[noreturn]] void EventOnlyProduct(T*);

  }
}

//===========================================================================
// Implementation below

#include "canvas/Utilities/Exception.h"
#include "cetlib/container_algorithms.h"
#include "cetlib/demangle.h"

#include <typeinfo>

template <typename T>
std::enable_if_t<!std::is_arithmetic<T>::value>
art::detail::aggregate(T& p, T const& other)
{
  p.aggregate(other);
}

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value>
art::detail::aggregate(T& p, T const& other)
{
  p += other;
}


template <typename T>
void
art::detail::aggregate(std::vector<T>& p, std::vector<T> const& other)
{
  p.insert(p.cend(), other.cbegin(), other.cend());
}


template <typename T>
void
art::detail::aggregate(std::list<T>& p, std::list<T> const& other)
{
  p.insert(p.cend(), other.cbegin(), other.cend());
}

template <typename T>
void
art::detail::aggregate(std::deque<T>& p, std::deque<T> const& other)
{
  p.insert(p.cend(), other.cbegin(), other.cend());
}

// std::array not currently supported by ROOT6
template <typename T, size_t N>
void
art::detail::aggregate(std::array<T,N>& p, std::array<T,N> const& other)
{
  cet::transform_all( p, other,
                      std::begin(p),
                      [](T t1, T const& t2) {
                        aggregate(t1, t2);
                        return t1;
                      } );
}

namespace art {
  namespace detail {

    template <std::size_t>
    struct AggregateTuple;

    template <>
    struct AggregateTuple<0u> {
      template <typename Tuple>
      static void combine(Tuple&, Tuple const&)
      {}
    };

    template <std::size_t I>
    struct AggregateTuple {
      template <typename Tuple>
      static void combine(Tuple& p, Tuple const& other)
      {
        aggregate(std::get<I>(p), std::get<I>(other) );
        AggregateTuple<I-1>::combine(p, other);
      }
    };

  }
}

// std::tuple not currently supported by ROOT6
template <typename ... ARGS>
void
art::detail::aggregate(std::tuple<ARGS...>& p, std::tuple<ARGS...> const& other)
{
  AggregateTuple<sizeof...(ARGS)-1>::combine(p, other);
}

template <typename K, typename V>
void
art::detail::aggregate(std::map<K,V>& p, std::map<K,V> const& other)
{
  p.insert(other.cbegin(), other.cend());
}

template <typename K, typename V>
void
art::detail::aggregate(std::pair<K,V>& p, std::pair<K,V> const& other)
{
  aggregate(p.first, other.first);
  aggregate(p.second, other.second);
}

template <typename K, typename V>
void
art::detail::aggregate(std::multimap<K,V>& p, std::multimap<K,V> const& other)
{
  p.insert(other.cbegin(), other.cend());
}

template <typename T>
void
art::detail::aggregate(std::set<T>& p, std::set<T> const& other)
{
  p.insert(other.cbegin(), other.cend());
}

template <typename T>
void
art::detail::aggregate(cet::map_vector<T>& p, cet::map_vector<T> const& other)
{
  p.insert(other.cbegin(), other.cend());
}

template <typename T>
void
art::detail::EventOnlyProduct(T*)
{
  throw art::Exception(art::errors::ProductCannotBeAggregated)
    << "A product of type \""
    << cet::demangle_symbol(typeid(T).name())
    << "\" cannot be aggregated as it is an event-only product.\n"
    << "Product aggregation is not supported for events.\n";
}

#endif

// Local variables:
// mode: c++
// End:
