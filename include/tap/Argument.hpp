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
 * @file basic_argument.hpp
 *
 * @brief Contains the definitions for regular arguments.
 */

#pragma once

#include <algorithm>
#include <functional>
#include <memory>

namespace TAP {

template<typename char_t>
class basic_argument;

/** Function pointer that is used by basic_argument::check(). */
template<typename char_t>
using argument_check_func = std::function<void(const basic_argument<char_t>&)>;

/**
 * Simple basic_argument class. arguments are identified by a flag ('-a') or name
 * ('--alpha'), or can be positional (only if they accept a value, see
 * value_argument).
 * An basic_argument by default is optional (as they normally should be for most
 * applications, makes them easier to use), but has functions to mark it
 * required.
 * By default an basic_argument is allowed to occur only once, but can be set to occur
 * an arbitrary amount of times.
 */
template<typename char_t>
class basic_argument: public base_argument<char_t> {
protected:
    /** Flags this arguments matches */
    std::basic_string<char_t> m_flags;
    /** Names this basic_argument matches */
    std::vector<std::basic_string<char_t>> m_names;

    /** True if used as positional basic_argument */
    bool m_isPositional;

    /** Description or help text of basic_argument */
    std::basic_string<char_t> m_description;

    /** Minimum number of occurrences if basic_argument is set */
    unsigned int m_min = 1;
    /** Maximum number of occurrences. 0 means no limit */
    unsigned int m_max = 1;

    /** Actual counted number of occurrences */
    mutable std::shared_ptr<unsigned int> m_count;

    /** Callback for check */
    argument_check_func<char_t> m_checkFunc = nullptr;

#ifndef TAP_AUTOFLAG
    /**
     * Create a positional basic_argument (has no name). This function is only valid
     * for subclasses that accept values, such as value_argument.
     * @param description Description of the basic_argument (used in help text)
     */
    basic_argument(std::basic_string<char_t> description) :
            m_isPositional(true), m_description(std::move(description)), m_count(std::make_shared<unsigned int>(0)) {
    }
#endif

public:
    ///////////////////
    // Creation/definition
    ///////////////////
#ifdef TAP_AUTOFLAG
    /**
     * Create an basic_argument with flag and/or name set by description. Using this
     * constructing with a description without a flag or name is invalid.
     * @param description Description of the basic_argument, requires a flag or name
     *        to be defined
     */
	basic_argument(std::basic_string<char_t> description) :
            m_isPositional(true), m_description(std::move(description)), m_count(std::make_shared<unsigned int>(0)) {

        parse_description();
        // Cannot perform below check as it requires virtual calls
        // instead, user will have to make sure this does not happen
        /*if (!takes_value() && m_isPositional) {
            throw std::logic_error("Cannot create positional basic_argument without value");
        }*/
    }
#endif

    /**
     * Create an basic_argument that is identified by a flag.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     */
	basic_argument(std::basic_string<char_t> description, char_t flag) :
            m_isPositional(false), m_description(std::move(description)), m_count(std::make_shared<unsigned int>(0)) {
#ifdef TAP_AUTOFLAG
        parse_description();
#endif
        alias(flag);
    }

    /**
     * Create an basic_argument that is identified by a name.
     * @param description Description of the basic_argument (used in help text)
     * @param name Name identifier of this basic_argument
     */
	basic_argument(std::basic_string<char_t> description, std::basic_string<char_t> name) :
            m_isPositional(false), m_description(std::move(description)), m_count(std::make_shared<unsigned int>(0)) {
#ifdef TAP_AUTOFLAG
        parse_description();
#endif
        alias(std::move(name));
    }

    /**
     * Create an basic_argument that is identified by both a name and flag.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param name Name identifier of this basic_argument
     */
	basic_argument(std::basic_string<char_t> description, char_t flag, std::basic_string<char_t> name) :
            m_isPositional(false), m_description(std::move(description)), m_count(std::make_shared<unsigned int>(0)) {
#ifdef TAP_AUTOFLAG
        parse_description();
#endif
        alias(flag, std::move(name));
    }

    /**
     * basic_argument copy constructor.
     */
    basic_argument(const basic_argument&) = default;

    /**
     * basic_argument move constructor.
     */
    basic_argument(basic_argument&&) = default;

    /**
     * basic_argument destructor.
     */
    virtual ~basic_argument() {
    }

    /**
     * basic_argument assignment operator.
     */
    basic_argument& operator=(const basic_argument& other) = default;

    /**
     * basic_argument move assignment operator.
     */
    basic_argument& operator=(basic_argument&& other) = default;

