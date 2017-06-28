#ifndef canvas_Persistency_Common_Assns_h
#define canvas_Persistency_Common_Assns_h
// vim: set sw=2:
////////////////////////////////////////////////////////////////////////
// Assns
//
// An association collection: a persistable collection of associations
// between two items in other persistent collections, with an optional
// ancillary object containing information about the association
// itself.
//
// N.B. An Assns<A, B> only makes sense when A and B are distinct:
// otherwise, there are ordering ambiguities when making and accessing
// associations. An attempt to specify an Assns<A,A> or <A, A, D> will
// fail with "... has incomplete type and cannot be defined."
//
// Note that the associations may be one-to-one, one-to-many or
// many-to-many.
//
// An Assns need only be used directly:
//
// 1. When being filled and put into the event; or
//
// 2. When the emphasis is on the associations themselves (and
// possibly their order) rather than particularly the associated
// objects. When it is desired to loop over A (or B) objects and
// access their counterparts and/or extra data objects a FindOne or
// FindMany would be more suitable.
//
////////////////////////////////////
// Interface.
//////////
//
// Note that the structure of the Assns template is non-trivial
// because there is a variant that has an extra data object attached
// to each association, and a variant that does not. In order to
// minimize code duplication then, a fairly advanced specialization
// technique is used.
//
// In order that the user of an Assns does not need to be able to
// parse and understand this mechanism, the interface is presented
// below:
//
// Notes:
//
// * L and R below are the types of two objects to be found in
// collections in the event. D where used is an arbitrary
// user-supplied type wherein information is stored about the
// association between a particular L and a particular R.
//
// * An Assns operates essentially as a vector of pairs of Ptr, with
// auxiliary methods for accessing the attached extra data object for
// an association, if applicable.
//
// * An attempt to create an Assns with a template argument D of
// pointer-type will result in a compile-time assertion failure.
//
// Useful typedefs:
//
//   typedef std::pair<Ptr<L>, Ptr<R>> assn_t;
//
// Constructors:
//
//   Assns<L, R>();
//   Assns<L, R, D>();
//
// Modifiers:
//
//   void swap(Assns& other);
//   void addSingle(Ptr<L> const&, Ptr<R> const&); // Assns<L, R> only.
//   void addSingle(Ptr<L> const&, Ptr<R> const&, D const&);
//
// Accessors:
//
//   assn_iterator begin() const; // De-referencing an assn_iterator
//   assn_iterator end() const;   // yields an assn_t const&.
//   assn_t const& operator [] (size_type i) const;
//   assn_t const& at(size_type i) const; // Bounds-checked.
//   size_type size() const;
//   D const& data(std::size_t index) const;
//   D const& data(assn_iterator it) const;
//
////////////////////////////////////////////////////////////////////////

#include "canvas/Persistency/Common/AssnsIter.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "canvas/Persistency/Common/detail/throwPartnerException.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/TypeID.h"
#include "cetlib/container_algorithms.h"
#include "cetlib_except/demangle.h"

#include "TBuffer.h"
#include "TClassStreamer.h" // Temporary
#include "TClass.h"
#include "TClassRef.h"

#include <iostream>
#include <typeinfo>
#include <vector>

namespace art {
  // General template.
  template <typename L, typename R, typename D = void>
  class Assns;

  // Specializations.
  // 1.
  template <typename LR>
  class Assns<LR, LR, void>; // Unimplemented.
  // 2.
  template <typename LR, typename D>
  class Assns<LR, LR, D>; // Unimplemented.
  // 3.
  template <typename L, typename R>
  class Assns<L, R, void>; // No data: base class.

  namespace detail {
    // Temporary streamer class until ioread rules are
    // working again.
    template <typename L, typename R>
    class AssnsStreamer : public TClassStreamer {
    public:
      void operator()(TBuffer& R_b, void* objp) {
        static TClassRef cl{TClass::GetClass(typeid(Assns<L, R, void>))};
        auto obj = reinterpret_cast<Assns<L, R, void>*>(objp);
        if (R_b.IsReading()) {
          cl->ReadBuffer(R_b, obj);
          obj->fill_transients();
        }
        else {
          obj->fill_from_transients();
          cl->WriteBuffer(R_b, obj);
        }
      }
    };
  }
}

