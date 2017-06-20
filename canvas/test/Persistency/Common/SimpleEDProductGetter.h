#ifndef canvas_test_Persistency_Common_SimpleEDProductGetter_h
#define canvas_test_Persistency_Common_SimpleEDProductGetter_h

#include "canvas/Persistency/Common/EDProduct.h"
#include "canvas/Persistency/Common/EDProductGetter.h"
#include "canvas/Persistency/Common/Wrapper.h"

#include <map>
#include <memory>

class SimpleEDProductGetter : public art::EDProductGetter
{
public:

  using map_t = std::map<art::ProductID, std::shared_ptr<art::EDProduct>>;
  template <class T>
  void
  addProduct(art::ProductID const id, std::unique_ptr<T>&& p)
  {
    database[id] = std::shared_ptr<art::Wrapper<T>>(p.release());
  }

  size_t size() const
  { return database.size(); }

  virtual art::EDProduct const* getIt(art::ProductID const id) const
  {
    map_t::const_iterator i = database.find(id);
    if (i == database.end())
      throw art::Exception(art::errors::ProductNotFound, "InvalidID")
        << "No product with ProductID "
        << id
        << " is available from this EDProductGetter\n";
    return i->second.get();
  }

 private:
  map_t database;
};

#endif /* canvas_test_Persistency_Common_SimpleEDProductGetter_h */

// Local Variables:
// mode: c++
// End:
