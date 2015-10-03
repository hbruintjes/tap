/*
 * Tap.cpp
 *
 *  Created on: Sep 13, 2015
 *      Author: harold
 */

#if 0

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

#else

#define TAP_STREAMSAFE 1
#define TAP_AUTOFLAG 1
#include "tap/Tap.h"

#include <array>
#include <cassert>

using namespace TAP;

///////////////////////
// Argument aliasing //
///////////////////////
void testArgumentMatchFlag() {
    Argument arg1("", 'a');
    assert(!arg1.matches());
    assert(arg1.matches('a'));
    assert(!arg1.matches('b'));
    assert(!arg1.matches("a"));
    assert(!arg1.matches("b"));
}

void testArgumentMatchName() {
    Argument arg1("", "a");
    assert(!arg1.matches());
    assert(!arg1.matches('a'));
    assert(!arg1.matches('b'));
    assert(arg1.matches("a"));
    assert(!arg1.matches("b"));
}

void testArgumentMatchPositional() {
    ValueArgument<int> arg1{"", 1};
    assert(arg1.matches());
    assert(!arg1.matches('a'));
    assert(!arg1.matches('b'));
    assert(!arg1.matches("a"));
    assert(!arg1.matches("b"));
}

void testArgumentMatchAlias() {
    Argument arg1("", 'a');
    arg1.alias('b');
    arg1.alias("c");
    assert(!arg1.matches());
    assert(arg1.matches('a'));
    assert(arg1.matches('b'));
    assert(!arg1.matches("a"));
    assert(!arg1.matches("b"));
    assert(arg1.matches("c"));
}

///////////////////
// Argument find //
///////////////////
void testArgumentFind() {
    Argument arg1("", 'a');
    std::vector<const Argument*> collector;
    arg1.find_all_arguments(collector);
    assert(collector.size() == 1 && collector[0] == &arg1);
}

//////////////////////
// Argument setting //
//////////////////////
void testArgumentSet() {
    Argument arg1("", 'a');

    assert(!arg1);
    assert(arg1.count() == 0);

    arg1.set();
    assert(arg1);
    assert(arg1.count() == 1);
}

void testArgumentSet2() {
    Argument arg1("", 'a');
    arg1.many();

    assert(!arg1);
    assert(arg1.count() == 0);

    arg1.set();
    assert(arg1);
    assert(arg1.count() == 1);

    arg1.set();
    assert(arg1);
    assert(arg1.count() == 2);
}

//////////////////////////
// Argument count tests //
//////////////////////////
void testArgumentOptional() {
    Argument arg1("", 'a');

    arg1.check_valid();

    arg1.set();
    arg1.check_valid();

    assert(!arg1.can_set());
    arg1.set();

    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }
}

void testArgumentRequired() {
    Argument arg1("", 'a');
    arg1.set_required();

    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }

    arg1.set();
    arg1.check_valid();

    arg1.set();
    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }
}

void testArgumentMax() {
    Argument arg1("", 'a');
    arg1.max(2); // default

    arg1.check_valid();
    assert(arg1.can_set());

    arg1.set();
    arg1.check_valid();
    assert(arg1.can_set());

    arg1.set();
    arg1.check_valid();

    assert(!arg1.can_set());
    arg1.set();
    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }
}

void testArgumentMin() {
    Argument arg1("", 'a');
    arg1.set_required();
    arg1.min(2);

    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }

    arg1.set();
    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }

    arg1.set();
    arg1.check_valid();

    assert(!arg1.can_set());
    arg1.set();
    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }
}

void testArgumentExact() {
    Argument arg1("", 'a');
    arg1.set_required();
    arg1.min(2);
    arg1.max(2);

    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }

    arg1.set();
    assert(arg1.can_set());
    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }

    arg1.set();
    arg1.check_valid();
    assert(!arg1.can_set());

    arg1.set();
    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }
}

void testArgumentRange() {
    Argument arg1("", 'a');
    arg1.set_required();
    arg1.min(2);
    arg1.max(3);

    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }

    arg1.set();
    assert(arg1.can_set());
    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }

    arg1.set();
    arg1.check_valid();
    assert(arg1.can_set());

    arg1.set();
    arg1.check_valid();

    assert(!arg1.can_set());
    arg1.set();
    try {
        arg1.check_valid();
        assert(false);
    } catch (argument_count_mismatch& e) {
        // Ok
    }
}

