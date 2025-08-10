#pragma once

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

#include "util/Joaat.hpp"
#include "util/Logger.hpp"
#include "Settings.hpp"

namespace SCOL
{
	using namespace std::chrono_literals;
}