// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef EVSE_SEC_SUPPORT_OLDER_CPP_VERSIONS_HPP_
#define EVSE_SEC_SUPPORT_OLDER_CPP_VERSIONS_HPP_

#ifndef LIBEVSE_SECURITY_USE_BOOST_FILESYSTEM
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif

#ifndef LIBEVSE_SECURITY_USE_BOOST_FILESYSTEM
namespace fs = std::filesystem;
namespace fsstd = std;
#else
namespace fs = boost::filesystem;
namespace fsstd = boost::filesystem;
#endif

#endif /* EVSE_SEC_SUPPORT_OLDER_CPP_VERSIONS_HPP_ */
