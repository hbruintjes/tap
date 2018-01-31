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
 * @file typed_argument.hpp
 * @brief Contains the definitions for basic_value_arguments.
 */

#pragma once

#include <type_traits>

namespace TAP {

template<typename char_t, typename T, bool multi = false>
class typed_argument;

/** Function pointer that is used by basic_value_argument::check() */
template<typename char_t, typename T, bool multi>
using typed_argumentCheckFunc = std::function<void(const typed_argument<char_t, T, multi>&, const T& value)>;

/**
 * Base class for arguments that hold a typed value. The class can optionally be
 * set to store multiple values in a vector. The check_typed function should be
 * used instead of check.
 * Template parameter T indicates the type of the value to store.
 * Template parameter multi indicates if multiple values should be stored.
 * Defaults to false. Note that if multi is false, but the basic_argument can be set
 * multiple times, the value is overwritten each time.
 * Note that this class cannot be used directly, a derived class is to be used.
 */
template<typename char_t, typename T, bool multi>
class typed_argument: public basic_argument<char_t> {
    static_assert(!std::is_void<T>::value, "Cannot make void arguments, use plain basic_argument");
    static_assert(!std::is_const<T>::value, "Cannot make const arguments");
    static_assert(!std::is_volatile<T>::value, "Cannot make volatile arguments");
    static_assert(!std::is_reference<T>::value, "Cannot make reference arguments");
    static_assert(!std::is_pointer<T>::value, "Cannot make pointer arguments");

protected:
    /** Alias for the storage type (which is a vector for Multibasic_value_argument
     * types) */
    using ST = typename std::conditional< !multi, T, std::vector<T> >::type;

    /** Pointer to value storage */
    mutable ST* m_storage;

    /** Callback for typed check */
    typed_argumentCheckFunc<char_t, T, multi> m_typedCheckFunc = nullptr;

protected:
    /**
     * Create a typed_argument with given description and storage pointer. Values
     * will be stored in the variable pointed to.
     * @param description Description of the basic_argument (used in help text)
     * @param storage Pointer to storage variable
     */
    typed_argument(std::basic_string<char_t> description, ST* storage) :
        basic_argument<char_t>(std::move(description)), m_storage(storage) {
        basic_argument<char_t>::m_max = (multi?0:1);
    }

    /**
     * Create a typed_argument with given description and storage pointer,
     * aliased to the given flag. Values will be stored in the variable pointed
     * to.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    typed_argument(std::basic_string<char_t> description, char_t flag, ST* storage) :
        basic_argument<char_t>(std::move(description), flag), m_storage(storage) {
        basic_argument<char_t>::m_max = (multi?0:1);
    }

    /**
     * Create a typed_argument with given description and storage pointer,
     * aliased to the given name. Values will be stored in the variable pointed
     * to.
     * @param description Description of the basic_argument (used in help text)
     * @param name Name identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    typed_argument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, ST* storage) :
        basic_argument<char_t>(std::move(description), name), m_storage(storage) {
        basic_argument<char_t>::m_max = (multi?0:1);
    }

    /**
     * Create a typed_argument with given description and storage pointer,
     * aliased to the given flag and name. Values will be stored in the variable
     * pointed to.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param name Name identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    typed_argument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, ST* storage) :
        basic_argument<char_t>(std::move(description), flag, name), m_storage(storage) {
        basic_argument<char_t>::m_max = (multi?0:1);
    }

public:
    /**
     * See basic_argument::set()
     */
    void set() const override = 0;

    /**
     * See basic_argument::takes_value()
     */
    bool takes_value() const override = 0;

    /**
     * Returns the value of this basic_argument.
     * @return Reference to the value of this basic_argument.
     */
    const ST& value() const {
        return *m_storage;
    }

    /**
     * See basic_argument::check()
     */
    virtual typed_argument& check_typed(typed_argumentCheckFunc<char_t, T, multi> typedCheckFunc) {
        m_typedCheckFunc = typedCheckFunc;
        return *this;
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() const & override = 0;

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() && override = 0;

protected:
    /**
     * See basic_argument::check()
     */
    void
    check() const override {
        doCheck();
    }

    /**
     * See basic_argument::check()
     */
    template<bool m = multi>
    typename std::enable_if<!m>::type
    doCheck() const {
        if (m_typedCheckFunc != nullptr) {
            m_typedCheckFunc(*this, *m_storage );
        }
    }

    /**
     * See basic_argument::check()
     */
    template<bool m = multi>
    typename std::enable_if<m>::type
    doCheck() const {
        if (m_typedCheckFunc != nullptr) {
            m_typedCheckFunc(*this, (*m_storage)[m_storage->size()-1] );
        }
    }
};

/**
 * Concrete implementation of typed_argument. Simply takes a value and stores it
 * in a variable.
 */
template<typename char_t, typename T, bool multi = false>
class basic_variable_argument : public typed_argument<char_t, T, multi>, public value_acceptor<char_t> {
    static_assert(!std::is_void<T>::value, "Cannot make void arguments, use basic_argument");

protected:
    /** Make typed_argument::ST accessible. */
    using ST = typename typed_argument<char_t, T, multi>::ST;

