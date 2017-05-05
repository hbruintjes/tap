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

template<typename... Args>
inline ArgumentParser::ArgumentParser(Args&&... args) :
    m_constraints("Constraints")
{
    m_argSets.emplace_back("Arguments", args...);
}

template<typename... Args>
inline ArgumentParser& ArgumentParser::addAll(Args&&... args) {
    detail::Temporary<char[]> { (
            add(std::forward<Args>(args))
            ,'0')..., '0' };
    return *this;
}

template<typename Arg>
inline ArgumentParser& ArgumentParser::add(Arg&& arg) {
    m_argSets[0].add(std::forward<Arg>(arg));
    return *this;
}

inline ArgumentParser& ArgumentParser::add(ArgumentSet argSet) {
    m_argSets.emplace_back(std::move(argSet));
    return *this;
}

template<typename Arg>
inline ArgumentParser& ArgumentParser::addConstraint(Arg&& constr) {
    m_constraints.add(std::forward<Arg>(constr));
    return *this;
}

inline std::string ArgumentParser::help() const {
    std::vector< std::pair<std::string, std::string> > infos;
    std::string::size_type maxLength = 0;

    for(const ArgumentSet& argSet: m_argSets) {
        for(const Argument* arg: argSet.args()) {
            maxLength = std::max(maxLength, arg->ident().length());
        }
    }

    maxLength += 2;

    std::string helpText = "Usage: ";
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
    for(const ArgumentSet& argSet: m_argSets) {
        if (argSet.size() == 0) {
            continue;
        }
        helpText += "\n" + argSet.name() + ":\n";
        for(const Argument* arg: argSet.args()) {
            std::string ident = arg->ident();
            helpText += "  " + ident;
            helpText += std::string(maxLength - ident.length(), ' ');
            helpText += arg->description();
            helpText += "\n";
        }
    }

    return helpText;
}

inline void ArgumentParser::parse(int argc, const char* const argv[]) {
    // Push back all arguments in vector.
    // skip argv[0], it is the program name
    if (m_programName.length() == 0) {
        m_programName = argv[0];
    }
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    parse(args);
}

inline const Argument* ArgumentParser::findArg() const {
    const Argument* arg = nullptr;
    for(const ArgumentSet& argSet: m_argSets) {
        for (const Argument* checkArg: argSet.args()) {
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

template<typename Ident>
inline const Argument* ArgumentParser::findArg(Ident ident) const {
    const Argument* arg = nullptr;
    for(const ArgumentSet& argSet: m_argSets) {
        for (const Argument* checkArg: argSet.args()) {
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

inline void ArgumentParser::parse(std::vector<std::string>& argv) const {
    bool noParse = false;

    for (auto it = argv.begin(); it != argv.end(); ++it) {
        const std::string& arg = *it;

        const Argument* matchedArg = nullptr;
        if (arg == skip) {
            // After skip token, stop parsing
            noParse = true;
            continue;
        } else if (!noParse && arg.compare(0, strlen(nameStart), nameStart) == 0 && arg != nameStart) {
            // Named argument
            std::string name;

            //Check if delimiter present, split if so
            std::size_t found = arg.find(nameDelim);
            bool hasDelim = (found != std::string::npos && found != 0);
            if (hasDelim) {
                name = arg.substr(strlen(nameStart), found - strlen(nameStart));
            } else {
                name = arg.substr(strlen(nameStart));
            }

            // Find argument
            matchedArg = findArg(name);

            if (matchedArg == nullptr) {
                throw unknown_argument(name);
            }

            if (matchedArg->takes_value()) {
                if (hasDelim) {
                    set_arg_value(matchedArg, arg.substr(found+1));
                } else {
                    ++it;
                    if (it == argv.end()) {
                        // Value expected but not given
                        throw argument_missing_value(*matchedArg);
                    } else {
                        set_arg_value(matchedArg, *it);
                    }
                }
            } else if (hasDelim) {
                throw argument_no_value(*matchedArg);
            } else {
                matchedArg->set();
            }
        } else if (!noParse && arg.compare(0, strlen(flagStart), flagStart) == 0 && arg != flagStart) {
            // flag argument. May be followed by other flags, or actual value
            // argument has to determine this
            size_t flagIndex = strlen(flagStart);
            for (; flagIndex < arg.length(); ++flagIndex) {
                matchedArg = findArg(arg[flagIndex]);

                if (matchedArg == nullptr) {
                    // Lookup failure
                    throw unknown_argument(arg[flagIndex]);
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
                        throw argument_missing_value(*matchedArg);
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
                throw unknown_argument();
            }

            if (matchedArg->takes_value()) {
                if (it == argv.end()) {
                    // Value expected but not given
                    throw argument_missing_value(*matchedArg);
                }
                // Set the argument value
                set_arg_value(matchedArg, *it);
            } else {
                matchedArg->set();
            }
        }
    }

    for(const ArgumentSet& argSet: m_argSets) {
		// Some error in arguments, print diagnostics
		argSet.check_valid();
    }

	// Some error in constraints, print diagnostics
	m_constraints.check_valid();
}

inline void ArgumentParser::set_arg_value(const Argument* arg, const std::string& value) const {
    if (!arg->takes_value()) {
        throw std::logic_error("Attempt to set value on non-valued argument");
    }
    const ValueAcceptor* valueArg = dynamic_cast<const ValueAcceptor*>(arg);
    if (valueArg == nullptr) {
        throw std::logic_error("Requested value interface on non-valued argument");
    }
    valueArg->set(value);
}

}