void testArgumentMany() {
    Argument arg1("", 'a');
    arg1.many();

    arg1.check_valid();

    arg1.set();
    arg1.check_valid();

    arg1.set();
    arg1.check_valid();

    arg1.set();
    arg1.check_valid();
}

void testArgumentCheck() {
    Argument arg1("", 'a');
    bool checked = false;

    arg1.check([&checked] (const Argument&) -> void {checked = true;});

    arg1.set();
    assert(checked == true);
}

void testArgumentCheckExcept() {
    Argument arg1("", 'a');
    bool checked = false;

    arg1.check([&checked] (const Argument&) -> void {throw TAP::exception("test");});

    try {
        arg1.set();
        assert(false);
    } catch (TAP::exception& e) {
        // Ok
    }
}

//////////////////////
// Valued arguments //
//////////////////////
void testValueArgument() {
    ValueArgument<int> arg1("", 'a');

    assert(arg1.value() == int());

    arg1.set("2");
    assert(arg1.value() == 2);
}

void testValueArgumentDefault() {
    ValueArgument<int> arg1("", 'a', 2);
    ValueArgument<int> arg2("", 2);
    ValueArgument<int> arg3("", "alpha", 2);

    assert(arg1.value() == 2);
    assert(arg2.value() == 2);
    assert(arg3.value() == 2);

    arg1.set("3");
    assert(arg1.value() == 3);
}

void testValueArgumentMany() {
    ValueArgument<int> arg1("", 'a');
    arg1.many();

    assert(arg1.value() == int());

    arg1.set("2");
    assert(arg1.value() == 2);

    arg1.set("3");
    assert(arg1.value() == 3);
}

void testValueArgumentInvalid() {
    ValueArgument<int> arg1("", 'a', 2);

    assert(arg1.value() == 2);

    try{
        arg1.set("a");
        assert(false);
    } catch (argument_invalid_value& e) {
        // Ok
    }

#ifdef TAP_STREAMSAFE
    assert(arg1.value() == 2);
#else
    assert(arg1.value() == 0);
#endif
}

void testVariableArgument() {
    int value = 4;
    VariableArgument<int> arg1("", 'a', value);

    assert(&arg1.value() == &value);
    assert(value == 4);

    arg1.set("2");
    assert(arg1.value() == 2);
    assert(value == 2);
}

void testVariableArgumentMany() {
    int value = 4;
    VariableArgument<int> arg1("", 'a', value);
    arg1.many();

    assert(arg1.value() == 4 && value == 4);

    arg1.set("2");
    assert(arg1.value() == 2 && value == 2);

    arg1.set("3");
    assert(arg1.value() == 3 && value == 3);
}

void testVariableArgumentInvalid() {
    int value = 4;
    VariableArgument<int> arg1("", 'a', value);

    try{
        arg1.set("a");
        assert(false);
    } catch (argument_invalid_value& e) {
        // Ok
    }

#ifdef TAP_STREAMSAFE
    assert(arg1.value() == 4);
#else
    assert(arg1.value() == 0);
#endif
}

void testValueArgumentCheck() {
    ValueArgument<int> arg1("", 'a');
    int checked = 0;

    arg1.check([&checked] (const TypedArgument<int>&, const int& value) -> void {checked = value;});

    arg1.set("3");
    assert(checked == 3);
}

void testValueArgumentCheckExcept() {
    ValueArgument<int> arg1("", 'a');
    int checked = 0;

    arg1.check(
            [&checked] (const TypedArgument<int>&, const int& value) -> void {
        if (value == 3) {
            throw TAP::exception("test 3");
        }} );

    try {
        arg1.set("3");
        assert(false);
    } catch (TAP::exception& e) {
        // Ok
    }
}

////////////////////////////
// Multi-Valued arguments //
////////////////////////////
void testMultiValueArgument() {
    MultiValueArgument<int> arg1("", 'a');

    assert(arg1.value().size() == 0);

    arg1.set("2");
    assert(arg1.value()[0] == 2);

    arg1.set("3");
    assert(arg1.value()[0] == 2);
    assert(arg1.value()[1] == 3);
}

