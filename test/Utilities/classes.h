#include "test/Utilities/TypeNameBranchName_t.h"
#include <vector>

namespace arttest {
struct empty {};
struct also_empty {};
} // namespace arttest

template class std::vector<arttest::MyString>;
