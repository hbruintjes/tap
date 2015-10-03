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
 * @file Parser.hpp
 * @brief Contains the definitions for the ArgumentParser.
 */

#pragma once

#include <algorithm> // min/max

namespace TAP {

/**
 * Argument parser class. Main job is to parse a given set of command line
 * options and feed them into a set of Argument instances, checking the
 * constraints afterwards. Arguments are identified either as a flag argument
 * (see TAP::flagStart), a named argument (see TAP::nameStart) or a positional
 * argument. If the skip marker has been defined (see TAP::skip), any argument
 * presented after this marker is treated as a positional argument. Flag
 * arguments are allowed to be joined together (e.g. '-abc' instead of
 * '-a -b -c'). The final flag is allowed to have its value attached (e.g
 * '-abvalue' is treated as '-a -b value' if '-b' accepts a value). Name
 * arguments can optionally have their value joined with a delimiter to the
 * argument itself (e.g. '--alpha=value', see TAP::nameDelim). Effectively,
 * parsing is similar to that of GNU get_opt_long(), except Perl like arguments
 * are not supported.
 */
class ArgumentParser {
protected:
    /** Collection ArgumentSets (groups) */
    std::vector<ArgumentSet> m_argSets;

    /** Stored argument constraints */
    ArgumentSet m_constraints;

    /** Program name as displayed in help text. If not set explicitly, will use
     * the first argument of the parse function */
    std::string m_programName;
public:
    /**
     * Construct a new ArgumentParser. The given list of Arguments is added to
     * the parser immediately. Does not generate help text.
     * @param args Arguments (or constraints) to add
     */
    template<typename... Args>
    ArgumentParser(Args&&... args);

    /**
     * ArgumentParser destructor.
     */
    virtual ~ArgumentParser() {
    }

    /**
     * Set the program name. This name is used in the usage string when
     * displaying help. If not set, the first argument from calling parse()
     * is used instead.
     * @return Reference to this ArgumentParser
     */
    ArgumentParser& program_name(const std::string& programName) {
        m_programName = programName;
        return *this;
    }

    /**
     * Returns the program name.
     * @return Program name
     */
    const std::string& program_name() const {
        return m_programName;
    }

    /**
     * Add all given arguments, or constraints, to the parser.
     * @param args Arguments to add
     * @return Reference to this ArgumentParser
     */
    template<typename... Args>
    ArgumentParser& addAll(Args&&... args);

    /**
     * Add the given argument, or constraint, to the parser.
     * @param arg Argument to add
     * @return Reference to this ArgumentParser
     */
    template<typename Arg>
    ArgumentParser& add(Arg&& arg);

    /**
     * Add the given ArgumentSet to the parser.
     * @param argSet ArgumentSet to add
     * @return Reference to this ArgumentParser
     */
    ArgumentParser& add(ArgumentSet argSet);

    /**
     * Add the given constraint, to the parser. Constraints are not shown in
     * the help, but are checked. Argument instances in the constraint must
     * have been added before as a regular argument for the parser to
     * recognize them (see add()).
     * @param constr Argument constraint to add
     * @return Reference to this ArgumentParser
     */
    template<typename Arg>
    ArgumentParser& addConstraint(Arg&& constr);

    /**
     * Get the Argument given the flag. If the Argument is not contained
     * exactly once, behavior is undefined.
     * @param flag Flag to search Argument by
     * @return The Argument that matches the flag
     */
    const Argument& operator[](char flag) const {
        const Argument* arg = findArg(flag);
        if (arg == nullptr) {
            throw std::out_of_range("Argument not found");
        }
        return *arg;
    }

    /**
     * Get the Argument given the flag. If the Argument is not contained
     * exactly once, behavior is undefined.
     * @param name Name to search Argument by
     * @return The Argument that matches the name
     */
    const Argument& operator[](const std::string& name) const {
        const Argument* arg = findArg(name);
        if (arg == nullptr) {
            throw std::out_of_range("Argument not found");
        }
        return *arg;
    }

    /**
     * Generate a help string for the user to see. Contains a short usage line,
     * and a list of accepted arguments with their descriptions.
     * @return A string with help text.
     */
    std::string help() const;

    /**
     * Parses the given arguments as they are presented on main() (see the
     * parsing rules in the description of the ArgumentParser class). Throws an
     * exception if parsing fails, otherwise returns a boolean indicating if the
     * program should continue or stop (e.g. after displaying help).
     * @param argc Number of items in the argv array
     * @param argv Program arguments. The first item is expected to be the
     *             program invocation name
     */
    void parse(int argc, const char* const argv[]);

private:
    /**
     * Find positional argument (see Argument::matches()), either the first one
     * that can be set, or the last one that matches. Returns a null-pointer if
     * not found.
     * @return First matching argument that can be set, last matching argument
     *         if none can be set, or nullptr if no matching arguments
     */
    const Argument* findArg() const;

    /**
     * Find the argument matching the given parameter (see
     * Argument::matches()), either the first one that can be set, or the last
     * one that matches. Returns a null-pointer if not found.
     * @param ident void, char or std::string to identify the argument to find
     * @return First matching argument that can be set, last matching argument
     *         if none can be set, or nullptr if no matching arguments
     */
    template<typename Ident>
    const Argument* findArg(Ident ident) const;

    /**
     * Parses the given argument vector (see the parsing rules in the
     * description of the ArgumentParser class). Throws an exception if parsing
     * fails, otherwise returns a Boolean indicating if the program should
     * continue or stop (e.g. after displaying help).
     * @param argv Program arguments. This vector may be modified by this
     *             function
     */
    void parse(std::vector<std::string>& argv) const;

    /**
     * Helper function to set the value of an Argument of which takes_value()
     * returns true.
     * @param arg Argument to set value to
     * @param value Value to set
     */
    void set_arg_value(const Argument* arg, const std::string& value) const;
};

}