    /** Name of the accepted value, used in the identifier */
    std::basic_string<char_t> m_valueName = std::basic_string<char_t>(widen_const<char_t>("value"));

public:
    /**
     * Create a basic_variable_argument with given description and storage pointer.
     * Values will be stored in the variable pointed to.
     * @param description Description of the basic_argument (used in help text)
     * @param storage Pointer to storage variable
     */
    basic_variable_argument(std::basic_string<char_t> description, ST* storage) :
        typed_argument<char_t, T, multi>(std::move(description), storage) {
    }

    /**
     * Create a basic_variable_argument with given description and storage pointer,
     * aliased to the given flag. Values will be stored in the variable pointed
     * to.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    basic_variable_argument(std::basic_string<char_t> description, char_t flag, ST* storage) :
        typed_argument<char_t, T, multi>(std::move(description), flag, storage) {
    }

    /**
     * Create a basic_variable_argument with given description and storage pointer,
     * aliased to the given name. Values will be stored in the variable pointed
     * to.
     * @param description Description of the basic_argument (used in help text)
     * @param name Name identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    basic_variable_argument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, ST* storage) :
        typed_argument<char_t, T, multi>(std::move(description), name, storage) {
    }

    /**
     * Create a basic_variable_argument with given description and storage pointer,
     * aliased to the given flag and name. Values will be stored in the variable
     * pointed to.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param name Name identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    basic_variable_argument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, ST* storage) :
        typed_argument<char_t, T, multi>(std::move(description), flag, name, storage) {
    }

    /**
     * Create a basic_variable_argument with given description and storage reference.
     * Values will be stored in the variable referenced.
     * @param description Description of the basic_argument (used in help text)
     * @param storage Pointer to storage variable
     */
    basic_variable_argument(std::basic_string<char_t> description, ST& storage) :
        typed_argument<char_t, T, multi>(std::move(description), &storage) {
    }

    /**
     * Create a basic_variable_argument with given description and storage reference,
     * aliased to the given flag. Values will be stored in the variable
     * referenced.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    basic_variable_argument(std::basic_string<char_t> description, char_t flag, ST& storage) :
        typed_argument<char_t, T, multi>(std::move(description), flag, &storage) {
    }

    /**
     * Create a basic_variable_argument with given description and storage reference,
     * aliased to the given name. Values will be stored in the variable
     * referenced.
     * @param description Description of the basic_argument (used in help text)
     * @param name Name identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    basic_variable_argument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, ST& storage) :
        typed_argument<char_t, T, multi>(std::move(description), name, &storage) {
    }

    /**
     * Create a basic_variable_argument with given description and storage reference,
     * aliased to the given flag and name. Values will be stored in the variable
     * referenced.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param name Name identifier of this basic_argument
     * @param storage Pointer to storage variable
     */
    basic_variable_argument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, ST& storage) :
        typed_argument<char_t, T, multi>(std::move(description), flag, name, &storage) {
    }

    /**
     * basic_variable_argument destructor.
     */
    virtual ~basic_variable_argument() = default;

    /**
     * basic_variable_argument copy constructor.
     */
    basic_variable_argument(const basic_variable_argument&) = default;

    /**
     * basic_variable_argument move constructor.
     */
    basic_variable_argument(basic_variable_argument&&) = default;

    /**
     * basic_variable_argument copy assignment operator.
     */
    basic_variable_argument& operator=(const basic_variable_argument&) = default;