void testMultiValueArgumentInvalid() {
    MultiValueArgument<int> arg1("", 'a');

    assert(arg1.value().size() == 0);

    try{
        arg1.set("a");
        assert(false);
    } catch (argument_invalid_value& e) {
        // Ok
    }
    assert(arg1.value().size() == 0);
}

void testMultiVariableArgument() {
    std::vector<int> values;
    MultiVariableArgument<int> arg1("", 'a', values);

    assert(values.size() == 0);

    arg1.set("2");
    assert(values[0] == 2);

    arg1.set("3");
    assert(values[0] == 2);
    assert(values[1] == 3);
}

void testMultiVariableArgumentInvalid() {
    std::vector<int> values;
    MultiVariableArgument<int> arg1("", 'a', values);

    assert(values.size() == 0);

    try{
        arg1.set("a");
        assert(false);
    } catch (argument_invalid_value& e) {
        // Ok
    }
    assert(values.size() == 0);
}

////////////////////////
// Constant arguments //
////////////////////////
void testConstArgument() {
    int value = 0;
    ConstArgument<int> arg1("", 'a', value, 3);

    assert(!arg1 && arg1.value() == 0 && value == 0);

    arg1.set();
    assert(arg1 && arg1.value() == 3 && value == 3);
}

//////////////////////
// Switch arguments //
//////////////////////
void testSwitchArgumentExternal() {
    bool value = false;
    SwitchArgument arg1("", 'a', value);

    assert(!arg1 && !arg1.value() && !value);

    arg1.set();
    assert(arg1 && arg1.value() && value);
}

void testSwitchArgumentExternalMany() {
    bool value = false;
    SwitchArgument arg1("", 'a', value);
    arg1.many();

    assert(!arg1 && !arg1.value() && !value);

    arg1.set();
    assert(arg1 && arg1.value() && value);

    arg1.set();
    assert(arg1 && !arg1.value() && !value);
}

void testSwitchArgument() {
    SwitchArgument arg1("", 'a');

    assert(!arg1 && !arg1.value());

    arg1.set();
    assert(arg1 && arg1.value());
}

void testSwitchArgumentMany() {
    SwitchArgument arg1("", 'a');
    arg1.many();

    assert(!arg1 && !arg1.value());

    arg1.set();
    assert(arg1 && arg1.value());

    arg1.set();
    assert(arg1 && !arg1.value());
}

/////////////////
// Constraints //
/////////////////
void testArgumentConstraint() {
    Argument arg1("", 'a');
    Argument arg2("", 'b');
    ArgumentConstraint<ConstraintType::None> argNone( arg1, arg2);

    std::vector<const Argument*> collector;
    argNone.find_all_arguments(collector);
    assert(collector.size() == 2);

    argNone.check_valid();
}

void testArgumentConstraintRequired() {
    Argument arg1("", 'a');
    Argument arg2("", 'b');
    ArgumentConstraint<ConstraintType::None> argNone{arg1, arg2};

    argNone.check_valid();

    arg1.set();
    try {
        argNone.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }
}

void testArgumentConstraintSatisfy() {
    Argument arg1("", 'a');
    Argument arg2("", 'b');
    ArgumentConstraint<ConstraintType::None> argNone( arg1, arg2);
    ArgumentConstraint<ConstraintType::One>  argOne( arg1, arg2);
    ArgumentConstraint<ConstraintType::Any>  argAny( arg1, arg2);
    ArgumentConstraint<ConstraintType::All>  argAll( arg1, arg2);
    argNone.set_required(); // this actually does nothing semantically
    argOne.set_required();
    argAny.set_required();
    argAll.set_required();

    argNone.check_valid();
    try{
        argOne.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }
    try{
        argAll.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }    try{
        argAny.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }

    arg1.set();

    try{
        argNone.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }
    argOne.check_valid();
    argAny.check_valid();
    try{
        argAll.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }

    arg2.set();

    try{
        argNone.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }
    try{
        argOne.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }
    argAny.check_valid();
    argAll.check_valid();
}

