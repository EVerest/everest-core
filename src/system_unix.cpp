// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "system_unix.hpp"

#include <cassert>
#include <stdexcept>

#include <fcntl.h>
#include <grp.h>
#include <linux/securebits.h>
#include <pwd.h>
#include <signal.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <fmt/core.h>

namespace Everest::system {

const auto PARENT_DIED_SIGNAL = SIGTERM;

struct GetPasswdEntryResult {
    explicit GetPasswdEntryResult(const std::string& error_) : error(error_) {
    }

    GetPasswdEntryResult(uid_t uid_, gid_t gid_, const std::vector<gid_t>& groups_) :
        uid(uid_), gid(gid_), groups(groups_) {
    }

    std::string error;
    uid_t uid;
    gid_t gid;
    std::vector<gid_t> groups;

    operator bool() const {
        return this->error.empty();
    }
};

static GetPasswdEntryResult get_passwd_entry(const std::string& user_name) {
    // Assuming that a user does not have more than 50 groups
    constexpr int max_supplementary_groups = 50;

    const auto entry = getpwnam(user_name.c_str());

    if (not entry) {
        return GetPasswdEntryResult("Could not get passwd entry for user name: " + user_name);
    }

    // get supplementary groups for this user
    int max_ngroups = max_supplementary_groups;
    gid_t groups[max_supplementary_groups];

    int ngroups = getgrouplist(user_name.c_str(), entry->pw_gid, groups, &max_ngroups);
    if (ngroups < 0) {
        return GetPasswdEntryResult("Could not get supplementary groups for user name: " + user_name);
    }

    return GetPasswdEntryResult(entry->pw_uid, entry->pw_gid, std::vector<gid_t>(groups, groups + ngroups));
}

bool keep_caps() {
    return (0 == cap_set_secbits(SECBIT_KEEP_CAPS));
}

std::string set_caps(const std::vector<std::string>& capabilities) {

    std::vector<cap_value_t> capability_values;
    capability_values.resize(capabilities.size());

    for (const auto& cap_name : capabilities) {
        auto& cap_value = capability_values.emplace_back();
        const auto error = cap_from_name(cap_name.c_str(), &cap_value);

        if (error) {
            return fmt::format("Failed to get capability value for capability name {}", cap_name);
        }
    }

    auto cap_ctx = cap_get_proc();
    if (cap_set_flag(cap_ctx, CAP_INHERITABLE, capability_values.size(), capability_values.data(), CAP_SET) != 0) {
        return "Failed to add capability flags to CAP_INHERITABLE";
    }

    if (cap_set_proc(cap_ctx) != 0) {
        return "Failed to set capabilities for process";
    };

    if (cap_free(cap_ctx) != 0) {
        return "Failed free memory for capability flags";
    };

    for (const auto cap_value : capability_values) {
        if (cap_set_ambient(cap_value, CAP_SET) != 0) {
            return "Failed to add capabilities to ambient set";
        }
    }

    return {};
}

std::string set_real_user(const std::string& user_name) {
    // Set special capabilities if required by module

    const auto entry = get_passwd_entry(user_name);

    if (not entry) {
        return entry.error;
    }

    const auto set_groups_failed = setgroups(entry.groups.size(), entry.groups.data());
    if (set_groups_failed) {
        return "setgroups failed";
    }

    const auto set_gid_failed = setgid(entry.gid);
    if (set_gid_failed) {
        return "setgid failed";
    }

    const auto set_uid_failed = setuid(entry.uid);
    if (set_uid_failed) {
        return "setuid failed";
    }

    return {};
}

void SubProcess::send_error_and_exit(const std::string& message) {
    assert(pid == 0);

    write(fd, message.c_str(), std::min(message.size(), MAX_PIPE_MESSAGE_SIZE - 1));
    close(fd);
    _exit(EXIT_FAILURE);
}

pid_t SubProcess::check_child_executed() {
    assert(pid != 0);

    if (check_child_executed_done) {
        return pid;
    }
    check_child_executed_done = true;

    std::string message(MAX_PIPE_MESSAGE_SIZE, 0);

    auto retval = read(fd, message.data(), MAX_PIPE_MESSAGE_SIZE);
    if (retval == -1) {
        throw std::runtime_error(fmt::format(
            "Failed to communicate via pipe with forked child process. Syscall to read() failed ({}), exiting",
            strerror(errno)));
    } else if (retval > 0) {
        throw std::runtime_error(fmt::format("Forked child process did not complete exec():\n{}", message.c_str()));
    }

    close(fd);
    return pid;
}

std::string set_user_and_capabilities(const std::string& run_as_user, const std::vector<std::string>& capabilities) {
    if (not capabilities.empty()) {
        // we need to keep caps, otherwise, we'll loose all our capabilities (except inherited)
        if (system::keep_caps() == false) {
            return "Keeping capabilities (SECBIT_KEEP_CAPS) failed";
        }
    }

    // Set real user for child process
    std::string error;
    if (not run_as_user.empty()) {
        error = system::set_real_user(run_as_user);
        if (not error.empty()) {
            return fmt::format("Failed to set real user to: {}", run_as_user);
        }
    }

    // Set capabilities for child process
    if (not capabilities.empty()) {
        error = system::set_caps(capabilities);
        if (not error.empty()) {
            return fmt::format("Failed to set capabilities: {}", error);
        }
    }

    return {};
}

SubProcess SubProcess::create(const std::string& run_as_user, const std::vector<std::string>& capabilities) {
    int pipefd[2];

    if (pipe2(pipefd, O_CLOEXEC | O_DIRECT)) {
        throw std::runtime_error(fmt::format("Syscall pipe2() failed ({}), exiting", strerror(errno)));
    }

    const auto reading_end_fd = pipefd[0];
    const auto writing_end_fd = pipefd[1];

    const auto parent_pid = getpid();

    const auto pid = fork();

    if (pid == -1) {
        throw std::runtime_error(fmt::format("Syscall fork() failed ({}), exiting", strerror(errno)));
    }

    if (pid == 0) {
        // close read end in child
        close(reading_end_fd);

        SubProcess handle{writing_end_fd, pid};
        auto error = set_user_and_capabilities(run_as_user, capabilities);

        if (not error.empty()) {
            handle.send_error_and_exit(error);
        }

        // FIXME (aw): how does the the forked process does cleanup when receiving PARENT_DIED_SIGNAL compared to
        //             _exit() before exec() has been called?
        if (prctl(PR_SET_PDEATHSIG, PARENT_DIED_SIGNAL)) {
            handle.send_error_and_exit(fmt::format("Syscall prctl() failed ({}), exiting", strerror(errno)));
        }

        if (getppid() != parent_pid) {
            // kill ourself, with the same handler as we would have
            // happened when the parent process died
            kill(getpid(), PARENT_DIED_SIGNAL);
        }

        return handle;
    } else {
        close(writing_end_fd);
        return {reading_end_fd, pid};
    }
}

} // namespace Everest::system
