/**
Copyright (c) 2015 Harold Bruintjes

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgement in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
/**
 * @file Exceptions.hpp
 * @brief Contains the definitions for all TAP exceptions.
 */

#pragma once

#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace TAP {

/**
 * Standard exception class for TAP. All TAP exceptions are derived from this.
 */
template<typename char_t>
class exception : public std::exception {
protected:
    /** Description of the exception. See what(). */
    std::basic_string<char_t> m_what;
    mutable std::string m_whatNarrow;

public:
    /**
     * Creates the exception with the given message.
     * @param what Exception details
     */
    exception(std::basic_string<char_t> what) : std::exception(), m_what(what) {};

    /**
     * Creates the exception with no message.
     */
    exception() : std::exception(), m_what() {};

    /**
     * See std::exception::what().
     */
    const char* what() const noexcept override {
        return narrow(m_what.c_str(), m_what.size());
    }

private:
    template<typename char_tt>
    std::enable_if_t<std::is_same<char, char_tt>::value, const char*> narrow(char_tt const* what, size_t size) const {
        return what;
    }
    template<typename char_tt>
    std::enable_if_t<!std::is_same<char, char_tt>::value, const char*> narrow(char_tt const* what, size_t size) const {
        m_whatNarrow.resize(size, '\0');
        std::use_facet<std::ctype<char_t> >(std::locale()).narrow(what, what+size, '?', m_whatNarrow.data());
        return m_whatNarrow.c_str();
    }
};

/**
 * Exception class for errors in the command line (such as unknown arguments or
 * syntax errors).
 */
template<typename char_t>
class command_error: public exception<char_t> {
public:
    using exception<char_t>::exception;
};

/**
 * Exception class raised when an unknown basic_argument is encountered.
 */
template<typename char_t>
class unknown_argument : public command_error<char_t> {
public:
    /**
     * Creates the exception for positional arguments.
     */
    unknown_argument() :
        command_error<char_t>(widen_const<char_t>("No positional arguments are supported")) {
    }

    /**
     * Creates the exception for flag arguments.
     * @param flag The unknown flag
     */
    unknown_argument(char_t flag) :
        command_error<char_t>(widen_const<char_t>("The flag basic_argument ") + flag + widen_const<char_t>(" is unknown")) {
    }

    /**
     * Creates the exception for name arguments.
     * @param name The unknown name
     */
    unknown_argument(std::basic_string<char_t> name) :
        command_error<char_t>(widen_const<char_t>("The named basic_argument ") + name + widen_const<char_t>(" is unknown")) {
    }
};

/**
 * Exception class raised when an error occurs verifying a command line
 * basic_argument.
 */
template<typename char_t>
class argument_error : public exception<char_t> {
protected:
    /** Copy of the basic_argument involved in the error */
    std::unique_ptr<basic_argument<char_t>> m_arg;

public:
    /**
     * Creates the exception for the given basic_argument, without a specified
     * reason.
     * @param arg The basic_argument with an error
     */
    argument_error(const basic_argument<char_t>& arg);

    /**
     * Creates the exception for the given basic_argument, with a specified reason.
     * @param arg The basic_argument with an error
     * @param reason The reason of the error
     */
    argument_error(const basic_argument<char_t>& arg, const std::basic_string<char_t>& reason);

    /**
     * Returns the basic_argument triggering the error.
     * @return The basic_argument triggering the error
     */
    const basic_argument<char_t>& arg() const {
        return *m_arg;
    }
};

/**
 * Exception class raised when an basic_argument is used an incorrect amount of
 * times.
 */
template<typename char_t>
class argument_count_mismatch : public argument_error<char_t> {
public:
    /**
     * Creates the exception for the given basic_argument, with the number of
     * occurrences and the expected number of occurrences. If the expected
     * value is higher, it is assumed to be a minimum, otherwise a maximum.
     * @param arg The basic_argument with an error
     * @param count The number of times the basic_argument occurred
     * @param expected The number of times the basic_argument was expected to occur
     */
    argument_count_mismatch(const basic_argument<char_t>& arg, unsigned int count, unsigned int expected) :
            argument_error<char_t>(arg) {
        std::basic_ostringstream<char_t> ss;
        ss << expected;
        if (count < expected) {
            if (expected > 1) {
                argument_error<char_t>::m_what += widen_const<char_t>(" is required to occur at least ") + ss.str() + widen_const<char_t>(" times");
            } else {
                argument_error<char_t>::m_what += widen_const<char_t>(" is required");
            }
        } else {
            if (expected > 1) {
                argument_error<char_t>::m_what += widen_const<char_t>(" can occur at most ") + ss.str() + widen_const<char_t>(" times");
            } else {
                argument_error<char_t>::m_what += widen_const<char_t>(" can only be set once");
            }
        }
    }
};

/**
 * Exception class raised when an basic_argument value is incorrect.
 */
template<typename char_t>
class argument_invalid_value : public argument_error<char_t> {
public:
    /**
     * Creates the exception for the given basic_argument, with the incorrect value.
     * @param arg The basic_argument with an error
     * @param value The value given on the command line
     */
    argument_invalid_value(const basic_argument<char_t>& arg, const std::basic_string<char_t>& value) :
        argument_error<char_t>(arg, widen_const<char_t>("does not accept the value ") + value) {
    }
};

/**
 * Exception class raised when an basic_argument value is missing from the command
 * line.
 */
template<typename char_t>
class argument_missing_value : public argument_error<char_t> {
public:
    /**
     * Creates the exception for the given basic_argument, which misses a value.
     */
    argument_missing_value(const basic_argument<char_t>& arg) :
        argument_error<char_t>(arg, widen_const<char_t>("requires a value")) {
    }
};

/**
 * Exception class raised when an basic_argument value is given but not expected.
 */
template<typename char_t>
class argument_no_value : public argument_error<char_t> {
public:
    /**
     * Creates the exception for the given basic_argument, which was given a value
     * but does not accept one.
     */
    argument_no_value(const basic_argument<char_t>& arg) :
        argument_error<char_t>(arg, widen_const<char_t>("does not accept a value")) {
    }
};

/**
 * Exception class raised when an basic_argument constraint is not satisfied.
 */
template<typename char_t>
class constraint_error : public exception<char_t> {
public:
    /**
     * Creates the exception with a reason for a constraint failure, and the
     * list of arguments involved.
     * @param reason Reason of failure
     * @param args List of arguments that caused the failure
     */
    constraint_error(const std::basic_string<char_t>& reason, const std::vector<const base_argument<char_t>*>& args);
};
}