    /**
     * An a flag alias for the basic_argument.
     * @param flag Flag alias of basic_argument
     * @return Reference to this basic_argument
     */
    basic_argument& alias(char_t flag) {
        m_flags.insert(m_flags.end(), flag);
        return *this;
    }

    /**
     * An a name alias for the basic_argument.
     * @param name Name alias of basic_argument
     * @return Reference to this basic_argument
     */
    basic_argument& alias(std::basic_string<char_t> name) {
        m_names.push_back(std::move(name));
        return *this;
    }

    /**
     * An both a flag and name alias for the basic_argument.
     * @param flag Flag alias of basic_argument
     * @param name Name alias of basic_argument
     * @return Reference to this basic_argument
     */
    basic_argument& alias(char_t flag, std::basic_string<char_t> name) {
        m_flags.insert(m_flags.end(), flag);
        m_names.push_back(std::move(name));
        return *this;
    }

    /**
     * Returns the description of this basic_argument. See also description(string).
     * @return basic_argument description
     */
    const std::basic_string<char_t>& description() const {
        return m_description;
    }

    /** Set the check function to use.
     * @param checkFunc Check function to use (see argument_check_func)
     * @return Reference to this basic_argument
     */
    virtual basic_argument<char_t>& check(argument_check_func<char_t> checkFunc) {
        m_checkFunc = checkFunc;
        return *this;
    }

    ///////////////////
    // Lookup operations
    ///////////////////
    /**
     * Returns a pointer to this basic_argument if it matches as a positional
     * basic_argument, otherwise a null-pointer.
     * @return Either a pointer to this basic_argument if positional, otherwise
     *         nullptr.
     */
    bool matches() const {
        if (m_isPositional) {
            return true;
        }
        return false;
    }

