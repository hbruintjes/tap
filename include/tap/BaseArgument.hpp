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
 * @file BaseArgument.hpp
* @brief Contains the definitions for BaseArgument.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace TAP {

class Argument;

/**
 * Base argument class, used both by actual Argument classes and constraints
 * (ArgumentConstraint).
 */
class BaseArgument {
protected:
    /** True if the argument has to be set */
    bool m_required = false;

public:
    /**
     * Base constructor.
     */
    BaseArgument() {
    }

    /**
     * BaseArgument copy constructor.
     */
    BaseArgument(const BaseArgument&) = default;

    /**
     * BaseArgument move constructor.
     */
    BaseArgument(BaseArgument&&) = default;

    /**
     * BaseArgument destructor.
     */
    virtual ~BaseArgument() {
    }

    /**
     * BaseArgument assignment operator.
     */
    BaseArgument& operator=(const BaseArgument&) = default;

    /**
     * BaseArgument move assignment operator.
     */
    BaseArgument& operator=(BaseArgument&&) = default;

    /**
     * Collect all Argument instances contained in this argument into the given
     * vector.
     * @param collector Vector to store the arguments in
     */
    virtual void find_all_arguments(std::vector<const Argument*>& collector) const = 0;

    /**
     * Boolean conversion operator (so you can do things like if (arg) { ... } )
     * It is set to true if and only if the argument is set.
     * @return True iff the argument is set
     */
    explicit operator bool() const {
        return count() > 0;
    }


    /**
     * Return the actual count of occurrences.
     * @return Count of times this argument occurred
     */
    virtual unsigned int count() const = 0;

    ///////////////////
    // Test operations
    ///////////////////
    /**
     * Mark the argument as required or not.
     * @param required If true, the argument must be set at least once (see
     *        min())
     * @return Reference to this argument
     */
    BaseArgument& set_required(bool required = true) {
        m_required = required;
        return *this;
    }

    /**
     * Returns whether the argument is required.
     * @return True iff the argument is required
     */
    bool required() const {
        return m_required;
    }

    /**
     * Check if the argument constraints are satisfied. Returns if this is the
     * case, will throw an TAP::exception indicating the problem otherwise.
     */
    virtual void check_valid() const = 0;

    /**
     * Returns a string representing how the argument may be used on the command
     * line
     * @return The usage string
     */
    virtual std::string usage() const = 0;

    /**
     * Make a clone of the BaseArgument. Returns a pointer which is owned by
     * the caller.
     * @return Clone of the BaseArgument (or sub-class thereof)
     */
    virtual std::unique_ptr<BaseArgument> clone() const & = 0;

    /**
     * Make a clone of the BaseArgument and move all data to it. Returns a
     * pointer which is owned by the caller. The original object is no longer
     * valid.
     * @return Clone of the BaseArgument (or sub-class thereof)
     */
    virtual std::unique_ptr<BaseArgument> clone() && = 0;

};

}