    /**
     * basic_variable_argument move assignment operator.
     */
    basic_variable_argument& operator=(basic_variable_argument&&) = default;

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() const & override {
        return std::unique_ptr<base_argument<char_t>>(new basic_variable_argument(*this));
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() && override {
        return std::unique_ptr<base_argument<char_t>>(new basic_variable_argument(std::move(*this)));
    }

    /**
     * Assign a human readable description for the value, used when describing
     * the basic_argument. Default is 'value'.
     * @param valueName String to describe the name of the value.
     * @return Reference to this basic_argument
     */
    basic_variable_argument& valuename(const std::basic_string<char_t>& valueName) {
        m_valueName = valueName;
        return *this;
    }

    /**
     * Return a human readable description for the value, used when describing
     * the basic_argument. Default is 'value'.
     * @return String representing the value basic_argument in help text
     */
    const std::basic_string<char_t>& valuename() {
        return m_valueName;
    }

    /**
     * See basic_argument::takes_value()
     */
    bool takes_value() const override {
        return !std::is_same<bool, T>::value;
    }

    /**
     * Calling this function is an error, call set(std::basic_string<char_t>) instead.
     */
    void set() const override;

    /**
     * See value_acceptor::set()
     */
    void set(const std::basic_string<char_t>& value) const override;

    /**
     * See basic_argument::usage()
     */
    std::basic_string<char_t> usage() const override;

    /**
     * See basic_argument::ident()
     */
    std::basic_string<char_t> ident() const override;
};

/**
 * Multibasic_variable_argument is a basic_variable_argument that, when allowed to occur
 * multiple times, stores each given value individually in a vector.
 * Alias for basic_variable_argument with the multi template basic_argument set to true.
 */
template<typename char_t, typename T>
using basic_multi_variable_argument = basic_variable_argument<char_t, T, true>;

/**
 * Implementation of typed_argument that stores a value by itself, based on
 * basic_variable_argument.
 */
template<typename char_t, typename T, bool multi = false>
class basic_value_argument : public basic_variable_argument<char_t, T, multi> {
protected:
    /** Make typed_argument::ST accessible. */
    using ST = typename basic_variable_argument<char_t, T, multi>::ST;

    /** If the basic_value_argument owns the variable storage, it is stored here */
    std::shared_ptr<ST> m_ownStorage;

public:
    /**
     * Create a positional basic_value_argument using an internal variable to store
     * the value. The parameter list is used to construct the value.
     * @param description Description of the basic_argument (used in help text)
     * @param params arguments passed to the constructor of the value storage
     */
    template<typename... U, typename = typename std::enable_if< std::is_constructible<ST, U...>::value >::type >
    basic_value_argument(std::basic_string<char_t> description, U&&... params) :
        basic_variable_argument<char_t, T, multi>(std::move(description), new ST(std::forward<U>(params)...)),
        m_ownStorage(typed_argument<char_t, T, multi>::m_storage)
    {
    }

    /**
     * Create a basic_value_argument using an internal variable to store the value,
     * aliased to the given flag. The parameter list is used to construct the
     * value.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param params arguments passed to the constructor of the value storage
     */
    template<typename... U, typename = typename std::enable_if< std::is_constructible<ST, U...>::value >::type>
    basic_value_argument(std::basic_string<char_t> description, char_t flag, U&&... params) :
        basic_variable_argument<char_t, T, multi>(std::move(description), flag, new ST(std::forward<U>(params)...)),
        m_ownStorage(typed_argument<char_t, T, multi>::m_storage)
    {
    }

    /**
     * Create a basic_value_argument using an internal variable to store the value,
     * aliased to the given flag. The parameter list is used to construct the
     * value.
     * @param description Description of the basic_argument (used in help text)
     * @param name Name identifier of this basic_argument
     * @param params arguments passed to the constructor of the value storage
     */
    template<typename... U, typename = typename std::enable_if< std::is_constructible<ST, U...>::value >::type >
    basic_value_argument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, U&&... params) :
        basic_variable_argument<char_t, T, multi>(std::move(description), name, new ST(std::forward<U>(params)...)),
        m_ownStorage(typed_argument<char_t, T, multi>::m_storage)
    {
    }

    /**
     * Create a basic_value_argument using an internal variable to store the value,
     * aliased to the given flag and name. The parameter list is used to
     * construct the value.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param name Name identifier of this basic_argument
     * @param params arguments passed to the constructor of the value storage
     */
    template<typename... U, typename = typename std::enable_if< std::is_constructible<ST, U...>::value >::type >
    basic_value_argument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, U&&... params) :
        basic_variable_argument<char_t, T, multi>(std::move(description), flag, name, new ST(std::forward<U>(params)...)),
        m_ownStorage(typed_argument<char_t, T, multi>::m_storage)
    {
    }

    /**
     * basic_value_argument destructor.
     */
    virtual ~basic_value_argument() = default;

    /**
     * basic_value_argument copy constructor.
     */
    basic_value_argument(const basic_value_argument&) = default;

    /**
     * basic_value_argument move constructor.
     */
    basic_value_argument(basic_value_argument&&) = default;

    /**
     * basic_value_argument copy assignment operator. Will copy the stored value if
     * the other basic_value_argument owns the variable, otherwise simply copies the
     * pointer.
     */
    basic_value_argument& operator=(const basic_value_argument&) = default;

