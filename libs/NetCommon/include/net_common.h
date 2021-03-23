#pragma once

#include <stdint.h> // for uint32_t etc.
#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

// log
#include "spdlog/spdlog.h"

#define NL "\n"
#define CR "\r\n"
