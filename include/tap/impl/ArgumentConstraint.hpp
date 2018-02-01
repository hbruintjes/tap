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
 * Helper struct to define constraint traits for basic_argument_constraint.
 */
template<typename char_t, constraint_type CType>
struct ConstraintTraits {
    /** String used to concatenate usage string of sub-arguments */
    static std::basic_string<char_t> joinStr()  {
        return widen_const<char_t>(" ");
    }
};

/**
 * Helper struct to define constraint traits for basic_argument_constraint.
 * Specialized for constraint_type::One.
 */
template<typename char_t>
struct ConstraintTraits<char_t, constraint_type::One> {
    /** String used to concatenate usage string of sub-arguments */
    static std::basic_string<char_t> joinStr()  {
        return widen_const<char_t>(" | ");
    }
};

}

template<typename char_t, constraint_type CType>
template<typename ... A>
inline basic_argument_constraint<char_t, CType>::basic_argument_constraint(A&& ... arguments) :
    base_argument<char_t>(), m_usageString() {
    //static_assert(CType != constraint_type::One || sizeof...(arguments) > 1, "constraint_type::One needs at least two arguments");
    add(std::forward<A>(arguments)...);
}

template<typename char_t, constraint_type CType>
template<typename Arg>
inline basic_argument_constraint<char_t, CType>& basic_argument_constraint<char_t, CType>::add(Arg&& arg) {
    auto newArg = std::forward<Arg>(arg).clone();
    if (m_args.size() > 0) {
        m_usageString += std::basic_string<char_t>(detail::ConstraintTraits<char_t, CType>::joinStr());
    }

    m_usageString += usageArgument(static_cast<const Arg&>(*newArg));

    m_args.emplace_back(std::move(newArg));

    return *this;
}

template<typename char_t, constraint_type CType>
template<typename Arg, typename ... A>
inline basic_argument_constraint<char_t, CType>& basic_argument_constraint<char_t, CType>::add(Arg&& arg, A&& ... args) {
   add(arg);
   add(args...);

    return *this;
}

namespace detail {
    template<typename char_t, constraint_type CType>
    struct CheckValid;

    template<typename char_t>
    struct CheckValid<char_t, constraint_type::Imp> {
        static void check_valid(std::vector< std::unique_ptr<base_argument<char_t>> > const& m_args) {
            bool checkNext = false;
            base_argument<char_t>* lastArg = nullptr;
            for(auto const& arg: m_args) {
                if (arg->is_set()) {
                    arg->check_valid();
                }
                if (checkNext && !arg->is_set()) {
                    throw constraint_error("basic_argument " + lastArg->usage() + " requires ", std::vector<const base_argument<char_t>*>{arg.get()});
                }
                if (arg->is_set()) {
                    checkNext = true;
                }

                lastArg = arg.get();
            }
        }
    };

    template<typename char_t>
    struct CheckValid<char_t, constraint_type::One> {
        static void check_valid(std::vector< std::unique_ptr<base_argument<char_t>> > const& m_args) {
            unsigned int counter = 0;
            for(auto const& arg: m_args) {
                if (arg->is_set()) {
                    arg->check_valid();
                    ++counter;
                }
            }
            if (counter != 1) {
                std::vector<const base_argument<char_t>*> args;
                for(auto const& arg: m_args) {
                    // Copy to raw pointer vector
                    args.push_back(arg.get());
                }
                throw constraint_error("Must set exactly one basic_argument from ", args);
            }
        }
    };

    template<typename char_t>
    struct CheckValid<char_t, constraint_type::Any> {
        static void check_valid(std::vector< std::unique_ptr<base_argument<char_t>> > const& m_args) {
            std::vector<const base_argument<char_t>*> failed_args;
            for(auto const& arg: m_args) {
                if (arg->is_set() || arg->required()) {
                    arg->check_valid();
                }

                if (arg->required() && !arg->is_set()) {
                    failed_args.push_back(arg.get());
                }
            }

            if (failed_args.size() > 0) {
                throw constraint_error<char_t>(widen_const<char_t>("The following requirements are missing: "), failed_args);
            }
        }
    };
}

template<typename char_t, constraint_type CType>
inline void basic_argument_constraint<char_t, CType>::check_valid() const {
    detail::CheckValid<char_t, CType>::check_valid(m_args);
}

template<typename char_t, constraint_type CType>
inline unsigned int basic_argument_constraint<char_t, CType>::count() const {
    for (auto const& arg : m_args) {
        if (arg->is_set()) {
            return 1;
        }
    }
    return 0;
}

template<typename char_t, constraint_type CType>
inline void basic_argument_constraint<char_t, CType>::diagnose_args() const {
    for (auto const& arg : this->m_args) {
        arg->check_valid();
    }
}

template<typename char_t, constraint_type CType>
inline std::basic_string<char_t> basic_argument_constraint<char_t, CType>::usageArgument(const basic_argument<char_t>& arg) const {
    if (!arg.required() && (CType == constraint_type::Any)) {
        return widen_const<char_t>("[ ") + arg.usage() + widen_const<char_t>(" ]");
    } else {
        return arg.usage();
    }
}

template<typename char_t, constraint_type CType>
template<constraint_type ACType>
inline std::basic_string<char_t> basic_argument_constraint<char_t, CType>::usageArgument(const basic_argument_constraint<char_t, ACType>& arg) const {
    bool paren = (
            (CType == constraint_type::One) ||
            (CType == constraint_type::Any && ACType != constraint_type::Any) ||
            (ACType == constraint_type::One)
        );
    if (!arg.required() && (CType == constraint_type::Any && ACType != constraint_type::Any)) {
        return widen_const<char_t>("[ ") + arg.usage() + widen_const<char_t>(" ]");
    }
    if (paren && arg.size() > 0) {
        return widen_const<char_t>("( ") + arg.usage() + widen_const<char_t>(" )");
    } else {
        return arg.usage();
    }
}

template<typename char_t>
template<typename ... A>
basic_argument_set<char_t>::basic_argument_set(const std::basic_string<char_t>& name, A&& ... args) :
    basic_argument_constraint<char_t, constraint_type::Any>(), m_name(name) {
    this->set_required(false);
    add(std::forward<A>(args)...);
}

}
