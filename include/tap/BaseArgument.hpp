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
 * @file base_argument.hpp
* @brief Contains the definitions for base_argument.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace TAP {

template<typename char_t>
class basic_argument;

/**
 * Base basic_argument class, used both by actual basic_argument classes and constraints
 * (basic_argument_constraint).
 */
template<typename char_t>
class base_argument {
protected:
    /** True if the basic_argument has to be set */
    bool m_required = false;

public:
    /**
     * Base constructor.
     */
    base_argument() {
    }

    /**
     * base_argument copy constructor.
     */
    base_argument(const base_argument&) = default;

    /**
     * base_argument move constructor.
     */
    base_argument(base_argument&&) = default;

    /**
     * base_argument destructor.
     */
    virtual ~base_argument() {
    }

    /**
     * base_argument assignment operator.
     */
    base_argument& operator=(const base_argument&) = default;

    /**
     * base_argument move assignment operator.
     */
    base_argument& operator=(base_argument&&) = default;

    /**
     * Collect all basic_argument instances contained in this basic_argument into the given
     * vector.
     * @param collector Vector to store the arguments in
     */
    virtual void find_all_arguments(std::vector<const basic_argument<char_t>*>& collector) const = 0;

    /**
     * Boolean conversion operator (so you can do things like
     * `if (arg) { ... }` ).
     * It is set to true if and only if the basic_argument is set.
     * @return True iff the basic_argument is set
     */
    explicit operator bool() const {
        return is_set();
    }

    /**
     * Return whether the basic_argument has been set at least once.
     * @return True iff the basic_argument is set
     */
    bool is_set() const {
        return count() > 0;
    }

    /**
     * Return the actual count of occurrences.
     * @return Count of times this basic_argument occurred
     */
    virtual unsigned int count() const = 0;

    ///////////////////
    // Test operations
    ///////////////////
    /**
     * Mark the basic_argument as required or not.
     * @param required If true, the basic_argument must be set at least once (see
     *        min())
     * @return Reference to this basic_argument
     */
    base_argument& set_required(bool required = true) {
        m_required = required;
        return *this;
    }

    /**
     * Returns whether the basic_argument is required.
     * @return True iff the basic_argument is required
     */
    bool required() const {
        return m_required;
    }

    /**
     * Check if the basic_argument constraints are satisfied. Returns if this is the
     * case, will throw an TAP::exception indicating the problem otherwise.
     */
    virtual void check_valid() const = 0;

    /**
     * Returns a string representing how the basic_argument may be used on the command
     * line
     * @return The usage string
     */
    virtual std::basic_string<char_t> usage() const = 0;

    /**
     * Make a clone of the base_argument. Returns a pointer which is owned by
     * the caller.
     * @return Clone of the base_argument (or sub-class thereof)
     */
    virtual std::unique_ptr<base_argument> clone() const & = 0;

    /**
     * Make a clone of the base_argument and move all data to it. Returns a
     * pointer which is owned by the caller. The original object is no longer
     * valid.
     * @return Clone of the base_argument (or sub-class thereof)
     */
    virtual std::unique_ptr<base_argument> clone() && = 0;

};

}
