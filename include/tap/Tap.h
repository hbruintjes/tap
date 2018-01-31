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
 * @file Tap.h
 * @brief Main TAP header file. Include this file to get started.
 *
 * @mainpage The Argument Parser
 *
 * @section sec_intro Introduction
 *
 * This is TAP, a library for command line argument parsing ala get_opt_long().
 * It is a header-only library with a bunch of templated classes that ought to
 * make argument parsing simple. The main reasons for the existence of this
 * library:
 * * Simple to install, drop in the headers and done;
 * * Ability to store argument variables directly into passed variables, or
 *   internally in the argument data;
 * * Attempt to somewhat separate the notion of constraints;
 * * Something that uses a bit more recent C++ dialect (C++14);
 * * It does mostly the same as anything else, but slightly different.
 *
 * Try before you buy, check some of the examples, and otherwise look at for
 * instance boost::program_options, TCLAP, get_opt() (ha),
 * "The Lean Mean C++ Option Parser", cpp-optparse, or just check
 * http://stackoverflow.com/questions/865668/parse-command-line-arguments.
 *
 * @section sec_install Installation
 *
 * As this is a header-only library, no install is necessary. Just drop the
 * directory 'tap' with all headers somewhere in your include path, and all
 * should be fine.
 *
 * @section sec_usage Usage
 *
 * To use the library, first add a
 * @code
 * #include <tap/Tap.h>
 * @endcode
 * to your sources, then simply code away. All members of TAP are defined in
 * the TAP namespace.
 *
 * Quickstart:
 * @code
#include <tap/Tap.h>
#include <iostream>

int main(int argc, const char* argv[]) {
    TAP::Argument badExit("If set, exit with non-zero status", 'x', "badexit");
    TAP::ArgumentParser parser(badExit);
    try {
        parser.parse(argc, argv);
    } catch (TAP::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    if (badExit) {
        // The argument was set
        std::cout << "Just, why?" << std::endl;
        return 1;
    }
    return 0;
}
 * @endcode
 *
 * @section sec_tutorial Tutorial and examples
 * Argument parsing starts with defining a number of TAP::Argument instances,
 * adding them to an TAP::ArgumentParser, and passing along the actual program
 * arguments (as received from main()) to the parser. If all goes well, you can
 * read in the parsed data from the arguments, otherwise an TAP::exception will
 * be thrown.
 *
 * The examples below assume TAP_AUTOFLAG has been defined, see @ref sec_config
 * for details.
 *
 * @subsection sec_argdef Defining arguments
 * At the basic level, an argument consists of a description, and zero or more
 * aliases. Aliases can be used to name the argument on the command line, either
 * as a flag or as a name (see TAP::ArgumentParser for details). One particular
 * remark about aliases: If an argument is created without an alias, it is
 * marked as positional. Any alias defined later does not change this. This
 * allows positional arguments to be defined, that can also be accessed via a
 * name or flag.
 *
 * The description can be used to define aliases as well. To do so, mark the
 * flags or names to use in the description (see
 * TAP::Argument::parse_description()). You can use this to save on typing when
 * defining arguments.
 *
 * Multiple argument classes exist that can be used, in short they are
 * * Argument: Simple argument class that can only be marked as set. They always
 *   have an alias associated with them. @code
 *   // These two arguments are equivalent
 *   TAP::Argument a1("Show this help text", 'h', "help");
 *   TAP::Argument a2("Show this &help text");
 *   @endcode
 * * VariableArgument: Argument that can be assigned a value on the command
 *   line, which is stored in a variable that is given to the argument. @code
 *   int optimization = 1; // This also defines the default value.
 *   TAP::VariableArgument<int> optimizeLevel("Set &optimization level", optimization);
 *   @endcode
 * * ValueArgument: Like VariableArgument, but stores the value internally.
 *   @code
 *   // Initialize with default value 1
 *   TAP::ValueArgument<int> optimizeLevel("Set &optimization level", 1);
 *   @endcode
 * * MultiVariableArgument, MultiValueArgument: Like above, but can occur
 *   multiple times on the command line, and each occurrence is stored
 *   separately. An std::vector is used to store these values.
 *   @code
 *   TAP::MultiValueArgument<std::string> includes("%Include these files");
 *   ...
 *   const std::string& firstFile = includes.value()[0];
 *   @endcode
 * * ConstArgument: Like Argument, but when it occurs, writes a constant value
 *   to a variable that is given to the argument.
 *   @code
 *   enum class LogLevel {DEBUG, INFO, ERROR};
 *   LogLevel logLevel = LogLevel::INFO; // default to info
 *   TAP::ConstArgument<LogLevel> debugLevel("Set &debug level", logLevel, LogLevel::DEBUG);
 *   TAP::ConstArgument<LogLevel> infoLevel("Set &info level", logLevel, LogLevel::INFO);
 *   TAP::ConstArgument<LogLevel> errorLevel("Set &error level", logLevel, LogLevel::ERROR);
 *   @endcode
 * * SwitchArgument: Like Argument, but keeps track of a boolean value, either
 *   internally or externally, that is switched each time it occurs.
 *   @code
 *   bool do_daemonize = false; // default to false
 *   TAP::SwitchArgument daemonize("Fork to background", 'd', do_daemonize);
 *   // restart defaults to true, so if set will switch to false
 *   // note: this is a terrible way to interact
 *   TAP::SwitchArgument restart("Do not &restart on error", true);
 *   @endcode
 *
 *
 * @subsection sec_argreq Required arguments and argument counts
 * By default, all arguments are optional, and can occur at most once. To change
 * this behavior, each argument has the following functions:
 * * set_required() : Require the argument to occur at least once
 * * many() : Allow the argument to occur an unbounded amount of times
 * * min(N) : Require the argument to occur at least N times
 * * max(N) : Allow the argument to occur at most N times
 *
 * Regular arguments by default are optional and can occur at most once. Multi*
 * arguments can occur many times.
 * Note that for most TypedArgument types (except Multi*), if the argument is
 * allowed to occur multiple times, its value is overwritten on each occurrence.
 *
 * A common occurrence of arguments that are allowed to occur multiple times,
 * but sometimes limited, are verbosity or debugging level arguments. For
 * example:
 * @code
 * TAP::Argument verbosity("Set the verbosity level", 'v', "verbose");
 * verbosity.max(3); // alternatively: many()
 * ...
 * if (verbosity) {
 *     unsigned int verbosityLevel = verbosity.count();
 * }
 * @endcode
 *
 * Note that there is a (small) semantic difference between a required argument
 * and the minimum number of occurrences. If required() is true, this only
 * means the argument has to occur somewhere. The min() value dictates how many
 * times. If required() is false, the argument is also alowed to be absent,
 * even if min is not zero. In fact, min is required to be always at least 1.
 *
 * @subsection sec_argcheck Argument callbacks
 * It is possible to assign a callback to an TAP::Argument that is called
 * whenever the argument is set. For example: @code
 * TAP::Argument help("Show this &help text");
 * TAP::ArgumentParser parser(help);
 * help.check([&parser](const TAP::Argument&) -> void {
 *     std::cout << parser.help() << std::endl; exit(1);
 * });
 * @endcode
 * For details see TAP::Argument::check() and TAP::ArgumentCheckFunc.
 *
 * Similarly, TAP::TypedArgument also allows for a callback with the value that
 * was set, for example: @code
 * TAP::ValueArgument<int> answer("Set the &answer");
 * TAP::ArgumentParser parser(answer);
 * answer.check([&parser](const TAP::TypedArgument<int>&, int val) -> void {
 *     if (val != 42) {
 *         throw TAP::exception("That is not the answer");
 *     }
 * });
 * @endcode
 * For details see TAP::TypedArgument::check() and TAP::TypedArgumentCheckFunc.
 *
 * @subsection sec_argconstr Argument constraints
 * Every now and then some arguments can only occur in certain combinations or
 * have some sort of constraint associated with them (aside from the number of
 * occurrences allowed or required, see @ref sec_argreq). To specify this,
 * arguments can be added (recursively) into TAP::ArgumentConstraint instances,
 * which specify a certain constraint on the occurrences of arguments relative
 * to each other. Most commonly, exactly one argument must be selected out of a
 * group of many.
 *
 * To specify such a constraint, create an instance of TAP::ArgumentConstraint,
 * templated to TAP::ConstraintType to specify which constraint, and add the
 * constrained arguments. For example:
 * @code
 * TAP::Argument left("Turn &left");
 * TAP::Argument right("Turn &right");
 * TAP::ArgumentConstraint<TAP::ConstraintType::One> dir(left, right);
 * TAP::Argument help("Show this &help text");
 * TAP::ArgumentParser parser(dir, help);
 * @endcode
 *
 * In this example, left and right cannot be set both at the same time. Should
 * either left or right be selected (so selecting neither is invalid), set the
 * constraint as required with set_required().
 *
 * Note: Be careful when adding arguments that are required to constraints with
 * an upper bound on the number of allowed arguments (such as
 * TAP::ConstraintType::One). This can lead to situations where the constraint
 * can never be satisfied, as all of its arguments are required, but the
 * constraint prohibits this.
 *
 * @subsubsection sec_argoperators Operators
 * For the common case of mutually exclusive arguments, the hat operator
 * (binary XOR) has been defined for arguments to create a
 * TAP::ConstraintType::One constraint of the given arguments. For example:
 * @code
 * TAP::Argument left("Turn &left");
 * TAP::Argument right("Turn &right");
 * TAP::Argument help("Show this &help text");
 * TAP::ArgumentParser parser(left ^ right, help);
 * @endcode
 * Similarly, the OR operator (|) is defined for TAP::ConstraintType::Any, and
 * the GREATER operator (>) is defined for TAP::ConstraintType::Imp.
 *
 * Furthermore, unary operators + and - set an argument respectively as
 * required or optional. Thus, in the above example, to require exactly left
 * or right to be set, you can use:
 * @code
 * TAP::ArgumentParser parser(+(left ^ right), help);
 * @endcode
 *
 * @subsection sec_argparsing Parsing arguments
 * Parsing arguments is relatively straight-forward. After defining all
 * arguments and an argument parser, with the arguments added to the parser,
 * simply invoke TAP::ArgumentParser::parse and done. If an error occurs, an
 * exception of the type TAP::exception (or a subclass thereof) will be thrown.
 * @code
 * int main(int argc, char** argv) {
 *     TAP::Argument help("Show this &help text");
 *     TAP::Argument version("Show &version information");
 *     TAP::ArgumentParser parser(help, version);
 *     parser.parse(argc, argv);
 *     if (help) {
 *         std::cout << parser.help() << std::endl;
 *     } else if(version) {
 *         std::cout << "FIXME" << std::endl;
 *     }
 *     return 0;
 * }
 * @endcode
 * Arguments can be added either in the constructor, or using the
 * TAP::ArgumentParser::add() method.
 *
 * @subsubsection sec_arggroups Argument groups
 * To group arguments (useful mostly for the help text) a TAP::ArgumentSet can
 * be created (similar to a constraint), with a given name. When added to the
 * parser, its children are grouped separately in the help text.
 *
 * @subsection argtest_sec Argument testing and value retrieval
 * To determine if an argument is set, converting to bool is possible, e.g.
 * @code
 * TAP::Argument myArg("&test");
 * if (myArg) {
 *     ...
 * }
 * @endcode
 * Keep in mind that for arguments that actually store a bool, this is not the
 * same as its value.
 *
 * To get the number of occurrences, see the TAP::Argument::count() method. For
 * TAP::TypedArgument instances, the value can be retrieved using
 * TAP::TypedArgument::value(). Note that for Multi valued arguments, the value
 * is automatically set to a vector.
 *
 * @section sec_config Configuration
 * TAP allows for some configuration in the main header file. The following
 * settings alter some of its behavior:
 * * TAP_STREAMSAFE : When defined, the default stream operator will defer
 *   writing to a stored variable until the input is considered valid. This
 *   prevents the variable from being modified if the input is not valid. Note
 *   that this requires stored variables to allow copy or move construction.
 * * TAP_AUTOFLAG : When defined, try to parse the description string to find
 *   flag and/or name markers. See also TAP::Argument::parse_description().
 *
 * Aside from these options, other defines allow some of the syntax to be
 * tweaked (see Tap.h for more details):
 * * TAP_FLAG : Defines the string for TAP::flagStart
 * * TAP_NAME : Defines the string for TAP::nameStart
 * * TAP_NAMEDELIMITER : Defines the string for TAP::nameDelim
 * * TAP_SKIP: Defines the string for TAP::skip
 *
 * @section sec_quirks Quirks
 * Though the library is intended to use relatively easy to use, it may not
 * always do what is expected. Some quirks are listed here:
 * * Arguments are stored as copies inside the parser, constraints and groups.
 *   Thus, adding an argument to the parser and modifying it afterwards does
 *   not work as one may expect. However, copies of arguments will share the
 *   same occurrence counter and value, thus adding an argument without
 *   modification to multiple constraints will still work.
 * * No argument duplication checks are made. If you add an argument (or two
 *   different arguments) with the same alias to the parser, constraint or
 *   group, the library will not generate an error. This is useful for the case
 *   where an argument occurs in multiple constraints (as they are stored as
 *   copies, see above point), but may yield surprising results otherwise. The
 *   parser will try to find arguments in the order they were added, so the
 *   first added argument has priority. However, if that argument is already
 *   set its maximum amount of times, it will look for the next. Only if that
 *   fails an error is generated.
 * * The point above also implies something else: It is possible to add
 *   multiple positional arguments that can occur many times. In that case,
 *   only the first argument added will receive values.
 *
 * @section sec_issues Known issues
 * * VariableArgument overload resolution kind of wonky
 * * Help text argument sorting and duplicates, also no skip marker
 * * Not particularly high performance
 */

#pragma once

#include <locale>
#include <string>

/**
 * @brief The main TAP namespace. Everything in the TAP library is part of this
 * namespace.
 */
namespace TAP {

/** Marker for flags (one letter arg) */
#ifndef TAP_FLAG
const char flagStart[] = "-";
#else
const char flagStart[] = TAP_FLAG;
#endif

/** Marker for names (>1 letter arg) */
#ifndef TAP_NAME
const char nameStart[] = "--";
#else
const char nameStart[] = TAP_NAME;
#endif

/** Delimiter between name and argument (e.g. --name=value).
 * Define as '\0' to disable */
#ifndef TAP_NAMEDELIMITER
const char nameDelim = '=';
#else
const char nameDelim = TAP_NAMEDELIMITER;
#endif

/** Define the parsed arg delimiter.
 * Define as "" to disable */
#ifndef TAP_SKIP
const char skip[] = "--";
#else
const char skip[] = TAP_SKIP;
#endif

// If TAP_STREAMSAFE is defined, stream operator will first check the result
// before writing to storage.
//#define TAP_STREAMSAFE 1

// If TAP_AUTOFLAG defined, special characters in the argument description will
// be used to define flags and names. It has a runtime hit but looks fancy.
//#define TAP_AUTOFLAG 1

	namespace detail {
		template<typename char_t, size_t N>
		struct Converter {
			static std::basic_string<char_t> widen(char const (&str)[N]) {
				char_t buffer[N];
				std::use_facet<std::ctype<char_t> >(std::locale()).widen(str, str+N, buffer);
				return buffer;
			}
		};
		template<size_t N>
		struct Converter<char, N> {
			static std::basic_string<char> widen(char const (&str)[N]) {
				return str;
			}
		};
	}
	template<typename char_t, size_t N>
	std::basic_string<char_t> widen_const(char const (&str)[N]) {
		return detail::Converter<char_t, N>::widen(str);
	}
	template<typename char_t>
	char_t widen_const(char const ch) {
		return std::use_facet<std::ctype<char_t> >(std::locale()).widen(ch);
	}
	template<>
	char widen_const<char>(char const ch) {
		return ch;
	}

}

#include "tap/BaseArgument.hpp"
#include "tap/Argument.hpp"
#include "tap/TypedArgument.hpp"
#include "tap/ArgumentConstraint.hpp"
#include "tap/Parser.hpp"
#include "tap/Exceptions.hpp"
#include "tap/Operators.hpp"

#include "tap/impl/Argument.hpp"
#include "tap/impl/TypedArgument.hpp"
#include "tap/impl/ArgumentConstraint.hpp"
#include "tap/impl/Parser.hpp"
#include "tap/impl/Exceptions.hpp"
#include "tap/impl/Operators.hpp"

namespace TAP {
	using argument = basic_argument<char>;
	using wargument = basic_argument<wchar_t>;

	template<typename T, bool multi = false>
	using variable_argument = basic_variable_argument<char, T, multi>;
	template<typename T, bool multi = false>
	using wvariable_argument = basic_variable_argument<wchar_t, T, multi>;

	template<typename T, bool multi = false>
	using value_argument = basic_value_argument<char, T, multi>;
	template<typename T, bool multi = false>
	using wvalue_argument = basic_value_argument<wchar_t, T, multi>;

	template<typename T>
	using const_argument = basic_const_argument<char, T>;
	template<typename T>
	using wconst_argument = basic_const_argument<wchar_t, T>;

	using switch_argument = basic_switch_argument<char>;
	using wswitch_argument = basic_switch_argument<wchar_t>;

	using argument_parser = basic_argument_parser<char>;
	using wargument_parser = basic_argument_parser<wchar_t>;

	template<constraint_type CType>
	using argument_constraint = basic_argument_constraint<char, CType>;
	template<constraint_type CType>
	using wargument_constraint = basic_argument_constraint<wchar_t, CType>;
}