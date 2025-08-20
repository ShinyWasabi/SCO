#pragma once

#include <AsyncLogger/Logger.hpp>
#include <MinHook.h>
#include <Windows.h>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>
#include <stack>
#include <unordered_set>

using namespace al;
#include "util/Joaat.hpp"
#include "Logging.hpp"
#include "Settings.hpp"

namespace SCOL
{
	using namespace std::chrono_literals;
}