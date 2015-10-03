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
 * @file ArgumentConstraint.hpp
 * @brief Contains the definitions for ArgumentConstraints.
 */

#pragma once

#include <algorithm> // find
#include <memory>    // unique_ptr

namespace TAP {

/**
 * Enumeration of possible argument constraints. See the comments inside the
 * enumeration for details and meaning.
 */
enum class ConstraintType {
    None,   /**< 0:     No arguments may be set */
    One,    /**< 1:     Exactly one argument must be set */
    Any,    /**< [1,n]: At least one argument must be set */
    All,    /**< n:     All arguments must be set */
};

/**
 * Argument constraint class. Given a set of sub-arguments, the constraint will
 * check if the number of satisfied arguments fall within a certain range.
 * Sub-arguments are allowed to be ArgumentConstraint instances themselves,
 * thus allowing to nest constraints and build a tree.
 * Template parameter CType indicates the type of the constraint, for
 * possibilities see ConstraintType.
 */
template<ConstraintType CType>
class ArgumentConstraint: public BaseArgument {
protected:
    /** Sub-arguments to check */
    std::vector< std::unique_ptr<BaseArgument> > m_args;

    /** Stored usage string */
    std::string m_usageString;

public:
    /**
     * Construct a ArgumentConstraint with the (possibly empty) given list of
     * arguments.
     * @param args Arguments to add
     */
    template<typename ... A>
    ArgumentConstraint(A&& ... args);

    /**
     * ArgumentConstraint copy constructor.
     */
    ArgumentConstraint(const ArgumentConstraint& other) :
        BaseArgument(other), m_usageString(other.m_usageString) {
        for(auto const & arg: other.m_args) {
            m_args.emplace_back(std::move(arg->clone()));
        }
    }

    /**
     * ArgumentConstraint move constructor.
     */
    ArgumentConstraint(ArgumentConstraint&&) = default;

    /**
     * ArgumentConstraint destructor.
     */
    virtual ~ArgumentConstraint() {
    }

    /**
     * ArgumentConstraint assignment operator.
     */
    ArgumentConstraint& operator=(ArgumentConstraint other) {
        std::swap(m_args, other.m_args);
        std::swap(m_usageString, other.m_usageString);
        return *this;
    }

    /**
     * ArgumentConstraint move assignment operator.
     */
    ArgumentConstraint& operator=(ArgumentConstraint&&) = default;

    /**
     * Appends the given argument to the constraint
     * @param arg Argument to append
     * @return Reference to this constraint
     */
    ArgumentConstraint& operator+=(Argument& arg) {
        add(arg);
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
     * See BaseArgument::find_all_arguments()
     */
    void find_all_arguments(std::vector<const Argument*>& collector) const override {
        for(const auto& arg: m_args) {
            arg->find_all_arguments(collector);
        }
    }

    /**
     * See BaseArgument::set()
     */
    void check_valid() const override;

    /**
     * See BaseArgument::usage()
     */
    std::string usage() const override {
        return m_usageString;
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument> clone() const & override {
        return std::unique_ptr<BaseArgument>(new ArgumentConstraint(*this));
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument> clone() && override {
        return std::unique_ptr<BaseArgument>(new ArgumentConstraint(std::move(*this)));
    }

protected:
    /**
     * Add an argument to the constraint.
     * @param arg Argument to add
     * @return Reference to this ArgumentConstraint
     */
    template<typename Arg>
    ArgumentConstraint& add(Arg&& arg);

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
    std::string usageArgument(const Argument& arg) const;

    /**
     * Returns a string for usage representation of sub-arguments.
     * @return String for usage representation of sub-arguments
     */
    template<ConstraintType ACType>
    std::string usageArgument(const ArgumentConstraint<ACType>& arg) const;
};

/**
 * Simple set of arguments. Acts very much like ArgumentConstraint<All>, but does
 * not modify arguments on insertion. Designed to be used by the parser.
 */
class ArgumentSet : public ArgumentConstraint<ConstraintType::Any> {
protected:
    /** The name of this ArgumentSet */
    std::string m_name;

    /** Cached arguments */
    mutable std::vector<const Argument*> m_args;

    /** If true updates list of known arguments when retrieving them */
    bool m_updateArgs = true;
public:
    /**
     * Create a new named ArgumentSet. The parameter args defines the initial set of
     * arguments.
     * @param name (Descriptive) name of the argument set
     * @param args Arguments to add to the set initially
     */
    template<typename ... A>
    ArgumentSet(const std::string& name, A&& ... args);

    /**
     * ArgumentSet copy constructor.
     */
    ArgumentSet(const ArgumentSet&) = default;

    /**
     * ArgumentSet move constructor.
     */
    ArgumentSet(ArgumentSet&&) = default;

    /**
     * ArgumentSet destructor.
     */
    virtual ~ArgumentSet() {
    }

    /**
     * ArgumentSet assignment.
     */
    ArgumentSet& operator=(const ArgumentSet&) = default;

    /**
     * ArgumentSet move assignment.
     */
    ArgumentSet& operator=(ArgumentSet&&) = default;

    /**
     * Get the name of this ArgumentSet.
     * @return string containing the name of this ArgumentSet
     */
    const std::string& name() const {
        return m_name;
    }

    /**
     * Get the Arguments contained in this ArgumentSet. Duplicates are filtered
     * out.
     * @return Arguments contained in this ArgumentSet
     */
    const std::vector<const Argument*>& args() const {
        if (m_updateArgs) {
            m_args.clear();
            find_all_arguments(m_args);
        }
        return m_args;
    }

    /**
     * Add an argument to the set.
     * @param arg Argument to add
     * @return Reference to this set
     */
    template<typename Arg>
    ArgumentSet& add(Arg&& arg) {
        ArgumentConstraint<ConstraintType::Any>::add(std::forward<Arg>(arg));
        m_updateArgs = true;
        return *this;
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument> clone() const & override {
        return std::unique_ptr<BaseArgument>(new ArgumentSet(*this));
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument> clone() && override {
        return std::unique_ptr<BaseArgument>(new ArgumentSet(std::move(*this)));
    }
};

}
