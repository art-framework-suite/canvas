#ifndef canvas_Utilities_FriendlyName_h
#define canvas_Utilities_FriendlyName_h
/*
 *  friendlyName.h
 *  CMSSW
 *
 *  Created by Chris Jones on 2/24/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
#include <string>

namespace art {
  namespace friendlyname {
    std::string friendlyName(std::string const& iFullName);
  }
}
#endif /* canvas_Utilities_FriendlyName_h */

// Local Variables:
// mode: c++
// End:
