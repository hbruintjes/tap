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

#include <vector>
#include <stdexcept>
#include <memory>

namespace TAP {

/**
 * Standard exception class for TAP. All TAP exceptions are derived from this.
 */
class exception : public std::exception {
protected:
    /** Description of the exception. See what(). */
    std::string m_what;

public:
    /**
     * Creates the exception with the given message.
     * @param what Exception details
     */
    exception(std::string what) : std::exception(), m_what(what) {};

    /**
     * Creates the exception with no message.
     */
    exception() : std::exception(), m_what() {};

    /**
     * See std::exception::what().
     */
    const char* what() const noexcept override {
        return m_what.c_str();
    }
};

/**
 * Exception class for errors in the command line (such as unknown arguments or
 * syntax errors).
 */
class command_error: public exception {
public:
    using exception::exception;
};

/**
 * Exception class raised when an unknown argument is encountered.
 */
class unknown_argument : public command_error {
public:
    /**
     * Creates the exception for positional arguments.
     */
    unknown_argument() :
        command_error("No positional arguments are supported") {
    }

    /**
     * Creates the exception for flag arguments.
     * @param flag The unknown flag
     */
    unknown_argument(char flag) :
        command_error(std::string("The flag argument ") + flag + " is unknown") {
    }

    /**
     * Creates the exception for name arguments.
     * @param name The unknown name
     */
    unknown_argument(std::string name) :
        command_error(std::string("The named argument ") + name + " is unknown") {
    }
};

/**
 * Exception class raised when an error occurs verifying a command line
 * argument.
 */
class argument_error : public exception {
protected:
    /** Copy of the argument involved in the error */
    std::unique_ptr<Argument> m_arg;

public:
    /**
     * Creates the exception for the given argument, without a specified
     * reason.
     * @param arg The argument with an error
     */
    argument_error(const Argument& arg);

    /**
     * Creates the exception for the given argument, with a specified reason.
     * @param arg The argument with an error
     * @param reason The reason of the error
     */
    argument_error(const Argument& arg, const std::string& reason);

    /**
     * Returns the argument triggering the error.
     * @return The argument triggering the error
     */
    const Argument& arg() const {
        return *m_arg;
    }
};

/**
 * Exception class raised when an argument is used an incorrect amount of
 * times.
 */
class argument_count_mismatch : public argument_error {
public:
    /**
     * Creates the exception for the given argument, with the number of
     * occurrences and the expected number of occurrences. If the expected
     * value is higher, it is assumed to be a minimum, otherwise a maximum.
     * @param arg The argument with an error
     * @param count The number of times the argument occurred
     * @param expected The number of times the argument was expected to occur
     */
    argument_count_mismatch(const Argument& arg, unsigned int count, unsigned int expected) :
        argument_error(arg) {
        if (count < expected) {
            if (expected > 1) {
                m_what += " is required to occur at least " + std::to_string(expected) + " times";
            } else {
                m_what += " is required";
            }
        } else {
            if (expected > 1) {
                m_what += " can occur at most " + std::to_string(expected) + " times";
            } else {
                m_what += " can only be set once";
            }
        }
    }
};

/**
 * Exception class raised when an argument value is incorrect.
 */
class argument_invalid_value : public argument_error {
public:
    /**
     * Creates the exception for the given argument, with the incorrect value.
     * @param arg The argument with an error
     * @param value The value given on the command line
     */
    argument_invalid_value(const Argument& arg, const std::string& value) :
        argument_error(arg, std::string("does not accept the value ") + value) {
    }
};

/**
 * Exception class raised when an argument value is missing from the command
 * line.
 */
class argument_missing_value : public argument_error {
public:
    /**
     * Creates the exception for the given argument, which misses a value.
     */
    argument_missing_value(const Argument& arg) :
        argument_error(arg, "requires a value") {
    }
};

/**
 * Exception class raised when an argument value is given but not expected.
 */
class argument_no_value : public argument_error {
public:
    /**
     * Creates the exception for the given argument, which was given a value
     * but does not accept one.
     */
    argument_no_value(const Argument& arg) :
        argument_error(arg, "does not accept a value") {
    }
};

/**
 * Exception class raised when an argument constraint is not satisfied.
 */
class constraint_error : public exception {
public:
    /**
     * Creates the exception with a reason for a constraint failure, and the
     * list of arguments involved.
     * @param reason Reason of failure
     * @param args List of arguments that caused the failure
     */
    constraint_error(const std::string& reason, const std::vector<const BaseArgument*>& args);
};
}