////////////////////////////////////////////////////////////////////////
// Implementation of the specialization (3).
template <typename L, typename R>
class art::Assns<L, R, void> {
public:
  typedef L left_t;
  typedef R right_t;
  typedef art::Assns<right_t, left_t, void> partner_t;

private:
  typedef std::vector<std::pair<Ptr<left_t>, Ptr<right_t>>> ptrs_t;
  typedef std::vector<std::pair<RefCore, std::size_t>> ptr_data_t;

public:
  typedef typename ptrs_t::value_type assn_t;
  typedef typename ptrs_t::const_iterator assn_iterator;
  typedef typename ptrs_t::size_type size_type;

  // Constructors, destructor.
  Assns();
  Assns(partner_t const& other);
  virtual ~Assns() = default;

  // Accessors.
  assn_iterator begin() const;
  assn_iterator end() const;
  assn_t const& operator [](size_type index) const;
  assn_t const& at(size_type index) const;
  size_type size() const;

  // Modifier.
  void addSingle(Ptr<left_t> const& left,
                 Ptr<right_t> const& right);
  void swap(art::Assns<L, R, void>& other);

  std::unique_ptr<EDProduct> makePartner(std::type_info const& wanted_wrapper_type) const;

  static short Class_Version() { return 10; }

  void aggregate(Assns const&) const {}

protected:
  virtual void swap_(art::Assns<L, R, void>& other);

  virtual std::unique_ptr<EDProduct> makePartner_(std::type_info const& wanted_wrapper_type) const;

private:
  friend class detail::AssnsStreamer<left_t, right_t>;
  friend class art::Assns<right_t, left_t, void>; // partner_t.

  // FIXME: The only reason this function is virtual is to cause the
  // correct behavior to occur when the wrong streamer class is
  // called. In future (>5.30.00) versions of ROOT that can register
  // ioread rules for class template instantiations using typedefs, this
  // can be made back into a static function.
#ifdef ROOT_CAN_REGISTER_IOREADS_PROPERLY
  static
#else
  virtual
#endif
  bool left_first()
#ifndef ROOT_CAN_REGISTER_IOREADS_PROPERLY
    const
#endif
    ;

  void fill_transients();
  void fill_from_transients();

  void init_streamer();

  ptrs_t ptrs_{}; //! transient
  ptr_data_t ptr_data_1_{};
  ptr_data_t ptr_data_2_{};
};

////////////////////////////////////////////////////////////////////////
// Yes, the general implementation inherits from the specific. Head
// exploded yet?
template <typename L, typename R, typename D>
class art::Assns : private art::Assns<L, R> {
private:
  typedef Assns<L, R> base;
public:
  typedef typename base::left_t left_t;
  typedef typename base::right_t right_t;
  typedef D data_t;
  typedef art::Assns<right_t, left_t, data_t> partner_t;
  typedef typename art::const_AssnsIter<L, R, D, Direction::Forward> const_iterator;
  typedef typename art::const_AssnsIter<L, R, D, Direction::Reverse> const_reverse_iterator;
  typedef typename base::size_type size_type;

  Assns();
  Assns(partner_t const& other);

  size_type size() const; // Implemented explicitly only to let Wrapper know.
  const_iterator begin() const;
  const_iterator end() const;
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

  using base::operator[];
  using base::at;

  data_t const& data(typename std::vector<data_t>::size_type index) const;
  data_t const& data(const_iterator it) const;

  void addSingle(Ptr<left_t> const& left,
                 Ptr<right_t> const& right,
                 data_t const& data);
  void swap(art::Assns<L, R, D>& other);

  // This is needed (as opposed to using base::makePartner) because
  // enable_if_function_exists_t does not detect the base's function.
  std::unique_ptr<EDProduct> makePartner(std::type_info const& wanted_wrapper_type) const;

  static short Class_Version() { return 10; }

  void aggregate(Assns const&) const {}

private:
  friend class art::Assns<right_t, left_t, data_t>; // partner_t.

  void swap_(art::Assns<L, R, void>& other) override;
  std::unique_ptr<EDProduct> makePartner_(std::type_info const& wanted_wrapper_type) const override;

  std::vector<data_t> data_;
};

/////

////////////////////////////////////////////////////////////////////////
template <typename L, typename R>
inline
art::Assns<L, R, void>::Assns()
{
  init_streamer();
}

template <typename L, typename R>
inline
art::Assns<L, R, void>::Assns(partner_t const& other)
{
  ptrs_.reserve(other.ptrs_.size());
  cet::transform_all(other.ptrs_,
                     std::back_inserter(ptrs_),
                     [](auto const& pr){
                       using pr_t = typename ptrs_t::value_type;
                       return pr_t{pr.second, pr.first};
                     });
  init_streamer();
}