    /**
     * basic_value_argument move assignment operator.
     */
    basic_value_argument& operator=(basic_value_argument&&) = default;

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() const & override {
        return std::unique_ptr<base_argument<char_t>>(new basic_value_argument(*this));
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() && override {
        return std::unique_ptr<base_argument<char_t>>(new basic_value_argument(std::move(*this)));
    }
};

/**
 * Multibasic_value_argument is a basic_value_argument that, when allowed to occur multiple
 * times, stores each given value individually in a vector.
 * Alias for basic_value_argument with the multi template basic_argument set to true.
 */
template<typename char_t, typename T>
using basic_multi_value_argument = basic_value_argument<char_t, T, true>;

/**
 * Specialization of typed_argument that acts as a switch on the command line,
 * but stores an arbitrary constant in the given variable storage. Practical for
 * for instance enumerations.
 */
template<typename char_t, typename T>
class basic_const_argument: public typed_argument<char_t, T, false> {
protected:
    /** Constant to store */
    T m_value;

public:
#ifdef TAP_AUTOFLAG
    /**
     * Create a constant basic_argument with aliases defined by the description, using
     * an external variable to store a predefined value.
     * @param description Description of the basic_argument (used in help text). Must
     *        contain flag or name markers
     * @param storage Variable to store the value in
     * @param value Value to set the variable to when this basic_argument is set
     */
    basic_const_argument(std::basic_string<char_t> description, T& storage, const T& value) :
        typed_argument<char_t, T, false>(std::move(description), &storage), m_value(value) {
    }
#endif

    /**
     * Create a constant basic_argument that is identified by a flag, using an
     * external variable to store a predefined value.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param storage Variable to store the value in
     * @param value Value to set the variable to when this basic_argument is set
     */
    basic_const_argument(std::basic_string<char_t> description, char_t flag, T& storage, const T& value) :
        typed_argument<char_t, T, false>(std::move(description), flag, &storage), m_value(value) {
    }

    /**
     * Create a constant basic_argument that is identified by a name, using an
     * external variable to store a predefined value.
     * @param description Description of the basic_argument (used in help text)
     * @param name Name identifier of this basic_argument
     * @param storage Variable to store the value in
     * @param value Value to set the variable to when this basic_argument is set
     */
    basic_const_argument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, T& storage, const T& value) :
        typed_argument<char_t, T, false>(std::move(description), name, &storage), m_value(value) {
    }

    /**
     * Create a constant basic_argument that is identified by both a flag and a name,
     * using an external variable to store a predefined value.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param name Name identifier of this basic_argument
     * @param storage Variable to store the value in
     * @param value Value to set the variable to when this basic_argument is set
     */
    basic_const_argument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, T& storage, const T& value) :
        typed_argument<char_t, T, false>(std::move(description), flag, name, &storage), m_value(value) {
    }

    /**
     * basic_const_argument copy constructor.
     */
    basic_const_argument(const basic_const_argument&) = default;
    /**
     * basic_const_argument move constructor.
     */
    basic_const_argument(basic_const_argument&&) = default;

    /**
     * basic_const_argument destructor.
     */
    virtual ~basic_const_argument() {
    }

    /**
     * basic_const_argument assignment operator.
     */
    basic_const_argument& operator=(const basic_const_argument& other) = default;

    /**
     * basic_const_argument move assignment operator.
     */
    basic_const_argument& operator=(basic_const_argument&& other) = default;

    /**
     * See basic_argument::takes_value()
     */
    bool takes_value() const override {
        return false;
    }

    /**
     * See basic_argument::set()
     */
    void set() const override {
        basic_argument<char_t>::set();
        *typed_argument<char_t, T, false>::m_storage = m_value;
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() const & override {
        return std::unique_ptr<base_argument<char_t>>(new basic_const_argument(*this));
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() && override {
        return std::unique_ptr<base_argument<char_t>>(new basic_const_argument(std::move(*this)));
    }
};

/**
 * basic_switch_argument is a specialization for typed_argument, which does not
 * accept values (it switches). When allowed to occur multiple times, the value
 * is inverted each time it occurs. Use this to keep track of the actual value
 * in a variable.
 */
template<typename char_t>
class basic_switch_argument : public typed_argument<char_t, bool, false> {
protected:
    /** If the basic_switch_argument owns the variable storage, it is stored here */
    std::shared_ptr<bool> m_ownStorage;

public:
    /////////////////
    // Constructors for storing value externally
    /////////////////
#ifdef TAP_AUTOFLAG
    /**
     * Create a switch basic_argument with aliases defined by the description, using
     * an external variable to store the value.
     * @param description Description of the basic_argument (used in help text). Must
     *        contain flag or name markers
     * @param storage Variable to store the value in
     */
    basic_switch_argument(std::basic_string<char_t> description, bool& storage) :
        typed_argument<char_t, bool, false>(std::move(description), &storage) {
    }
#endif

    /**
     * Create a switch basic_argument that is identified by a flag, using an
     * external variable to store the value.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param storage Variable to store the value in
     */
    basic_switch_argument(std::basic_string<char_t> description, char_t flag, bool& storage) :
        typed_argument<char_t, bool, false>(std::move(description), flag, &storage) {
    }

    /**
     * Create a switch basic_argument that is identified by a name, using an
     * external variable to store the value.
     * @param description Description of the basic_argument (used in help text)
     * @param name Name identifier of this basic_argument
     * @param storage Variable to store the value in
     */
    basic_switch_argument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, bool& storage) :
        typed_argument<char_t, bool, false>(std::move(description), name, &storage) {
    }

    /**
     * Create a switch basic_argument that is identified by both a flag and a name,
     * using an external variable to store the value.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param name Name identifier of this basic_argument
     * @param storage Variable to store the value in
     */
    basic_switch_argument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, bool& storage) :
        typed_argument<char_t, bool, false>(std::move(description), flag, name, &storage) {
    }

    /////////////////
    // Constructors for storing value internally
    /////////////////
