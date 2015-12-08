#ifndef art_Persistency_Common_EDProductGetterFinder_h
#define art_Persistency_Common_EDProductGetterFinder_h

// The RefCoreStreamer uses the interface defined here
// to find the EDProductGetter corresponding to the
// current event and the ProductID. A pointer to the
// EDProductGetter is then written into the RefCore
// and will get used the first time the Ptr containing
// the RefCore is dereferenced.  This base class breaks
// the dependence of the RefCoreStreamer on the classes
// that derive from this class. In the full framework
// this will be a base class for the art::EventPrincipal
// and in artfwlite this will be a base class for
// the artfwlite::Event.

namespace art {

  class EDProductGetter;
  class ProductID;

  class EDProductGetterFinder {
  public:

    virtual ~EDProductGetterFinder() = default;

    EDProductGetter const* getEDProductGetter(ProductID const& pid) const {
      return getEDProductGetterImpl(pid);
    }

  private:
    virtual EDProductGetter const* getEDProductGetterImpl(ProductID const&) const = 0;
  };
}

#endif /* art_Persistency_Common_EDProductGetterFinder_h */

// Local Variables:
// mode: c++
// End:
