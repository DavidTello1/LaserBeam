#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream> //*** should not be included in Dist versions
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Davos/Log.h"

#ifdef DVS_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
