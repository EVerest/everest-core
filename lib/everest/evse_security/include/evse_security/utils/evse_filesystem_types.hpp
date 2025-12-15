// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#ifndef LIBEVSE_SECURITY_USE_BOOST_FILESYSTEM
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#endif

#ifndef LIBEVSE_SECURITY_USE_BOOST_FILESYSTEM
namespace fs = std::filesystem;
namespace fsstd = std;
#else
namespace fs = boost::filesystem;
namespace fsstd = boost::filesystem;
#endif
