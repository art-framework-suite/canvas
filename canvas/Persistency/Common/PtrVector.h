#ifndef canvas_Persistency_Common_PtrVector_h
#define canvas_Persistency_Common_PtrVector_h

// ======================================================================
//
// PtrVector: a container which returns art::Ptr<>'s referring to items
// in one container in the art::Event
//
// ======================================================================

#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVectorBase.h"
#include "cetlib/container_algorithms.h"

#include <initializer_list>
#include <iterator>
#include <vector>

namespace art {
  template <typename T>
  class PtrVector;

  template <typename T>
  void swap(PtrVector<T>&, PtrVector<T>&);

  template <typename Comp>
  class ComparePtrs {
  public:
    ComparePtrs(Comp const comp) : comp_{comp} {}
    template <typename T>
    bool
    operator()(Ptr<T> const& a, Ptr<T> const& b)
    {
      return comp_(*a, *b);
    }

  private:
    Comp comp_;
  };
}

template <typename T>
class art::PtrVector : public PtrVectorBase {
private:
  using data_t = std::vector<Ptr<T>>;

public:
  using value_type = typename data_t::value_type;
  using allocator_type = typename data_t::allocator_type;
  using reference = typename data_t::reference;
  using const_reference = typename data_t::const_reference;
  using pointer = typename data_t::pointer;
  using const_pointer = typename data_t::const_pointer;
  using iterator = typename data_t::iterator;
  using const_iterator = typename data_t::const_iterator;
  using reverse_iterator = typename data_t::reverse_iterator;
  using const_reverse_iterator = typename data_t::const_reverse_iterator;
  using difference_type = typename data_t::difference_type;
  using size_type = typename data_t::size_type;

  PtrVector();
  template <typename U>
  PtrVector(PtrVector<U> const& other);

  template <typename U>
  PtrVector(std::initializer_list<Ptr<U>> il);
  template <typename U>
  PtrVector<T>& operator=(std::initializer_list<Ptr<U>> il);

  template <typename U>
  PtrVector<T>& operator=(PtrVector<U> const& other) &;

  // Iterators.
  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;

  const_iterator cbegin() const;
  const_iterator cend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  // Capacity.
  size_type size() const;
  size_type max_size() const;
  void resize(size_type n);
  size_type capacity() const;
  bool empty() const;
  void reserve(size_type n);
  void shrink_to_fit();

  // Element access.
  Ptr<T> const& operator[](unsigned long const idx) const;
  reference at(size_type n);
  const_reference at(size_type n) const;
  reference front();
  const_reference front() const;
  reference back();
  const_reference back() const;
  // No data() functions by design.

  // Modifiers.
  template <typename U>
  void assign(size_type n, Ptr<U> const& p);
  template <class InputIterator>
  void assign(InputIterator first, InputIterator last);
  template <typename U>
  void assign(std::initializer_list<Ptr<U>> il);
  template <typename U>
  void push_back(Ptr<U> const& p);
  template <typename... Args>
  void emplace_back(Args&&... args);
  void pop_back();
  template <typename U>
  iterator insert(iterator position, Ptr<U> const& p);
  template <typename U>
  void insert(iterator position, size_type n, Ptr<U> const& p);
  template <typename InputIterator>
  iterator insert(const_iterator position,
                  InputIterator first,
                  InputIterator last);
  iterator erase(iterator position);
  iterator erase(iterator first, iterator last);
  void swap(PtrVector& other);
  void swap(key_type k1, key_type k2);
  void clear();
  // No emplace() due to problems associated with checking for
  // compatible ProductID.

  bool operator==(PtrVector const& other) const;
  void sort();
  template <class Comp>
  void sort(Comp comp);
  static short
  Class_Version()
  {
    return 11;
  }

private:
  void fill_offsets(indices_t& indices) override;
  void fill_from_offsets(indices_t const& indices) const override;
  void zeroTransients() override;

  // Need to explicitly zero this from custom streamer for base class.
  mutable data_t ptrs_{}; //! transient
};

#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>

// Constructors.
template <typename T>
inline art::PtrVector<T>::PtrVector() = default;

template <typename T>
template <typename U>
inline art::PtrVector<T>::PtrVector(PtrVector<U> const& other)
  : PtrVectorBase{other}
{
  // Ensure that types are compatible.
  static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>,
                "PtrVector: incompatible types");
  ptrs_.reserve(other.size());
  cet::copy_all(other, std::back_inserter(ptrs_));
}

