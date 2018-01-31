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

#include <cstring>   // strlen
#include <algorithm> // min/max

namespace TAP {

template<typename char_t>
template<typename... Args>
inline ArgumentParser<char_t>::ArgumentParser(Args&&... args) :
    m_constraints("Constraints")
{
    m_argSets.emplace_back("Arguments", args...);
}

template<typename char_t>
template<typename... Args>
inline ArgumentParser<char_t>& ArgumentParser<char_t>::addAll(Args&&... args) {
    detail::Temporary<char[]> { (
            add(std::forward<Args>(args))
            ,'0')..., '0' };
    return *this;
}

template<typename char_t>
template<typename Arg>
inline ArgumentParser<char_t>& ArgumentParser<char_t>::add(Arg&& arg) {
    m_argSets[0].add(std::forward<Arg>(arg));
    return *this;
}

template<typename char_t>
inline ArgumentParser<char_t>& ArgumentParser<char_t>::add(ArgumentSet<char_t> argSet) {
    m_argSets.emplace_back(std::move(argSet));
    return *this;
}

template<typename char_t>
template<typename Arg>
inline ArgumentParser<char_t>& ArgumentParser<char_t>::addConstraint(Arg&& constr) {
    m_constraints.add(std::forward<Arg>(constr));
    return *this;
}

template<typename char_t>
inline std::basic_string<char_t> ArgumentParser<char_t>::help() const {
    std::vector< std::pair<std::basic_string<char_t>, std::basic_string<char_t>> > infos;
    typename std::basic_string<char_t>::size_type maxLength = 0;

    for(auto const& argSet: m_argSets) {
        for(auto const& arg: argSet.args()) {
            maxLength = std::max(maxLength, arg->ident().length());
        }
    }

    maxLength += 2;

    std::basic_string<char_t> helpText = "Usage: ";
    if (m_programName.length() > 0) {
        helpText += m_programName + " ";
    }
    helpText += m_argSets[0].usage();
    for(auto it = (m_argSets.begin()+1); it != m_argSets.end(); ++it) {
        if (it->size() == 0) {
            continue;
        }
        helpText += " " + it->usage();
    }
    helpText += '\n';
    for(auto const& argSet: m_argSets) {
        if (argSet.size() == 0) {
            continue;
        }
        helpText += "\n" + argSet.name() + ":\n";
        for(auto const& arg: argSet.args()) {
            std::basic_string<char_t> ident = arg->ident();
            helpText += "  " + ident;
            helpText += std::basic_string<char_t>(maxLength - ident.length(), ' ');
            helpText += arg->description();
            helpText += "\n";
        }
    }

    return helpText;
}

template<typename char_t>
inline void ArgumentParser<char_t>::parse(int argc, const char_t* const argv[]) {
    // Push back all arguments in vector.
    // skip argv[0], it is the program name
    if (m_programName.length() == 0) {
        m_programName = argv[0];
    }
    std::vector<std::basic_string<char_t>> args;
    for (int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    parse(args);
}

template<typename char_t>
inline const Argument<char_t>* ArgumentParser<char_t>::findArg() const {
    const Argument<char_t>* arg = nullptr;
    for(auto const& argSet: m_argSets) {
        for (auto const& checkArg: argSet.args()) {
            if (checkArg->matches()) {
                arg = checkArg;
                if (arg->can_set()) {
                    return arg;
                }
            }
        }
    }
    return arg;
}

template<typename char_t>
template<typename Ident>
inline const Argument<char_t>* ArgumentParser<char_t>::findArg(Ident ident) const {
    const Argument<char_t>* arg = nullptr;
    for(auto const& argSet: m_argSets) {
        for (auto const& checkArg: argSet.args()) {
            if (checkArg->matches(ident)) {
                arg = checkArg;
                if (arg->can_set()) {
                    return arg;
                }
            }
        }
    }
    return arg;
}

template<typename char_t>
inline void ArgumentParser<char_t>::parse(std::vector<std::basic_string<char_t>> const& argv) const {
    bool noParse = false;

    for (auto it = argv.begin(); it != argv.end(); ++it) {
        auto const& arg = *it;

        const Argument<char_t>* matchedArg = nullptr;
        if (arg == skip) {
            // After skip token, stop parsing
            noParse = true;
            continue;
        } else if (!noParse && arg.compare(0, strlen(nameStart), nameStart) == 0 && arg != nameStart) {
            // Named argument
            std::basic_string<char_t> name;

            //Check if delimiter present, split if so
            auto found = arg.find(nameDelim);
            bool hasDelim = (found != std::basic_string<char_t>::npos && found != 0);
            if (hasDelim) {
                name = arg.substr(strlen(nameStart), found - strlen(nameStart));
            } else {
                name = arg.substr(strlen(nameStart));
            }

            // Find argument
            matchedArg = findArg(name);

            if (matchedArg == nullptr) {
                throw unknown_argument<char_t>(name);
            }

            if (matchedArg->takes_value()) {
                if (hasDelim) {
                    set_arg_value(matchedArg, arg.substr(found+1));
                } else {
                    ++it;
                    if (it == argv.end()) {
                        // Value expected but not given
                        throw argument_missing_value<char_t>(*matchedArg);
                    } else {
                        set_arg_value(matchedArg, *it);
                    }
                }
            } else if (hasDelim) {
                throw argument_no_value<char_t>(*matchedArg);
            } else {
                matchedArg->set();
            }
        } else if (!noParse && arg.compare(0, strlen(flagStart), flagStart) == 0 && arg != flagStart) {
            // flag argument. May be followed by other flags, or actual value
            // argument has to determine this
            auto flagIndex = strlen(flagStart);
            for (; flagIndex < arg.length(); ++flagIndex) {
                matchedArg = findArg(arg[flagIndex]);

                if (matchedArg == nullptr) {
                    // Lookup failure
                    throw unknown_argument<char_t>(arg[flagIndex]);
                }

                // Test if the flag takes a value, if not, grab next index
                if (matchedArg->takes_value()) {
                    ++flagIndex;
                    break;
                } else {
                    matchedArg->set();
                }
            }

            if (matchedArg->takes_value()) {
                if (flagIndex < arg.length()) {
                    set_arg_value(matchedArg, arg.substr(flagIndex));
                } else {
                    ++it;
                    if (it == argv.end()) {
                        // Value expected but not given
                        throw argument_missing_value<char_t>(*matchedArg);
                    } else {
                        set_arg_value(matchedArg, *it);
                    }
                }
            } else {
                // Already set in for loop
            }
        } else {
            // Positional/unnamed argument
            matchedArg = findArg();

            // If still no argument, cannot do anything
            if (matchedArg == nullptr) {
                throw unknown_argument<char_t>();
            }

            if (matchedArg->takes_value()) {
                if (it == argv.end()) {
                    // Value expected but not given
                    throw argument_missing_value<char_t>(*matchedArg);
                }
                // Set the argument value
                set_arg_value(matchedArg, *it);
            } else {
                matchedArg->set();
            }
        }
    }

    for(auto const& argSet: m_argSets) {
		// Some error in arguments, print diagnostics
		argSet.check_valid();
    }

	// Some error in constraints, print diagnostics
	m_constraints.check_valid();
}

template<typename char_t>
inline void ArgumentParser<char_t>::set_arg_value(const Argument<char_t>* arg, const std::basic_string<char_t>& value) const {
    if (!arg->takes_value()) {
        throw std::logic_error("Attempt to set value on non-valued argument");
    }
    auto valueArg = dynamic_cast<const ValueAcceptor<char_t>*>(arg);
    if (valueArg == nullptr) {
        throw std::logic_error("Requested value interface on non-valued argument");
    }
    valueArg->set(value);
}

}
