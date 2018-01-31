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
 * @file basic_argument_constraint.hpp
 * @brief Contains the definitions for basic_argument_constraints.
 */

#pragma once

#include <algorithm> // find
#include <memory>    // unique_ptr

namespace TAP {

/**
 * Enumeration of possible basic_argument constraints. See the comments inside the
 * enumeration for details and meaning.
 */
enum class constraint_type {
    Imp,   /**< 0:    Each basic_argument implies the next */
    One,   /**< 1:    Exactly one basic_argument must be set */
    Any,   /**< ?:    Manually specified */
};

/**
 * basic_argument constraint class. Given a set of sub-arguments, the constraint will
 * check if the number of satisfied arguments fall within a certain range.
 * Sub-arguments are allowed to be basic_argument_constraint instances themselves,
 * thus allowing to nest constraints and build a tree.
 * Template parameter CType indicates the type of the constraint, for
 * possibilities see constraint_type.
 */
template<typename char_t, constraint_type CType>
class basic_argument_constraint: public base_argument<char_t> {
protected:
    /** Sub-arguments to check */
    std::vector< std::unique_ptr<base_argument<char_t>> > m_args;

    /** Stored usage string */
    std::basic_string<char_t> m_usageString;

public:
    /**
     * Construct a basic_argument_constraint with the (possibly empty) given list of
     * arguments.
     * @param args arguments to add
     */
    template<typename ... A>
    basic_argument_constraint(A&& ... args);

    /**
     * basic_argument_constraint copy constructor.
     */
    basic_argument_constraint(const basic_argument_constraint& other) :
        base_argument<char_t>(other), m_usageString(other.m_usageString) {
        for(auto const & arg: other.m_args) {
            m_args.emplace_back(std::move(arg->clone()));
        }
    }

    /**
     * basic_argument_constraint move constructor.
     */
    basic_argument_constraint(basic_argument_constraint&&) = default;

    /**
     * basic_argument_constraint destructor.
     */
    virtual ~basic_argument_constraint() {
    }

    /**
     * basic_argument_constraint assignment operator.
     */
    basic_argument_constraint& operator=(basic_argument_constraint other) {
        using std::swap;
        swap(m_args, other.m_args);
        swap(m_usageString, other.m_usageString);
        return *this;
    }

    /**
     * basic_argument_constraint move assignment operator.
     */
    basic_argument_constraint& operator=(basic_argument_constraint&&) = default;

    /**
     * Appends the given basic_argument to the constraint
     * @param arg basic_argument to append
     * @return Reference to this constraint
     */
    basic_argument_constraint& operator+=(basic_argument<char_t>& arg) {
        add(arg);
        return *this;
    }

    /**
     * Appends the given basic_argument to the constraint
     * @param arg basic_argument to append
     * @return Reference to this constraint
     */
    basic_argument_constraint& operator+=(basic_argument_constraint<char_t, CType> const& other) {
        for(auto const & arg: other.m_args) {
            m_args.emplace_back(std::move(arg->clone()));
        }
        return *this;
    }

    /**
     * Returns the number of contained sub-arguments.
     * @return Number of contained sub-arguments.
     */
    size_t size() const {
        return m_args.size();
    }

    /**
     * See base_argument::find_all_arguments()
     */
    void find_all_arguments(std::vector<const basic_argument<char_t>*>& collector) const override {
        for(const auto& arg: m_args) {
            arg->find_all_arguments(collector);
        }
    }

    /**
     * See base_argument::check_valid()
     */
    void check_valid() const override;

