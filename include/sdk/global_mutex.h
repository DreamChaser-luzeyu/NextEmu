#pragma once

#include <mutex>

namespace Builtin_ns {

extern std::mutex STDOutMutex_GV;  ///< Global stdout mutex

}