void testArgumentConstraintNested() {
    Argument arg1("", 'a');
    Argument arg2("", 'b');
    Argument arg3("", 'c');
    ArgumentConstraint<ConstraintType::None> argNone2(arg3);
    ArgumentConstraint<ConstraintType::None> argNone(arg1, arg2, argNone2);

    std::vector<const Argument*> collector;
    argNone.find_all_arguments(collector);
    assert(collector.size() == 3);
    collector.clear();
    argNone2.find_all_arguments(collector);
    assert(collector.size() == 1);
}

void testArgumentConstraintNestedSatisfy() {
    Argument arg1("", 'a');
    Argument arg2("", 'b');
    Argument arg3("", 'c');
    ArgumentConstraint<ConstraintType::All> argAll2(arg3);
    ArgumentConstraint<ConstraintType::All> argAll(arg1, arg2, argAll2);
    argAll.set_required();
    argAll2.set_required();

    try{
        argAll.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }
    try{
        argAll2.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }

    arg1.set();

    try{
        argAll.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }
    try{
        argAll2.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }

    arg3.set();

    try{
        argAll.check_valid();
        assert(false);
    } catch (TAP::constraint_error& e) {
        // Ok
    }
    argAll2.check_valid();

    arg2.set();

    argAll.check_valid();
    argAll2.check_valid();

}

//#include <iostream>

void testArgumentConstraintUsage() {
    Argument arg0("", 'o');
    Argument arg1("", 'a');
    Argument arg2("", 'b');
    Argument arg3("", 'c');
    Argument arg4("", 'd');
    Argument arg5("", 'e');
    Argument arg6("", 'f');
    Argument arg7("", 'g');
    Argument arg8("", 'h');
    Argument arg9("", 'i');
    ArgumentConstraint<ConstraintType::None> carg1(+arg1, arg2);
    ArgumentConstraint<ConstraintType::One> carg2(+arg3, arg4);
    ArgumentConstraint<ConstraintType::Any> carg4(+arg7, arg8);
    ArgumentConstraint<ConstraintType::All> carg5(+arg9, arg0);

    ArgumentConstraint<ConstraintType::None> carg6(carg1, carg2, carg4, carg5);
    ArgumentConstraint<ConstraintType::One> carg7(carg1, carg2, carg4, carg5);
    ArgumentConstraint<ConstraintType::Any> carg9(carg1, carg2, carg4, carg5);
    ArgumentConstraint<ConstraintType::All> carg0(carg1, carg2, carg4, carg5);

    /*std::cout << carg6.usage() << '\n';
    std::cout << carg7.usage() << '\n';
    std::cout << carg9.usage() << '\n';
    std::cout << carg0.usage() << '\n';*/
}

//////////////////
// Parser flags //
//////////////////
void testArgumentParserFlags() {
    Argument arg1("", 'a');
    Argument arg2("", 'b');
    Argument arg3("", 'c');

    std::array<const char*, 4> args = {
            "", "-a","-b", "-c"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2); p.add(arg3);
    p.parse(static_cast<int>(args.size()), args.data());

    assert(arg1 && arg2 && arg3);
}

void testArgumentParserFlagsJoin() {
    Argument arg1("", 'a');
    Argument arg2("", 'b');
    Argument arg3("", 'c');

    std::array<const char*, 2> args = {
            "", "-abc"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2); p.add(arg3);
    p.parse(static_cast<int>(args.size()), args.data());

    assert(arg1 && arg2 && arg3);
}

void testArgumentParserFlagsValue() {
    Argument arg1("", 'a');
    ValueArgument<std::string> arg2("", 'b');
    ValueArgument<std::string> arg3("", 'c');

    std::array<const char*, 3> args = {
            "", "-bvalue", "-acvalue"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2); p.add(arg3);
    p.parse(static_cast<int>(args.size()), args.data());

    assert(arg1 && arg2 && arg3);
    assert(arg2.value() == "value");
    assert(arg3.value() == "value");
}

void testArgumentParserFlagsNoValue() {
    Argument arg1("", 'a');

    std::array<const char*, 2> args = {
            "", "-avalue"
    };

    ArgumentParser p;
    p.add(arg1);
    try {
        p.parse(static_cast<int>(args.size()), args.data());
        assert(false);
    } catch(unknown_argument& e) {
        // OK
    }
}

void testArgumentParserFlagsMissingValue() {
    ValueArgument<std::string> arg1("", 'a');

    std::array<const char*, 2> args = {
            "", "-a"
    };

    ArgumentParser p;
    p.add(arg1);
    try {
        p.parse(static_cast<int>(args.size()), args.data());
        assert(false);
    } catch(argument_missing_value& e) {
        // OK
    }
}