#ifdef TAP_AUTOFLAG
    /**
     * Create a switch basic_argument with aliases defined by the description, using
     * an internal variable to store the value.
     * @param description Description of the basic_argument (used in help text). Must
     *        contain flag or name markers
     */
    basic_switch_argument(std::basic_string<char_t> description) :
        typed_argument<char_t, bool, false>(std::move(description), new bool()),
        m_ownStorage(typed_argument<char_t, bool, false>::m_storage) {
    }
#endif

    /**
     * Create a switch basic_argument that is identified by a flag, using an
     * external variable to store the value.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     */
    basic_switch_argument(std::basic_string<char_t> description, char_t flag) :
        typed_argument<char_t, bool, false>(std::move(description), flag, new bool()),
        m_ownStorage(typed_argument<char_t, bool, false>::m_storage) {
    }

    /**
     * Create a switch basic_argument that is identified by a name, using an
     * external variable to store the value.
     * @param description Description of the basic_argument (used in help text)
     * @param name Name identifier of this basic_argument
     */
    basic_switch_argument(std::basic_string<char_t> description, const std::basic_string<char_t>& name) :
        typed_argument<char_t, bool, false>(std::move(description), name, new bool()),
        m_ownStorage(typed_argument<char_t, bool, false>::m_storage) {
    }

    /**
     * Create a switch basic_argument that is identified by both a flag and a name,
     * using an external variable to store the value.
     * @param description Description of the basic_argument (used in help text)
     * @param flag Flag identifier of this basic_argument
     * @param name Name identifier of this basic_argument
     */
    basic_switch_argument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name) :
        typed_argument<char_t, bool, false>(std::move(description), flag, name, new bool()),
        m_ownStorage(typed_argument<char_t, bool, false>::m_storage)  {
    }

    /**
     * basic_switch_argument copy constructor.
     */
    basic_switch_argument(const basic_switch_argument&) = default;

    /**
     * basic_switch_argument move constructor.
     */
    basic_switch_argument(basic_switch_argument&&) = default;

    /**
     * basic_switch_argument destructor.
     */
    virtual ~basic_switch_argument() {}

    /**
     * basic_switch_argument assignment operator.
     */
    basic_switch_argument& operator=(const basic_switch_argument& other) = default;

    /**
     * basic_switch_argument move assignment operator.
     */
    basic_switch_argument& operator=(basic_switch_argument&& other) = default;

    /**
     * See basic_argument::takes_value()
     */
    bool takes_value() const override {
        return false;
    }

    /**
     * See basic_argument::set()
     */
    void set() const override {
        basic_argument<char_t>::set();
        *typed_argument<char_t, bool, false>::m_storage = !*typed_argument<char_t, bool, false>::m_storage;
    }

    /*
     * See base_argument::operator bool()
     */
    explicit operator bool() const {
        return typed_argument<char_t, bool, false>::is_set();
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() const & override {
        return std::unique_ptr<base_argument<char_t>>(new basic_switch_argument(*this));
    }

    /**
     * See base_argument::clone().
     */
    std::unique_ptr<base_argument<char_t>> clone() && override {
        return std::unique_ptr<base_argument<char_t>>(new basic_switch_argument(std::move(*this)));
    }
};


}


