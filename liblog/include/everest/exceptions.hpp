#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <boost/exception/exception.hpp>
#include <everest/metamacros.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

namespace Everest {
///
/// \brief base class for all everest logic exceptions
class EverestBaseError : public boost::exception {
public:
    ///
    /// \brief base constructor for all everest exceptions, inherited from boost::exception
    EverestBaseError() {
    }
};

///
/// \brief base class for all everest logic exceptions
class EverestBaseLogicError : public EverestBaseError, public std::logic_error {
public:
    ///
    /// \brief base constructor for all everest exceptions, inherited from std::logic_error
    explicit EverestBaseLogicError(const std::string& what) : std::logic_error(what) {
    }
};

///
/// \brief base class for all everest runtime exceptions
class EverestBaseRuntimeError : public EverestBaseError, public std::runtime_error {
public:
    ///
    /// \brief base constructor for all everest exceptions, inherited from std::runtime_error
    explicit EverestBaseRuntimeError(const std::string& what) : std::runtime_error(what) {
    }
};

///
/// \brief represents an internal error, like a missing configuration file
class EverestInternalError : public EverestBaseRuntimeError {
public:
    using EverestBaseRuntimeError::EverestBaseRuntimeError;
};

///
/// \brief represents a configuration error, like a malformed configuration entry
class EverestConfigError : public EverestBaseLogicError {
    using EverestBaseLogicError::EverestBaseLogicError;
};

///
/// \brief represents a API error, like tring to call a non-existing command
class EverestApiError : public EverestBaseLogicError {
public:
    using EverestBaseLogicError::EverestBaseLogicError;
};

///
/// \brief represents a timeout error, like tring to call a not answering command
class EverestTimeoutError : public EverestBaseRuntimeError {
public:
    using EverestBaseRuntimeError::EverestBaseRuntimeError;
};

} // namespace Everest

#define EVEXCEPTION(ex, ...)                                                                                           \
    ex((static_cast<const std::ostringstream&>(metamacro_foreach(_EVEXCEPTION_INTERNALS, <<, __VA_ARGS__))).str())
#define _EVEXCEPTION_INTERNALS(index, arg) metamacro_if_eq(0, index)(std::ostringstream() << arg)(arg)

#endif // EXCEPTIONS_HPP