template <typename L, typename R>
inline
typename art::Assns<L, R, void>::assn_iterator
art::Assns<L, R, void>::begin() const
{
  return ptrs_.begin();
}

template <typename L, typename R>
inline
typename art::Assns<L, R, void>::assn_iterator
art::Assns<L, R, void>::end() const
{
  return ptrs_.end();
}

template <typename L, typename R>
inline
typename art::Assns<L, R, void>::assn_t const&
art::Assns<L, R, void>::operator[](size_type const index) const
{
  return ptrs_[index];
}

template <typename L, typename R>
inline
typename art::Assns<L, R, void>::assn_t const&
art::Assns<L, R, void>::at(size_type const index) const
{
  return ptrs_.at(index);
}

template <typename L, typename R>
inline
typename art::Assns<L, R, void>::size_type
art::Assns<L, R, void>::size() const
{
  return ptrs_.size();
}

template <typename L, typename R>
inline
void
art::Assns<L, R, void>::addSingle(Ptr<left_t> const& left,
                                  Ptr<right_t> const& right)
{
  ptrs_.emplace_back(left, right);
}

template <typename L, typename R>
inline
void
art::Assns<L, R, void>::swap(art::Assns<L, R, void>& other)
{
  swap_(other);
}

template <typename L, typename R>
inline
std::unique_ptr<art::EDProduct>
art::Assns<L, R, void>::makePartner(std::type_info const& wanted_wrapper_type) const
{
  return makePartner_(wanted_wrapper_type);
}

template <typename L, typename R>
inline
void
art::Assns<L, R, void>::swap_(art::Assns<L, R, void>& other)
{
  using std::swap;
  swap(ptrs_, other.ptrs_);
  swap(ptr_data_1_, other.ptr_data_1_);
  swap(ptr_data_2_, other.ptr_data_2_);
}

template <typename L, typename R>
std::unique_ptr<art::EDProduct>
art::Assns<L, R, void>::makePartner_(std::type_info const& wanted_wrapper_type) const
{
  if (wanted_wrapper_type != typeid(Wrapper<partner_t>)) {
    detail::throwPartnerException(typeid(*this), wanted_wrapper_type);
  }
  return std::make_unique<Wrapper<partner_t>>(std::make_unique<partner_t>(*this));
}

template <typename L, typename R>
inline
bool
art::Assns<L, R, void>::left_first() const
{
  static bool lf_s = (art::TypeID{typeid(left_t)}.friendlyClassName() <
                      art::TypeID{typeid(right_t)}.friendlyClassName());
  return lf_s;
}

template <typename L, typename R>
void
art::Assns<L, R, void>::fill_transients()
{
  // Precondition: ptr_data_1_.size() = ptr_data_2_.size();
  ptrs_.clear();
  ptrs_.reserve(ptr_data_1_.size());
  ptr_data_t const& l_ref = left_first() ? ptr_data_1_ : ptr_data_2_;
  ptr_data_t const& r_ref = left_first() ? ptr_data_2_ : ptr_data_1_;
  for (auto l = l_ref.cbegin(),
         e = l_ref.cend(),
         r = r_ref.cbegin();
       l != e;
       ++l, ++r) {
    ptrs_.emplace_back(Ptr<left_t>{l->first.id(), l->second, l->first.productGetter()},
                       Ptr<right_t>{r->first.id(), r->second, r->first.productGetter()});
  }
  // Empty persistent representation.
  ptr_data_t tmp1, tmp2;
  ptr_data_1_.swap(tmp1);
  ptr_data_2_.swap(tmp2);
}

template <typename L, typename R>
void
art::Assns<L, R, void>::fill_from_transients()
{
  if (!ptr_data_1_.empty()) {
    assert(ptr_data_1_.size() == ptr_data_2_.size() &&
           ptr_data_2_.size() == ptrs_.size() &&
           "Assns: internal inconsistency between transient and persistent member data.");
    // Multiple output modules: nothing to do on second and subsequent
    // calls.
    return;
  }
  ptr_data_t& l_ref = left_first() ? ptr_data_1_ : ptr_data_2_;
  ptr_data_t& r_ref = left_first() ? ptr_data_2_ : ptr_data_1_;
  l_ref.reserve(ptrs_.size());
  r_ref.reserve(ptrs_.size());
  for (auto const& pr : ptrs_) {
    l_ref.emplace_back(pr.first.refCore(), pr.first.key());
    r_ref.emplace_back(pr.second.refCore(), pr.second.key());
  }
}