    /**
     * See base_argument::usage()
     */
    std::basic_string<char_t> usage() const override {
        return m_usageString;
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() const & override {
        return std::unique_ptr<base_argument<char_t>>(new basic_argument_constraint(*this));
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() && override {
        return std::unique_ptr<base_argument<char_t>>(new basic_argument_constraint(std::move(*this)));
    }

protected:
    /**
     * Add no arguments to the constraint. This is a simple placeholder for
     * iterating over vararg templates.
     * @return Reference to this basic_argument_constraint
     */
    basic_argument_constraint& add() {
        return *this;
    }

    /**
     * Add an basic_argument to the constraint.
     * @param arg basic_argument to add
     * @return Reference to this basic_argument_constraint
     */
    template<typename Arg>
    basic_argument_constraint& add(Arg&& arg);

    /**
     * Add multiple arguments to the constraint.
     * @param arg basic_argument to add
     * @param args Other arguments to add
     * @return Reference to this basic_argument_constraint
     */
    template<typename Arg, typename ... A>
    basic_argument_constraint& add(Arg&& arg, A&& ... args);

    /**
     * Returns the number of child arguments which are set.
     */
    unsigned int count() const override;

    /**
     * Calls diagnose for all set child arguments.
     */
    void diagnose_args() const;

    /**
     * Returns a string for usage representation of sub-arguments.
     * @return String for usage representation of sub-arguments
     */
    std::basic_string<char_t> usageArgument(const basic_argument<char_t>& arg) const;

    /**
     * Returns a string for usage representation of sub-arguments.
     * @return String for usage representation of sub-arguments
     */
    template<constraint_type ACType>
    std::basic_string<char_t> usageArgument(const basic_argument_constraint<char_t, ACType>& arg) const;
};

/**
 * Simple set of arguments. Acts very much like basic_argument_constraint<Any>, but does
 * not modify arguments on insertion. Designed to be used by the parser.
 */
template<typename char_t>
class basic_argument_set : public basic_argument_constraint<char_t, constraint_type::Any> {
protected:
    /** The name of this basic_argument_set */
    std::basic_string<char_t> m_name;

    /** Cached arguments */
    mutable std::vector<const basic_argument<char_t>*> m_args;

    /** If true updates list of known arguments when retrieving them */
    bool m_updateArgs = true;
public:
    /**
     * Create a new named basic_argument_set. The parameter args defines the initial set of
     * arguments.
     * @param name (Descriptive) name of the basic_argument set
     * @param args arguments to add to the set initially
     */
    template<typename ... A>
    basic_argument_set(const std::basic_string<char_t>& name, A&& ... args);

    /**
     * basic_argument_set copy constructor.
     */
    basic_argument_set(const basic_argument_set&) = default;

    /**
     * basic_argument_set move constructor.
     */
    basic_argument_set(basic_argument_set&&) = default;

    /**
     * basic_argument_set destructor.
     */
    virtual ~basic_argument_set() {
    }

    /**
     * basic_argument_set assignment.
     */
    basic_argument_set& operator=(const basic_argument_set&) = default;

    /**
     * basic_argument_set move assignment.
     */
    basic_argument_set& operator=(basic_argument_set&&) = default;

    /**
     * Get the name of this basic_argument_set.
     * @return string containing the name of this basic_argument_set
     */
    const std::basic_string<char_t>& name() const {
        return m_name;
    }

    /**
     * Get the arguments contained in this basic_argument_set. Duplicates are filtered
     * out.
     * @return arguments contained in this basic_argument_set
     */
    const std::vector<const basic_argument<char_t>*>& args() const {
        if (m_updateArgs) {
            m_args.clear();
            this->find_all_arguments(m_args);
        }
        return m_args;
    }

    /**
     * Add arguments to the set.
     * @param args arguments to add
     * @return Reference to this set
     */
    template<typename ... A>
    basic_argument_set& add(A&& ... args) {
        basic_argument_constraint<char_t, constraint_type::Any>::add(std::forward<A>(args)...);
        m_updateArgs = true;
        return *this;
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() const & override {
        return std::unique_ptr<base_argument<char_t>>(new basic_argument_set(*this));
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() && override {
        return std::unique_ptr<base_argument<char_t>>(new basic_argument_set(std::move(*this)));
    }
};

}
