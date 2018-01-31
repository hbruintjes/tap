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
 * @brief Contains the definitions for the basic_argument_parser.
 */

#pragma once

#include <algorithm> // min/max

namespace TAP {

/**
 * basic_argument parser class. Main job is to parse a given set of command line
 * options and feed them into a set of basic_argument instances, checking the
 * constraints afterwards. arguments are identified either as a flag basic_argument
 * (see TAP::flagStart), a named basic_argument (see TAP::nameStart) or a positional
 * basic_argument. If the skip marker has been defined (see TAP::skip), any basic_argument
 * presented after this marker is treated as a positional basic_argument. Flag
 * arguments are allowed to be joined together (e.g. '-abc' instead of
 * '-a -b -c'). The final flag is allowed to have its value attached (e.g
 * '-abvalue' is treated as '-a -b value' if '-b' accepts a value). Name
 * arguments can optionally have their value joined with a delimiter to the
 * basic_argument itself (e.g. '--alpha=value', see TAP::nameDelim). Effectively,
 * parsing is similar to that of GNU get_opt_long(), except Perl like arguments
 * are not supported.
 */
template<typename char_t>
class basic_argument_parser {
protected:
    /** Collection basic_argument_sets (groups) */
    std::vector<basic_argument_set<char_t>> m_argSets;

    /** Stored basic_argument constraints */
    basic_argument_set<char_t> m_constraints;

    /** Program name as displayed in help text. If not set explicitly, will use
     * the first basic_argument of the parse function */
    std::basic_string<char_t> m_programName;
public:
    /**
     * Construct a new basic_argument_parser. The given list of arguments is added to
     * the parser immediately. Does not generate help text.
     * @param args arguments (or constraints) to add
     */
    template<typename... Args>
    basic_argument_parser(Args&&... args);

    /**
     * basic_argument_parser destructor.
     */
    virtual ~basic_argument_parser() {
    }

    /**
     * Set the program name. This name is used in the usage string when
     * displaying help. If not set, the first basic_argument from calling parse()
     * is used instead.
     * @return Reference to this basic_argument_parser
     */
    basic_argument_parser& program_name(const std::basic_string<char_t>& programName) {
        m_programName = programName;
        return *this;
    }

    /**
     * Returns the program name.
     * @return Program name
     */
    const std::basic_string<char_t>& program_name() const {
        return m_programName;
    }

    /**
     * Add all given arguments, or constraints, to the parser.
     * @param args arguments to add
     * @return Reference to this basic_argument_parser
     */
    template<typename... Args>
    basic_argument_parser& addAll(Args&&... args);

    /**
     * Add the given basic_argument, or constraint, to the parser.
     * @param arg basic_argument to add
     * @return Reference to this basic_argument_parser
     */
    template<typename Arg>
    basic_argument_parser& add(Arg&& arg);

    /**
     * Add the given basic_argument_set to the parser.
     * @param argSet basic_argument_set to add
     * @return Reference to this basic_argument_parser
     */
    basic_argument_parser& add(basic_argument_set<char_t> argSet);

    /**
     * Add the given constraint, to the parser. Constraints are not shown in
     * the help, but are checked. basic_argument instances in the constraint must
     * have been added before as a regular basic_argument for the parser to
     * recognize them (see add()).
     * @param constr basic_argument constraint to add
     * @return Reference to this basic_argument_parser
     */
    template<typename Arg>
    basic_argument_parser& addConstraint(Arg&& constr);

    /**
     * Get the basic_argument given the flag. If the basic_argument is not contained
     * exactly once, behavior is undefined.
     * @param flag Flag to search basic_argument by
     * @return The basic_argument that matches the flag
     */
    const basic_argument<char_t>& operator[](char_t flag) const {
        auto arg = findArg(flag);
        if (arg == nullptr) {
            throw std::out_of_range("basic_argument not found");
        }
        return *arg;
    }

    /**
     * Get the basic_argument given the flag. If the basic_argument is not contained
     * exactly once, behavior is undefined.
     * @param name Name to search basic_argument by
     * @return The basic_argument that matches the name
     */
    const basic_argument<char_t>& operator[](const std::basic_string<char_t>& name) const {
        auto arg = findArg(name);
        if (arg == nullptr) {
            throw std::out_of_range("basic_argument not found");
        }
        return *arg;
    }

    /**
     * Generate a help string for the user to see. Contains a short usage line,
     * and a list of accepted arguments with their descriptions.
     * @return A string with help text.
     */
    std::basic_string<char_t> help() const;

    /**
     * Parses the given arguments as they are presented on main() (see the
     * parsing rules in the description of the basic_argument_parser class). Throws an
     * exception if parsing fails, otherwise returns a boolean indicating if the
     * program should continue or stop (e.g. after displaying help).
     * @param argc Number of items in the argv array
     * @param argv Program arguments. The first item is expected to be the
     *             program invocation name
     */
    void parse(int argc, const char_t* const argv[]);

    /**
     * Parses the given basic_argument vector (see the parsing rules in the
     * description of the basic_argument_parser class). Throws an exception if parsing
     * fails, otherwise returns a Boolean indicating if the program should
     * continue or stop (e.g. after displaying help).
     * @param argv Program arguments. This vector may be modified by this
     *             function
     */
    void parse(std::vector<std::basic_string<char_t>> const& argv) const;

private:
    /**
     * Find positional basic_argument (see basic_argument::matches()), either the first one
     * that can be set, or the last one that matches. Returns a null-pointer if
     * not found.
     * @return First matching basic_argument that can be set, last matching basic_argument
     *         if none can be set, or nullptr if no matching arguments
     */
    const basic_argument<char_t>* findArg() const;

    /**
     * Find the basic_argument matching the given parameter (see
     * basic_argument::matches()), either the first one that can be set, or the last
     * one that matches. Returns a null-pointer if not found.
     * @param ident void, char or std::basic_string<char_t> to identify the basic_argument to find
     * @return First matching basic_argument that can be set, last matching basic_argument
     *         if none can be set, or nullptr if no matching arguments
     */
    template<typename Ident>
    const basic_argument<char_t>* findArg(Ident ident) const;

    /**
     * Helper function to set the value of an basic_argument of which takes_value()
     * returns true.
     * @param arg basic_argument to set value to
     * @param value Value to set
     */
    void set_arg_value(const basic_argument<char_t>* arg, const std::basic_string<char_t>& value) const;
};

}
