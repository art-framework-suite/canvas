#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "cetlib/map_vector.h"
#include "cetlib/sha1.h"
#include <string>

template class boost::array<cet::sha1::uchar,cet::sha1::digest_sz>;
template class art::Wrapper<art::Ptr<cet::map_vector<std::string>::value_type>>;


// Local variables:
// mode: c++
// End:
