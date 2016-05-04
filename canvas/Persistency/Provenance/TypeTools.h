#ifndef art_Persistency_Provenance_TypeTools_h
#define art_Persistency_Provenance_TypeTools_h
// vim: set sw=2:

//
//  TypeTools provides a small number of Reflex-based tools, used in
//  the CMS event model.
//

#include "canvas/Persistency/Provenance/TypeWithDict.h"
#include "canvas/Utilities/WrappedClassName.h"
#include "TClass.h"
#include <ostream>
#include <string>
#include <vector>

namespace art {

  TypeWithDict
  find_nested_type_named(std::string const& nested_type,
                         TClass* const type_to_search);

  TypeWithDict
  value_type_of(TClass* t);

  TypeWithDict
  mapped_type_of(TClass* t);

  void
  public_base_classes(TClass* cl, std::vector<TClass*>& baseTypes);

  std::string
  name_of_template_arg(std::string const & template_instance,
                       size_t desired_arg);

  TypeWithDict
  type_of_template_arg(TClass* template_instance, size_t desired_arg);

  TypeWithDict
  type_of_template_arg(std::string const & template_instance,
                       size_t desired_arg);

  std::string
  name_of_assns_partner(std::string assns_type_name);

  TypeWithDict
  type_of_assns_partner(std::string assns_type_name);

  bool
  is_instantiation_of(TClass* cl, std::string const& template_name);

  void
  throwLateDictionaryError(std::string const & className);

} // namespace art

inline
art::TypeWithDict
art::type_of_template_arg(std::string const & template_instance,
                          size_t desired_arg)
{
  TypeWithDict found_type(name_of_template_arg(template_instance, desired_arg));
  return found_type;
}

inline
art::TypeWithDict
art::type_of_assns_partner(std::string assns_type_name)
{
  TypeWithDict result(name_of_assns_partner(assns_type_name));
  return result;
}



// Local Variables:
// mode: c++
// End:
#endif // art_Persistency_Provenance_TypeTools_h
