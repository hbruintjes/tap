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
template<typename char_t, ConstraintType CType>
struct ConstraintTraits {
    /** String used to concatenate usage string of sub-arguments */
    static constexpr const char_t* joinStr()  {
        return " ";
    }
};

/**
 * Helper struct to define constraint traits for ArgumentConstraint.
 * Specialized for ConstraintType::One.
 */
template<typename char_t>
struct ConstraintTraits<char_t, ConstraintType::One> {
    /** String used to concatenate usage string of sub-arguments */
    static constexpr const char_t* joinStr()  {
        return " | ";
    }
};

}

template<typename char_t, ConstraintType CType>
template<typename ... A>
inline ArgumentConstraint<char_t, CType>::ArgumentConstraint(A&& ... arguments) :
    BaseArgument<char_t>(), m_usageString() {
    //static_assert(CType != ConstraintType::One || sizeof...(arguments) > 1, "ConstraintType::One needs at least two arguments");
    add(std::forward<A>(arguments)...);
}

template<typename char_t, ConstraintType CType>
template<typename Arg>
inline ArgumentConstraint<char_t, CType>& ArgumentConstraint<char_t, CType>::add(Arg&& arg) {
    auto newArg = std::forward<Arg>(arg).clone();
    if (m_args.size() > 0) {
        m_usageString += std::basic_string<char_t>(detail::ConstraintTraits<char_t, CType>::joinStr());
    }

    m_usageString += usageArgument(static_cast<const Arg&>(*newArg));

    m_args.emplace_back(std::move(newArg));

    return *this;
}

template<typename char_t, ConstraintType CType>
template<typename Arg, typename ... A>
inline ArgumentConstraint<char_t, CType>& ArgumentConstraint<char_t, CType>::add(Arg&& arg, A&& ... args) {
   add(arg);
   add(args...);

    return *this;
}

namespace detail {
    template<typename char_t, ConstraintType CType>
    struct CheckValid;

    template<typename char_t>
    struct CheckValid<char_t, ConstraintType::Imp> {
        static void check_valid(std::vector< std::unique_ptr<BaseArgument<char_t>> > const& m_args) {
            bool checkNext = false;
            BaseArgument<char_t>* lastArg = nullptr;
            for(auto const& arg: m_args) {
                if (arg->is_set()) {
                    arg->check_valid();
                }
                if (checkNext && !arg->is_set()) {
                    throw constraint_error("Argument " + lastArg->usage() + " requires ", std::vector<const BaseArgument<char_t>*>{arg.get()});
                }
                if (arg->is_set()) {
                    checkNext = true;
                }

                lastArg = arg.get();
            }
        }
    };

    template<typename char_t>
    struct CheckValid<char_t, ConstraintType::One> {
        static void check_valid(std::vector< std::unique_ptr<BaseArgument<char_t>> > const& m_args) {
            unsigned int counter = 0;
            for(auto const& arg: m_args) {
                if (arg->is_set()) {
                    arg->check_valid();
                    ++counter;
                }
            }
            if (counter != 1) {
                std::vector<const BaseArgument<char_t>*> args;
                for(auto const& arg: m_args) {
                    // Copy to raw pointer vector
                    args.push_back(arg.get());
                }
                throw constraint_error("Must set exactly one argument from ", args);
            }
        }
    };

    template<typename char_t>
    struct CheckValid<char_t, ConstraintType::Any> {
        static void check_valid(std::vector< std::unique_ptr<BaseArgument<char_t>> > const& m_args) {
            std::vector<const BaseArgument<char_t>*> failed_args;
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
    };
}

template<typename char_t, ConstraintType CType>
inline void ArgumentConstraint<char_t, CType>::check_valid() const {
    detail::CheckValid<char_t, CType>::check_valid(m_args);
}

template<typename char_t, ConstraintType CType>
inline unsigned int ArgumentConstraint<char_t, CType>::count() const {
    for (auto const& arg : m_args) {
        if (arg->is_set()) {
            return 1;
        }
    }
    return 0;
}

template<typename char_t, ConstraintType CType>
inline void ArgumentConstraint<char_t, CType>::diagnose_args() const {
    for (auto const& arg : this->m_args) {
        arg->check_valid();
    }
}

template<typename char_t, ConstraintType CType>
inline std::basic_string<char_t> ArgumentConstraint<char_t, CType>::usageArgument(const Argument<char_t>& arg) const {
    if (!arg.required() && (CType == ConstraintType::Any)) {
        return "[ " + arg.usage() + " ]";
    } else {
        return arg.usage();
    }
}

template<typename char_t, ConstraintType CType>
template<ConstraintType ACType>
inline std::basic_string<char_t> ArgumentConstraint<char_t, CType>::usageArgument(const ArgumentConstraint<char_t, ACType>& arg) const {
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

template<typename char_t>
template<typename ... A>
ArgumentSet<char_t>::ArgumentSet(const std::basic_string<char_t>& name, A&& ... args) :
    ArgumentConstraint<char_t, ConstraintType::Any>(), m_name(name) {
    this->set_required(false);
    add(std::forward<A>(args)...);
}

}