template <typename T>
template <typename U>
inline art::PtrVector<T>::PtrVector(std::initializer_list<Ptr<U>> const il)
{
  static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                  std::is_base_of_v<U, T>,
                "PtrVector: incompatible types");
  ptrs_.reserve(il.size());
  for (auto&& p : il) {
    updateCore(p.refCore());
    ptrs_.push_back(std::move(p));
  }
}

template <typename T>
template <typename U>
inline art::PtrVector<T>&
art::PtrVector<T>::operator=(std::initializer_list<Ptr<U>> const il)
{
  static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                  std::is_base_of_v<U, T>,
                "PtrVector: incompatible types");
  assign(il);
  return *this;
}

template <typename T>
template <typename U>
inline art::PtrVector<T>&
art::PtrVector<T>::operator=(PtrVector<U> const& other) &
{
  static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>,
                "PtrVector: incompatible types");
  PtrVectorBase::operator=(other);
  ptrs_.clear();
  cet::copy_all(other, std::back_inserter(ptrs_));
  return *this;
}

// Iterators.
template <typename T>
inline auto
art::PtrVector<T>::begin() -> iterator
{
  return ptrs_.begin();
}

template <typename T>
inline auto
art::PtrVector<T>::begin() const -> const_iterator
{
  return ptrs_.begin();
}

template <typename T>
inline auto
art::PtrVector<T>::end() -> iterator
{
  return ptrs_.end();
}

template <typename T>
inline auto
art::PtrVector<T>::end() const -> const_iterator
{
  return ptrs_.end();
}

template <typename T>
inline auto
art::PtrVector<T>::rbegin() -> reverse_iterator
{
  return ptrs_.rbegin();
}

template <typename T>
inline auto
art::PtrVector<T>::rbegin() const -> const_reverse_iterator
{
  return ptrs_.rbegin();
}

template <typename T>
inline auto
art::PtrVector<T>::rend() -> reverse_iterator
{
  return ptrs_.rend();
}

template <typename T>
inline auto
art::PtrVector<T>::rend() const -> const_reverse_iterator
{
  return ptrs_.rend();
}

template <typename T>
inline auto
art::PtrVector<T>::cbegin() const -> const_iterator
{
  return ptrs_.cbegin();
}

template <typename T>
inline auto
art::PtrVector<T>::cend() const -> const_iterator
{
  return ptrs_.cend();
}

template <typename T>
inline auto
art::PtrVector<T>::crbegin() const -> const_reverse_iterator
{
  return ptrs_.crbegin();
}

template <typename T>
inline auto
art::PtrVector<T>::crend() const -> const_reverse_iterator
{
  return ptrs_.crend();
}

// Capacity.
template <typename T>
inline auto
art::PtrVector<T>::size() const -> size_type
{
  return ptrs_.size();
}

template <typename T>
inline auto
art::PtrVector<T>::max_size() const -> size_type
{
  return ptrs_.max_size();
}

template <typename T>
inline void
art::PtrVector<T>::resize(size_type const n)
{
  ptrs_.resize(n);
}

template <typename T>
inline auto
art::PtrVector<T>::capacity() const -> size_type
{
  return ptrs_.capacity();
}

template <typename T>
inline bool
art::PtrVector<T>::empty() const
{
  return ptrs_.empty();
}

template <typename T>
inline void
art::PtrVector<T>::reserve(size_type const n)
{
  ptrs_.reserve(n);
}

template <typename T>
inline void
art::PtrVector<T>::shrink_to_fit()
{
  ptrs_.shrink_to_fit();
}

// Element access.
template <typename T>
inline art::Ptr<T> const&
art::PtrVector<T>::operator[](unsigned long const idx) const
{
  return *(begin() + idx);
}

template <typename T>
inline auto
art::PtrVector<T>::at(size_type const n) -> reference
{
  return ptrs_.at(n);
}

template <typename T>
inline auto
art::PtrVector<T>::at(size_type const n) const -> const_reference
{
  return ptrs_.at(n);
}

template <typename T>
inline auto
art::PtrVector<T>::front() -> reference
{
  return ptrs_.front();
}

template <typename T>
inline auto
art::PtrVector<T>::front() const -> const_reference
{
  return ptrs_.front();
}

template <typename T>
inline auto
art::PtrVector<T>::back() -> reference
{
  return ptrs_.back();
}

template <typename T>
inline auto
art::PtrVector<T>::back() const -> const_reference
{
  return ptrs_.back();
}

// Modifiers.
template <typename T>
template <typename U>
inline void
art::PtrVector<T>::assign(size_type const n, Ptr<U> const& p)
{
  static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                  std::is_base_of_v<U, T>,
                "PtrVector: incompatible types");
  PtrVectorBase::clear();
  updateCore(p.refCore());
  ptrs_.assign(n, p);
}

template <typename T>
template <typename InputIterator>
inline void
art::PtrVector<T>::assign(InputIterator const first, InputIterator const last)
{
  PtrVectorBase::clear();
  std::for_each(
    first, last, [this](Ptr<T> const& p) { updateCore(p.refCore()); });
  ptrs_.assign(first, last);
}

template <typename T>
template <typename U>
inline void
art::PtrVector<T>::assign(std::initializer_list<Ptr<U>> const il)
{
  assign(il.begin(), il.end());
}

template <typename T>
template <typename U>
inline void
art::PtrVector<T>::push_back(Ptr<U> const& p)
{
  // Ensure that types are compatible.
  static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                  std::is_base_of_v<U, T>,
                "PtrVector: incompatible types");
  updateCore(p.refCore());
  ptrs_.push_back(p);
}

template <typename T>
template <typename... Args>
inline void
art::PtrVector<T>::emplace_back(Args&&... args)
{
  Ptr<T> p(std::forward<Args>(args)...);
  updateCore(p.refCore());
  ptrs_.push_back(std::move(p));
}

template <typename T>
inline void
art::PtrVector<T>::pop_back()
{
  ptrs_.pop_back();
}

template <typename T>
template <typename U>
inline typename art::PtrVector<T>::iterator
art::PtrVector<T>::insert(iterator const position, Ptr<U> const& p)
{
  // Ensure that types are compatible.
  static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                  std::is_base_of_v<U, T>,
                "PtrVector: incompatible types");
  updateCore(p.refCore());
  return ptrs_.insert(position, p);
}

template <typename T>
template <typename U>
inline void
art::PtrVector<T>::insert(iterator const position,
                          size_type const n,
                          Ptr<U> const& p)
{
  // Ensure that types are compatible.
  static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U> ||
                  std::is_base_of_v<U, T>,
                "PtrVector: incompatible types");
  updateCore(p.refCore());
  ptrs_.insert(position, n, p);
}

template <typename T>
template <typename InputIterator>
inline auto
art::PtrVector<T>::insert(const_iterator position,
                          InputIterator first,
                          InputIterator last) -> iterator
{
  std::for_each(
    first, last, [this](Ptr<T> const& p) { updateCore(p.refCore()); });
  return ptrs_.insert(position, first, last);
}

template <typename T>
inline auto
art::PtrVector<T>::erase(iterator position) -> iterator
{
  return ptrs_.erase(position);
}

template <typename T>
inline auto
art::PtrVector<T>::erase(iterator first, iterator last) -> iterator
{
  return ptrs_.erase(first, last);
}

template <typename T>
inline void
art::PtrVector<T>::swap(PtrVector& other)
{
  ptrs_.swap(other.ptrs_);
  PtrVectorBase::swap(other);
}

template <typename T>
inline void
art::PtrVector<T>::swap(key_type k1, key_type k2)
{
  std::swap(ptrs_[k1], ptrs_[k2]);
}

template <typename T>
inline void
art::PtrVector<T>::clear()
{
  ptrs_.clear();
  PtrVectorBase::clear();
}

template <typename T>
inline bool
art::PtrVector<T>::operator==(PtrVector const& other) const
{
  return ptrs_ == other.ptrs_ && PtrVectorBase::operator==(other);
}

template <typename T>
inline void
art::PtrVector<T>::sort()
{
  sort(std::less<T>{});
}

template <typename T>
template <class Comp>
inline void
art::PtrVector<T>::sort(Comp const comp)
{
  cet::sort_all(ptrs_, ComparePtrs{comp});
}

template <typename T>
void
art::PtrVector<T>::fill_offsets(indices_t& indices)
{
  // Precondition: indices is expected to be empty.
  assert(indices.empty());
  indices.reserve(ptrs_.size());
  for (auto const& i : ptrs_) {
    indices.push_back(i.key());
  }
}

template <typename T>
void
art::PtrVector<T>::fill_from_offsets(indices_t const& indices) const
{
  // Precondition: ptrs_ is expected to be empty.
  assert(ptrs_.empty());
  ptrs_.reserve(indices.size());
  for (auto i : indices) {
    ptrs_.emplace_back(id(), i, productGetter());
  }
}

template <typename T>
inline void
art::PtrVector<T>::zeroTransients()
{
  data_t tmp;
  ptrs_.swap(tmp);
}

template <typename T>
inline void
art::swap(PtrVector<T>& lhs, PtrVector<T>& rhs)
{
  lhs.swap(rhs);
}

#endif /* canvas_Persistency_Common_PtrVector_h */

// Local Variables:
// mode: c++
// End:
