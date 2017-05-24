#ifndef canvas_Persistency_Common_AssnsIter_h
#define canvas_Persistency_Common_AssnsIter_h

/* Assns Iterator for art::Assns<L, R, D> */
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"

#include <iterator>
#include <iostream>

namespace art {
   //for dereference return type
   template <class L, class R, class D>
   struct AssnsNode {
      art::Ptr<L> first {};
      art::Ptr<R> second {};
      D const * data {};
      AssnsNode() = default;
      AssnsNode(art::Ptr<L> l, art::Ptr<R> r, D const &d) : first(l), second(r), data(&d) { };
   };


////////////////////////////
// Const Iterator

template <class L, class R, class D>
    class const_AssnsIter : public std::iterator <std::random_access_iterator_tag, typename art::AssnsNode<L, R, D>, size_t>  {
public:
    const_AssnsIter() = default;
    const_AssnsIter(const art::Assns<L, R, D> & assns) : coll_(&assns), index_(assns.size()){};
    const_AssnsIter(const art::Assns<L, R, D> & assns, size_t i) : coll_(&assns), index_(i) {};
    const_AssnsIter<L, R, D> & operator =(const art::const_AssnsIter<L, R, D> & iter);

    const art::AssnsNode<L, R, D>& operator*() const;
    const art::AssnsNode<L, R, D>* operator->() const;
    const_AssnsIter<L, R, D>& operator++();
    const_AssnsIter<L, R, D> operator++(int);
    const_AssnsIter<L, R, D>& operator--();
    const_AssnsIter<L, R, D> operator--(int);
    bool operator==(art::const_AssnsIter<L, R, D> const & iter) const;
    bool operator!=(art::const_AssnsIter<L, R, D> const & iter) const;
    const_AssnsIter<L, R, D>& operator+=(size_t i);
    const_AssnsIter<L, R, D> operator+(size_t i) const;
    const_AssnsIter<L, R, D>& operator-=(size_t i);
    const_AssnsIter<L, R, D> operator-(size_t i) const;
    size_t operator-(art::const_AssnsIter<L, R, D> const iter1) const;
    art::AssnsNode<L, R, D> operator[](size_t i) const;

    bool operator<(art::const_AssnsIter<L, R, D> const & iter) const;
    bool operator<=(art::const_AssnsIter<L, R, D> const & iter) const;
    bool operator>(art::const_AssnsIter<L, R, D> const & iter) const;
    bool operator>=(art::const_AssnsIter<L, R, D> const & iter) const;


    size_t getIndex() const {return index_;};

private:
    const art::Assns<L, R, D>* coll_ = nullptr;
    size_t index_ = -1;
    mutable AssnsNode<L, R, D> node_ = {};
};


// Dereference
template <class L, class R, class D>
const typename art::AssnsNode<L, R, D>&
const_AssnsIter<L, R, D>::operator*() const
{
    node_.first = (*coll_)[index_].first;
    node_.second = (*coll_)[index_].second;
    node_.data = &(coll_->data(index_));
    return node_;
}

// right arrow
template <class L, class R, class D>
const typename art::AssnsNode<L, R, D>*
const_AssnsIter<L, R, D>::operator->() const
{
    node_.first = (*coll_)[index_].first;
    node_.second = (*coll_)[index_].second;
    node_.data = &(coll_->data(index_));
    return &node_;
}

// Pre-increment
template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D>&  const_AssnsIter<L, R, D>::operator++()
{
    ++index_;
    return *this;
}

// Post-increment
template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D> const_AssnsIter<L, R, D>::operator++(int)
{
    art::const_AssnsIter<L, R, D> tmp = *this;
    ++index_;
    return tmp;
}

// Pre-decrement
template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D>& const_AssnsIter<L, R, D>::operator--()
{
    --index_;
    return *this;
}
//post-decrement
template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D> const_AssnsIter<L, R, D>::operator--(int)
{
    art::const_AssnsIter<L, R, D> tmp = *this;
    --index_;
    return tmp;
}

//equality
template <class L, class R, class D>
bool const_AssnsIter<L, R, D>::operator==(art::const_AssnsIter<L, R, D> const & iter) const
{
    return index_==iter.index_;
}

//in-equality
template <class L, class R, class D>
bool const_AssnsIter<L, R, D>::operator!=(art::const_AssnsIter<L, R, D> const & iter) const
{
    return !(index_==iter.index_);
}

// increment by a given value ...
template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D>& const_AssnsIter<L, R, D>::operator+=(size_t i)
{
   //to do add check for index bounds and make sure it works for both positive and negative values
    index_+=i;
    return *this;
}

//random access
template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D> const_AssnsIter<L, R, D>::operator+(size_t i) const
{
    art::const_AssnsIter<L, R, D> tmp = *this;
    tmp.index_+=i;
    return tmp;
}

// decrement by a given value ...
template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D>& const_AssnsIter<L, R, D>::operator-=(size_t i)
{
   //to do add check for index bounds and make sure it works for both positive and negative values
    index_-=i;
    return *this;
}

//random access
template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D> const_AssnsIter<L, R, D>::operator-(size_t i) const
{
    art::const_AssnsIter<L, R, D> tmp = *this;
    tmp.index_-=i;
    return tmp;
}

//difference between two iterators to return an index
template <class L, class R, class D>
size_t const_AssnsIter<L, R, D>::operator-(art::const_AssnsIter<L, R, D> const iter1) const {
    return (iter1.index_ - index_);
}

// Dereference
template <class L, class R, class D>
typename art::AssnsNode<L, R, D>
const_AssnsIter<L, R, D>::operator[](size_t n) const
{
    art::const_AssnsIter<L, R, D> tmp = *this;
    tmp.index_+=n;
    return tmp.node_;
}

//less than
template <class L, class R, class D>
bool const_AssnsIter<L, R, D>::operator<(art::const_AssnsIter<L, R, D> const & iter) const
{
    return index_<iter.index_;
}

//less than equal to
template <class L, class R, class D>
bool const_AssnsIter<L, R, D>::operator<=(art::const_AssnsIter<L, R, D> const & iter) const
{
    return index_<=iter.index_;
}

//less than equal to
template <class L, class R, class D>
bool const_AssnsIter<L, R, D>::operator>(art::const_AssnsIter<L, R, D> const & iter) const
{
    return index_>iter.index_;
}

//greater than equal to
template <class L, class R, class D>
bool const_AssnsIter<L, R, D>::operator>=(art::const_AssnsIter<L, R, D> const & iter) const
{
    return index_>=iter.index_;
}

template <class L, class R, class D>
typename art::const_AssnsIter<L, R, D>& const_AssnsIter<L, R, D>::operator =(const art::const_AssnsIter<L, R, D>& iter)
{
    this->node_ = iter.node_;
    return *this;
}
}
#endif