void testArgumentParserFlagsUnknown() {
    Argument arg1("", 'a');

    std::array<const char*, 2> args = {
            "", "-b"
    };

    ArgumentParser p;
    p.add(arg1);

    try {
        p.parse(static_cast<int>(args.size()), args.data());
        assert(false);
    } catch(unknown_argument& e) {
        // OK
    }
}

//////////////////
// Parser names //
//////////////////
void testArgumentParserNames() {
    Argument arg1("", "alpha");
    Argument arg2("", "beta");
    Argument arg3("", "delta");

    std::array<const char*, 4> args = {
            "", "--alpha","--beta", "--delta"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2); p.add(arg3);
    p.parse(static_cast<int>(args.size()), args.data());

    assert(arg1 && arg2 && arg3);
}

void testArgumentParserNamesValue() {
    Argument arg1("", "alpha");
    ValueArgument<std::string> arg2("", "beta", std::string());
    Argument arg3("", "delta");

    std::array<const char*, 5> args = {
            "", "--alpha", "--beta", "value", "--delta"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2); p.add(arg3);
    p.parse(static_cast<int>(args.size()), args.data());

    assert(arg1 && arg2 && arg3);
    assert(arg2.value() == "value");
}

void testArgumentParserNamesDelimiter() {
    Argument arg1("", "alpha");
    ValueArgument<std::string> arg2("", "beta", std::string());
    Argument arg3("", "delta");

    std::array<const char*, 4> args = {
            "", "--alpha", "--beta=value", "--delta"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2); p.add(arg3);
    p.parse(static_cast<int>(args.size()), args.data());

    assert(arg1 && arg2 && arg3);
    assert(arg2.value() == "value");
}

void testArgumentParserNamesNoValue() {
    Argument arg1("", "alpha");

    std::array<const char*, 2> args = {
            "", "--alpha=value"
    };

    ArgumentParser p;
    p.add(arg1);
    try {
        p.parse(static_cast<int>(args.size()), args.data());
        assert(false);
    } catch(argument_no_value& e) {
        // OK
    }
}

void testArgumentParserNamesMissingValue() {
    ValueArgument<std::string> arg1("", "alpha", std::string());

    std::array<const char*, 2> args = {
            "", "--alpha"
    };

    ArgumentParser p;
    p.add(arg1);
    try {
        p.parse(static_cast<int>(args.size()), args.data());
        assert(false);
    } catch(argument_missing_value& e) {
        // OK
    }
}

void testArgumentParserNamesUnknown() {
    Argument arg1("", "alpha");

    std::array<const char*, 2> args = {
            "", "--beta"
    };

    ArgumentParser p;
    p.add(arg1);

    try {
        p.parse(static_cast<int>(args.size()), args.data());
        assert(false);
    } catch(unknown_argument& e) {
        // OK
    }
}

///////////////////////
// Parser positional //
///////////////////////
void testArgumentParserPositional() {
    ValueArgument<std::string> arg1{""};

    std::array<const char*, 2> args = {
            "", "a"
    };

    ArgumentParser p;
    p.add(arg1);
    p.parse(static_cast<int>(args.size()), args.data());
    assert(arg1 && arg1.count() == 1 && arg1.value() == "a");
}

void testArgumentParserPositionalMany() {
    ValueArgument<std::string> arg1{""};
    arg1.many();

    std::array<const char*, 5> args = {
            "", "a", "b", "c", "d"
    };

    ArgumentParser p;
    p.add(arg1);
    p.parse(static_cast<int>(args.size()), args.data());
    //assert(arg1 && arg1.count() == 4 && arg1.value() == "d");
}

void testArgumentParserPositionalMany2() {
    ValueArgument<std::string> arg1{""};
    ValueArgument<std::string> arg2{""};
    arg1.many();
    arg2.many();

    std::array<const char*, 5> args = {
            "", "a", "b", "c", "d"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2);
    p.parse(static_cast<int>(args.size()), args.data());
    assert(arg1 && arg1.count() == 4 && arg1.value() == "d");
    assert(!arg2);
}

void testArgumentParserPositionalMany3() {
    ValueArgument<std::string> arg1{""};
    ValueArgument<std::string> arg2{""};
    arg2.many();

    std::array<const char*, 5> args = {
            "", "a", "b", "c", "d"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2);
    p.parse(static_cast<int>(args.size()), args.data());
    assert(arg1 && arg1.count() == 1 && arg1.value() == "a");
    assert(arg2 && arg2.count() == 3 && arg2.value() == "d");
}

void testArgumentParserPositionalCountedRange() {
    ValueArgument<std::string> arg1{""};
    ValueArgument<std::string> arg2{""};
    arg1.max(2);
    arg2.max(2);

    std::array<const char*, 5> args = {
            "", "a", "b", "c", "d"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2);
    p.parse(static_cast<int>(args.size()), args.data());
    assert(arg1 && arg1.count() == 2 && arg1.value() == "b");
    assert(arg2 && arg2.count() == 2 && arg2.value() == "d");
}

void testArgumentParserPositionalSkipUnknown() {
    Argument arg1("", 'a');
    ValueArgument<std::string> arg2{""};
    Argument arg3("", 'b');

    std::array<const char*, 5> args = {
            "", "-a", "-b", "--", "-c"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2); p.add(arg3);
    p.parse(static_cast<int>(args.size()), args.data());
    assert(arg1 && arg2 && arg3);
    assert(arg2.count() == 1 && arg2.value() == "-c");
}

void testArgumentParserPositionalSkipKnown() {
    Argument arg1("", 'a');
    ValueArgument<std::string> arg2{""};
    arg2.many();
    Argument arg3("", 'b');
    Argument arg4("", 'c');
    Argument arg5("", "delta");

    std::array<const char*, 6> args = {
            "", "-a", "-b", "--", "-c", "--delta"
    };

    ArgumentParser p;
    p.add(arg1); p.add(arg2); p.add(arg3); p.add(arg4); p.add(arg5);
    p.parse(static_cast<int>(args.size()), args.data());
    assert(arg1 && arg2 && arg3 && !arg4 && !arg5);
    assert(arg2.count() == 2 && arg2.value() == "--delta");
}

void testArgumentParserPositionalUnknown() {
    std::array<const char*, 2> args = {
            "", "value"
    };

    ArgumentParser p;

    try {
        p.parse(static_cast<int>(args.size()), args.data());
        assert(false);
    } catch(unknown_argument& e) {
        // OK
    }
}

class Arg {
public:
    int x;

    Arg() : x(0) {
    }

    Arg(char a) : x(a) {
    }

    Arg(int a) : x(a) {
    }

};

std::istream& operator>>(std::istream& is, Arg& a) {
    return is >> a.x;
}

void testArgumentConstructors() {
    ValueArgument<int> a0("");
    assert(a0.matches() && a0.value() == int());
    ValueArgument<int> a1("", 1);
    assert(a1.matches() && a1.value() == 1);
    ValueArgument<int> a2("", 'a', 1);
    assert(a2.matches('a') && a2.value() == 1);
    ValueArgument<int> a3("", "a", 1);
    assert(a3.matches("a") && a3.value() == 1);
    ValueArgument<int> a4("", std::string("a"), 1);
    assert(a4.matches("a") && a4.value() == 1);

    ValueArgument<char> b0("");
    assert(b0.matches() && b0.value() == char());
    ValueArgument<char> b1("", 'b');
    assert(b1.matches('b') && b1.value() == char());
    ValueArgument<char> b2("", 'b', '1');
    assert(b2.matches('b') && b2.value() == '1');
    ValueArgument<char> b3("", "b", '1');
    assert(b3.matches("b") && b3.value() == '1');
    ValueArgument<char> b4("", std::string("b"), '1');
    assert(b4.matches("b") && b4.value() == '1');

    ValueArgument<std::string> c0("");
    assert(c0.matches() && c0.value() == "");
    ValueArgument<std::string> c1("", 'b');
    assert(c1.matches('b') && c1.value() == "");
    ValueArgument<std::string> c2("", 'b', "1");
    assert(c2.matches('b') && c2.value() == "1");
    ValueArgument<std::string> c3("", "b", std::string("1"));
    assert(c3.matches("b") && c3.value() == "1");
    ValueArgument<std::string> c4("", std::string("b"), "1");
    assert(c4.matches("b") && c4.value() == "1");
    ValueArgument<std::string> c5("", "b", std::string(2, 'a'));
    assert(c5.matches("b") && c5.value() == "aa");

    ValueArgument<std::string> c6("", "b");
    assert(c6.matches() && c6.value() == "b");

    ValueArgument<Arg> d1("", 3);
    ValueArgument<Arg> d2("", 'c', 3);
    ValueArgument<Arg> d3("", "b", 3);
    ValueArgument<Arg> d4("", 'c', "b", 3);
    ValueArgument<Arg> d5("", 'c', "b", Arg(3));
    Arg x{1};
    VariableArgument<Arg> d6{"", x};
    VariableArgument<Arg> d7("", 'c', x);
    VariableArgument<Arg> d8("", "b", x);
    VariableArgument<Arg> d9("", 'c', "b", x);

    ValueArgument<Arg> da{""};
    ValueArgument<Arg> db("", 'c');
    assert(db.matches('c'));
    ValueArgument<Arg> dc("", Arg('c'));
    assert(dc.matches());
    ValueArgument<Arg> dd("", 'c', 3);
    assert(dd.matches('c') && dd.value().x == 3);
    ValueArgument<Arg> de("", 3);
    assert(de.value().x == 3);

    d1.set("2");
    assert(d1.value().x == 2);
    d6.set("2");
    assert(x.x == 2);
}