template <typename L, typename R>
void
art::Assns<L, R, void>::init_streamer()
{
  static TClassRef cl{TClass::GetClass(typeid(Assns<L, R, void>))};
  if (cl->GetStreamer() == nullptr) {
    cl->AdoptStreamer(new detail::AssnsStreamer<L, R>);
  }
}

template <typename L, typename R, typename D>
inline
art::Assns<L, R, D>::Assns()
{
  static_assert((!std::is_pointer<D>::value), "Data template argument must not be pointer type!");
}

template <typename L, typename R, typename D>
art::Assns<L, R, D>::Assns(partner_t const& other)
  : base(other)
  , data_(other.data_)
{}

template <typename L, typename R, typename D>
inline
typename art::Assns<L, R, void>::size_type
art::Assns<L, R, D>::size() const
{
  return base::size();
}

template <typename L, typename R, typename D>
inline
typename art::Assns<L, R, D>::const_iterator
art::Assns<L, R, D>::begin() const
{
  return const_iterator{*this, 0};
}

template <typename L, typename R, typename D>
inline
typename art::Assns<L, R, D>::const_iterator
art::Assns<L, R, D>::end() const
{
  return const_iterator{*this};
}

template <typename L, typename R, typename D>
inline
typename art::Assns<L, R, D>::const_reverse_iterator
art::Assns<L, R, D>::rbegin() const
{
  return const_reverse_iterator{*this, size()};
}

template <typename L, typename R, typename D>
inline
typename art::Assns<L, R, D>::const_reverse_iterator
art::Assns<L, R, D>::rend() const
{
  return const_reverse_iterator{*this, 0};
}

template <typename L, typename R, typename D>
inline
typename art::Assns<L, R, D>::data_t const&
art::Assns<L, R, D>::data(typename std::vector<data_t>::size_type index) const
{
  return data_.at(index);
}

template <typename L, typename R, typename D>
inline
typename art::Assns<L, R, D>::data_t const&
art::Assns<L, R, D>::data(const_iterator it) const
{
  return data_.at(it.getIndex());
}

template <typename L, typename R, typename D>
inline
void
art::Assns<L, R, D>::addSingle(Ptr<left_t> const& left,
                               Ptr<right_t> const& right,
                               data_t const& data)
{
  base::addSingle(left, right);
  data_.push_back(data);
}

template <typename L, typename R, typename D>
inline
void
art::Assns<L, R, D>::swap(Assns<L, R, D>& other)
{
  using std::swap;
  base::swap_(other);
  swap(data_, other.data_);
}

template <typename L, typename R, typename D>
inline
std::unique_ptr<art::EDProduct>
art::Assns<L, R, D>::makePartner(std::type_info const& wanted_wrapper_type) const
{
  return makePartner_(wanted_wrapper_type);
}

template <typename L, typename R, typename D>
inline
void
art::Assns<L, R, D>::swap_(Assns<L, R, void>& other)
{
  try {
    swap(dynamic_cast<Assns<L, R, D>&>(other));
  }
  catch (std::bad_cast const&) {
    throw Exception(errors::LogicError, "AssnsBadCast")
      << "Attempt to swap base with derived!\n";
  }
}

template <typename L, typename R, typename D>
std::unique_ptr<art::EDProduct>
art::Assns<L, R, D>::makePartner_(std::type_info const& wanted_wrapper_type) const
{
  using bp = typename base::partner_t;
  std::unique_ptr<art::EDProduct> result;
  if (wanted_wrapper_type == typeid(Wrapper<partner_t>)) { // Partner.
    result = std::make_unique<Wrapper<partner_t>>(std::make_unique<partner_t>(*this));
  } else if (wanted_wrapper_type == typeid(Wrapper<base>)) { // Base.
    result = std::make_unique<Wrapper<base>>(std::make_unique<base>(static_cast<base>(*this)));
  } else if (wanted_wrapper_type == typeid(Wrapper<bp>)) { // Base of partner.
    result = std::make_unique<Wrapper<bp>>(std::make_unique<bp>(static_cast<base>(*this)));
  } else { // Oops.
    detail::throwPartnerException(typeid(*this), wanted_wrapper_type);
  }
  return result;
}
#endif /* canvas_Persistency_Common_Assns_h */

// Local Variables:
// mode: c++
// End:
