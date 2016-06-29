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
    add(std::forward<A>(arguments)...);
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

template<ConstraintType CType>
template<typename Arg, typename ... A>
inline ArgumentConstraint<CType>& ArgumentConstraint<CType>::add(Arg&& arg, A&& ... args) {
   add(arg);
   add(args...);

    return *this;
}

template<>
inline void ArgumentConstraint<ConstraintType::Imp>::check_valid() const {
    bool checkNext = false;
    BaseArgument* lastArg = nullptr;
    for(auto const& arg: m_args) {
        if (arg->is_set()) {
            arg->check_valid();
        }
        if (checkNext && !arg->is_set()) {
            throw constraint_error("Argument " + lastArg->usage() + " requires ", std::vector<const BaseArgument*>{arg.get()});
        }
        if (arg->is_set()) {
            checkNext = true;
        }

        lastArg = arg.get();
    }
}

template<>
inline void ArgumentConstraint<ConstraintType::One>::check_valid() const {
    unsigned int counter = 0;
    for(auto const& arg: m_args) {
        if (arg->is_set()) {
            arg->check_valid();
            ++counter;
        }
    }
    if (counter != 1) {
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
    std::vector<const BaseArgument*> failed_args;
    for(auto const& arg: m_args) {
        if (arg->is_set() || arg->required()) {
            arg->check_valid();
        }

        if (arg->required() && !arg->is_set()) {
            failed_args.push_back(arg.get());
        }
    }

    if (failed_args.size() > 0) {
        throw constraint_error("The following requirements are missing: ", failed_args);
    }
}

template<ConstraintType CType>
inline unsigned int ArgumentConstraint<CType>::count() const {
    for (auto const& arg : m_args) {
        if (arg->is_set()) {
            return 1;
        }
    }
    return 0;
}

template<ConstraintType CType>
inline void ArgumentConstraint<CType>::diagnose_args() const {
    for (auto const& arg : m_args) {
        arg->check_valid();
    }
}

template<ConstraintType CType>
inline std::string ArgumentConstraint<CType>::usageArgument(const Argument& arg) const {
    if (!arg.required() && (CType == ConstraintType::Any)) {
        return "[ " + arg.usage() + " ]";
    } else {
        return arg.usage();
    }
}

template<ConstraintType CType>
template<ConstraintType ACType>
inline std::string ArgumentConstraint<CType>::usageArgument(const ArgumentConstraint<ACType>& arg) const {
    bool paren = (
            (CType == ConstraintType::One) ||
            (CType == ConstraintType::Any && ACType != ConstraintType::Any) ||
            (ACType == ConstraintType::One)
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

template<typename ... A>
ArgumentSet::ArgumentSet(const std::string& name, A&& ... args) :
    ArgumentConstraint<ConstraintType::Any>(), m_name(name) {
    set_required(false);
    add(std::forward<A>(args)...);
}

}