void testArgumentAutoFlag() {
    ValueArgument<int> arg1("this is a &test argument");
    assert(arg1.matches('t'));
    assert(arg1.matches("test"));

    ValueArgument<int> arg2("this is a $test argument");
    assert(!arg2.matches('t'));
    assert(arg2.matches("test"));

    ValueArgument<int> arg3("this is a %test argument");
    assert(arg3.matches('t'));
    assert(!arg3.matches("test"));
}

int main(int argc, const char** argv) {
    testArgumentMatchFlag();
    testArgumentMatchName();
    testArgumentMatchPositional();
    testArgumentMatchAlias();
    testArgumentFind();
    testArgumentSet();
    testArgumentSet2();
    testArgumentOptional();
    testArgumentRequired();
    testArgumentMax();
    testArgumentMin();
    testArgumentExact();
    testArgumentRange();
    testArgumentMany();
    testArgumentCheck();
    testArgumentCheckExcept();

    testValueArgument();
    testValueArgumentDefault();
    testValueArgumentMany();
    testValueArgumentInvalid();
    testVariableArgument();
    testVariableArgumentMany();
    testVariableArgumentInvalid();
    testValueArgumentCheck();
    testValueArgumentCheckExcept();

    testMultiValueArgument();
    testMultiValueArgumentInvalid();
    testMultiVariableArgument();
    testMultiVariableArgumentInvalid();

    testConstArgument();

    testSwitchArgumentExternal();
    testSwitchArgumentExternalMany();
    testSwitchArgument();
    testSwitchArgumentMany();

    testArgumentConstraint();
    testArgumentConstraintRequired();
    testArgumentConstraintSatisfy();
    testArgumentConstraintNested();
    testArgumentConstraintNestedSatisfy();
    testArgumentConstraintUsage();

    testArgumentParserFlags();
    testArgumentParserFlagsJoin();
    testArgumentParserFlagsValue();
    testArgumentParserFlagsNoValue();
    testArgumentParserFlagsMissingValue();
    testArgumentParserFlagsUnknown();

    testArgumentParserNames();
    testArgumentParserNamesDelimiter();
    testArgumentParserNamesValue();
    testArgumentParserNamesNoValue();
    testArgumentParserNamesMissingValue();
    testArgumentParserNamesUnknown();

    testArgumentParserPositional();
    testArgumentParserPositionalMany();
    testArgumentParserPositionalMany2();
    testArgumentParserPositionalMany3();
    testArgumentParserPositionalUnknown();

    testArgumentParserPositionalCountedRange();
    testArgumentParserPositionalSkipKnown();
    testArgumentParserPositionalSkipUnknown();

    testArgumentConstructors();

    testArgumentAutoFlag();

    ArgumentParser pars{};
    pars.parse(argc, argv);

    return 0;
}
#endif