    /**
     * Returns a pointer to this basic_argument if it matches the given flag with any
     * alias, otherwise a null-pointer.
     * @param flag Flag to check
     * @return Either a pointer to this basic_argument if positional, otherwise
     *         nullptr.
     */
    bool matches(char_t flag) const {
        for (char_t self_flag : m_flags) {
            if (self_flag == flag) {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns a pointer to this basic_argument if it matches the given name with any
     * alias, otherwise a null-pointer.
     * @param name Name to check
     * @return Either a pointer to this basic_argument if positional, otherwise
     *         nullptr.
     */
    bool matches(const std::basic_string<char_t>& name) const {
        for (const std::basic_string<char_t>& self_name : m_names) {
            if (self_name == name) {
                return true;
            }
        }
        return false;
    }

    /**
     * See base_argument::find_all_arguments()
     */
    void find_all_arguments(std::vector<const basic_argument*>& collector) const override {
        collector.push_back(this);
    }

    // Make public
    using base_argument<char_t>::set_required;
    using base_argument<char_t>::required;

    ///////////////////
    // Count operations
    ///////////////////
    /**
     * Allow the basic_argument to occur multiple times or not. See also max()
     * @param many If true, set the maximum number of occurrences to infinite,
     *        otherwise the current limit or 1.
     * @return Reference to this basic_argument
     */
    basic_argument& many(bool many = true) {
        if (!many) {
            // Note: max of N (!=0) does not mean many
            m_max = std::max(m_max, 1u);
        } else {
            m_max = 0u;
        }
        return *this;
    }

    /*bool many() const {
        return m_max != 1;
    }*/

    /**
     * Set the minimum number of required occurrences if set. If this value is
     * higher than the current maximum, the maximum is set to the same value.
     * @param min Minimum number of occurrences, must be >0
     * @return Reference to this basic_argument
     */
    basic_argument& min(unsigned int min) {
        if (min == 0) {
            throw std::logic_error("Cannot set zero minimum");
        }
        m_min = min;
        if (min > m_max && m_max != 0u) {
            m_max = m_min;
        }
        return *this;
    }

    /**
     * Returns the minimum number of occurrences of this basic_argument for it to be
     * satisfied if set.
     * @return Minimum number of occurrences
     */
    unsigned int min() const {
        return m_min;
    }

    /**
     * Set the maximum number of required occurrences. This value may be 0 to
     * indicate unbounded.
     * @param max Maximum number of occurrences
     * @return Reference to this basic_argument
     */
    basic_argument& max(unsigned int max) {
        m_max = max;
        return *this;
    }

    /**
     * Returns the maximum number of occurrences of this basic_argument.
     * @return Maximum number of occurrences
     */
    unsigned int max() const {
        return m_max;
    }

    ///////////////////
    // value operations
    ///////////////////
    /**
     * See base_argument::count()
     */
    unsigned int count() const override {
        return *m_count;
    }

    /**
     * Returns whether this basic_argument is still allowed to occur on the command
     * line.
     * @return True iff the basic_argument is still allowed to occur
     */
    bool can_set() const {
        return (m_max == 0 || *m_count < m_max);
    }

    /**
     * Set the basic_argument (mark as occurred).
     */
    virtual void set() const {
        (*m_count)++;
        check();
    }

    /**
     * Returns whether or not this basic_argument requires a value when it occurs
     * (see also set()).
     * @return True iff the basic_argument requires a value when set
     */
    virtual bool takes_value() const {
        // Note: not the same as setCount < max, it is a fixed property
        return false;
    }

    ///////////////////
    // Validation operations
    ///////////////////
    /**
     * See base_argument::check_valid()
     */
    void check_valid() const override;

    /**
     * See base_argument::usage()
     */
    std::basic_string<char_t> usage() const override;

    /**
     * Print a string representation of this basic_argument to the given stream. This
     * is usually represented in the first column of help text.
     * @return String representation.
     */
    virtual std::basic_string<char_t> ident() const;

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() const & override {
        return std::unique_ptr<base_argument<char_t>>(new basic_argument(*this));
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() && override {
        return std::unique_ptr<base_argument<char_t>>(new basic_argument(std::move(*this)));
    }
protected:
    /**
     * Executes the associated check function.
     * @return Result of the check function, or true if not set
     */
    virtual void check() const {
        if (m_checkFunc != nullptr) {
            m_checkFunc(*this);
        }
    }

private:
#ifdef TAP_AUTOFLAG
    /**
     * When TAP_AUTOFLAG is defined, this function finds flag and/or name
     * markers in the description of the basic_argument, and uses these to set
     * aliases. Names are delimited by word boundaries, which is any
     * non-alphanumeric character. Empty names are ignored. Markers:
     * *  % : Next character is a flag
     * *  $ : Next word is a name
     * *  & : Next character is a flag, and next word is a name
     *
     * For example, the description
     *       "Show this &help text"
     * will set a flag 'h' and a name "help".
     */
    void parse_description() {
        bool escape = false;
        bool addFlag = false;
        bool addName = false;
        auto nameStart = m_description.begin();
        std::vector<typename std::basic_string<char_t>::iterator> specialChars;
        for (auto it = m_description.begin(); it != m_description.end(); ++it) {
            char_t c = *it;
            bool skip = false;

            if (addFlag) {
                alias(c);
                addFlag = false;
                m_isPositional = false;
                skip = true;
            }

            if (!isalnum(c) && addName) {
                // Check if valid character for a name
                if (it != nameStart) {
                    alias( std::basic_string<char_t>(nameStart, it) );
                    m_isPositional = false;
                }
                addName = false;
            }

            if (skip) {
                continue;
            }

            switch(c) {
                case '\\':
                    escape = !escape;
                    if (!escape) {
                        specialChars.push_back(it);
                    }
                    break;
                case '%':
                    // Treat next word as flag
                    if (escape) {
                        escape = false;
                        specialChars.push_back(it-1);
                        continue;
                    } else {
                        addFlag = true;
                        specialChars.push_back(it);
                    }
                    break;
                case '$':
                    // Treat next word as name
                    if (escape) {
                        escape = false;
                        specialChars.push_back(it-1);
                        continue;
                    } else {
                        addName = true;
                        nameStart = it+1;
                        specialChars.push_back(it);
                    }
                    break;
                case '&':
                    // Treat next word as flag and name
                    if (escape) {
                        escape = false;
                        specialChars.push_back(it-1);
                        continue;
                    } else {
                        addFlag = true;
                        addName = true;
                        nameStart = it+1;
                        specialChars.push_back(it);
                    }
                    break;
                default:
                    break;
            }
        }
        if (addName && nameStart != m_description.end()) {
            alias(std::basic_string<char_t>(nameStart, m_description.end()));
        }
        // Remove the special characters
        for (auto it = specialChars.rbegin(); it != specialChars.rend(); it++) {
            m_description.erase(*it, (*it)+1);
        }
    }
#endif
};

/**
 * Interface class for arguments that accept a value when takes_value() is true.
 */
template<typename char_t>
class value_acceptor {
protected:
    ~value_acceptor() {}
public:
    /**
     * Set the basic_argument (mark as occurred), and assign a value to it. The
     * parameter is a string representing the value. See also basic_argument::set()
     * @param value The value to set, as a string
     */
    virtual void set(const std::basic_string<char_t>& value) const = 0;
};

}
