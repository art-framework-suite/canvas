// ======================================================================
//
// Exception: art-specific customizations for cetlib_except/coded_exception
//
// Note that errors::ErrorCodes is tightly coupled to detail::translate()
// such that any change to one will necessitate a corresponding change to
// the other.  The actions table in Framework/Core/Actions may also need
// adjustment.
//
// ======================================================================

#include "canvas/Utilities/Exception.h"

using namespace art;

// Map an art::errors::ErrorCodes into the appropriate string.
std::string
ExceptionDetail::translate(errors::ErrorCodes code)
{
  using namespace errors;

  switch (code) {
  case OtherArt:
    return "OtherArt";
  case StdException:
    return "StdException";
  case Unknown:
    return "Unknown";
  case BadAlloc:
    return "BadAlloc";
  case BadExceptionType:
    return "BadExceptionType";
  case ProductNotFound:
    return "ProductNotFound";
  case DictionaryNotFound:
    return "DictionaryNotFound";
  case ProductPutFailure:
    return "ProductPutFailure";
  case Configuration:
    return "Configuration";
  case LogicError:
    return "LogicError";
  case UnimplementedFeature:
    return "UnimplementedFeature";
  case InvalidReference:
    return "InvalidReference";
  case TypeConversion:
    return "TypeConversion";
  case NullPointerError:
    return "NullPointerError";
  case EventTimeout:
    return "EventTimeout";
  case DataCorruption:
    return "DataCorruption";
  case ScheduleExecutionFailure:
    return "ScheduleExecutionFailure";
  case EventProcessorFailure:
    return "EventProcessorFailure";
  case EndJobFailure:
    return "EndJobFailure";
  case FileOpenError:
    return "FileOpenError";
  case FileReadError:
    return "FileReadError";
  case FatalRootError:
    return "FatalRootError";
  case MismatchedInputFiles:
    return "MismatchedInputFiles";
  case CatalogServiceError:
    return "CatalogServiceError";
  case ProductDoesNotSupportViews:
    return "ProductDoesNotSupportViews";
  case ProductDoesNotSupportPtr:
    return "ProductDoesNotSupportPtr";
  case SQLExecutionError:
    return "SQLExecutionError";
  case InvalidNumber:
    return "InvalidNumber";
  case NotFound:
    return "NotFound";
  case ServiceNotFound:
    return "ServiceNotFound";
  case ProductCannotBeAggregated:
    return "ProductCannotBeAggregated";
  case ProductRegistrationFailure:
    return "ProductRegistrationFailure";
  case EventRangeOverlap:
    return "EventRangeOverlap";
  }
  throw Exception{errors::LogicError}
    << "Internal error: missing string translation for error " << code
    << " which was not caught at compile time!\n";
}
