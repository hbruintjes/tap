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

#pragma once

namespace TAP {

namespace detail {
/** Internal type alias used by the constraints constructor */
template<class T> using Temporary = T;

/**
 * Helper struct to define constraint traits for ArgumentConstraint.
 */
template<ConstraintType CType>
struct ConstraintTraits {
    /** String used to concatenate usage string of sub-arguments */
    static constexpr const char* joinStr()  {
        return " ";
    }
};

/**
 * Helper struct to define constraint traits for ArgumentConstraint.
 * Specialized for ConstraintType::One.
 */
template<>
struct ConstraintTraits<ConstraintType::One> {
    /** String used to concatenate usage string of sub-arguments */
    static constexpr const char* joinStr()  {
        return " | ";
    }
};

}

template<ConstraintType CType>
template<typename ... A>
inline ArgumentConstraint<CType>::ArgumentConstraint(A&& ... arguments) :
    BaseArgument(), m_usageString() {
    //static_assert(CType != ConstraintType::One || sizeof...(arguments) > 1, "ConstraintType::One needs at least two arguments");
    // See http://stackoverflow.com/questions/13978916/inserting-a-variadic-argument-list-into-a-vector
    // build temporary char array with push_back side-effect
    detail::Temporary<char[]> { (
            add(std::forward<A>(arguments))
            ,'0')..., '0' };
}

template<ConstraintType CType>
template<typename Arg>
inline ArgumentConstraint<CType>& ArgumentConstraint<CType>::add(Arg&& arg) {
    auto newArg = std::forward<Arg>(arg).clone();
    if (m_args.size() > 0) {
        m_usageString += std::string(detail::ConstraintTraits<CType>::joinStr());
    }

    m_usageString += usageArgument(static_cast<const Arg&>(*newArg));

    m_args.emplace_back(std::move(newArg));

    return *this;
}

template<>
inline void ArgumentConstraint<ConstraintType::None>::check_valid() const {
    std::vector<const BaseArgument*> failedArgs;
    for(auto const& arg: m_args) {
        arg->check_valid();
        if (arg->count() > 0) {
            failedArgs.push_back(arg.get());
        }
    }
    if (failedArgs.size() == 1) {
        throw constraint_error("Cannot set the argument ", failedArgs);
    } else if (failedArgs.size() > 1) {
        throw constraint_error("Not allowed to set the following arguments: ", failedArgs);
    }
}

template<>
inline void ArgumentConstraint<ConstraintType::One>::check_valid() const {
    unsigned int counter = 0;
    for(auto const& arg: m_args) {
        arg->check_valid();
        if (arg->count() > 0) {
            ++counter;
        }
    }
    if (counter > 1 || (counter == 0 && required())) {
        std::vector<const BaseArgument*> args;
        for(auto const& arg: m_args) {
            // Copy to raw pointer vector
            args.push_back(arg.get());
        }
        throw constraint_error("Must set exactly one argument from ", args);
    }
}

template<>
inline void ArgumentConstraint<ConstraintType::Any>::check_valid() const {
    unsigned int counter = 0;
    for(auto const& arg: m_args) {
        arg->check_valid();
        if (arg->count() > 0) {
            ++counter;
        }
    }

    if (counter == 0 && required()) {
        std::vector<const BaseArgument*> args;
        for(auto const& arg: m_args) {
            // Copy to raw pointer vector
            args.push_back(arg.get());
        }
        throw constraint_error("At least one of the following arguments must be set ", args);
    }
}

template<>
inline void ArgumentConstraint<ConstraintType::All>::check_valid() const {
    std::vector<const BaseArgument*> failedArgs;
    unsigned int counter = 0;
    for(auto const& arg: m_args) {
        arg->check_valid();
        if (arg->count() > 0) {
            ++counter;
        } else {
            failedArgs.push_back(arg.get());
        }
    }
    if (counter < size() && (counter != 0 || required())) {
        throw constraint_error("The following arguments are missing ", failedArgs);
    }
}

template<ConstraintType CType>
inline unsigned int ArgumentConstraint<CType>::count() const {
    unsigned int count = 0;
    for (auto const& arg : m_args) {
        if (arg->count() > 0) {
            count++;
        }
    }
    return count;
}

template<ConstraintType CType>
inline void ArgumentConstraint<CType>::diagnose_args() const {
    for (auto const& arg : m_args) {
        arg->check_valid();
    }
}

template<ConstraintType CType>
inline std::string ArgumentConstraint<CType>::usageArgument(const Argument& arg) const {
    if (CType == ConstraintType::None) {
        return std::string("!") + arg.usage();
    } else if (!arg.required() && (CType == ConstraintType::Any)) {
        return "[ " + arg.usage() + " ]";
    } else {
        return arg.usage();
    }
}

template<ConstraintType CType>
template<ConstraintType ACType>
inline std::string ArgumentConstraint<CType>::usageArgument(const ArgumentConstraint<ACType>& arg) const {
    if (CType == ConstraintType::None) {
        if (arg.size() > 0) {
            return "!( " + arg.usage() + " )";
        } else {
            return "!" + arg.usage();
        }
    } else {
        bool paren = (
                (CType == ConstraintType::One) ||
                (CType == ConstraintType::Any && ACType != ConstraintType::Any) ||
                (CType == ConstraintType::All && ACType == ConstraintType::One)
            );
        if (!arg.required() && (CType == ConstraintType::Any && ACType != ConstraintType::Any)) {
            return "[ " + arg.usage() + " ]";
        }
        if (paren && arg.size() > 0) {
            return "( " + arg.usage() + " )";
        } else {
            return arg.usage();
        }
    }
}

template<typename ... A>
ArgumentSet::ArgumentSet(const std::string& name, A&& ... args) :
    ArgumentConstraint<ConstraintType::Any>(), m_name(name) {
    set_required(false);
    // See http://stackoverflow.com/questions/13978916/inserting-a-variadic-argument-list-into-a-vector
    // build temporary char array with push_back side-effect
    detail::Temporary<char[]> { (
            add(std::forward<A>(args))
            ,'0')..., '0' };
}

}
