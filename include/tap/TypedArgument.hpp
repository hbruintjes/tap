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
 * @file TypedArgument.hpp
 * @brief Contains the definitions for ValueArguments.
 */

#pragma once

#include <type_traits>

namespace TAP {

template<typename char_t, typename T, bool multi = false>
class TypedArgument;

/** Function pointer that is used by ValueArgument::check() */
template<typename char_t, typename T, bool multi>
using TypedArgumentCheckFunc = std::function<void(const TypedArgument<char_t, T, multi>&, const T& value)>;

/**
 * Base class for arguments that hold a typed value. The class can optionally be
 * set to store multiple values in a vector. The check_typed function should be
 * used instead of check.
 * Template parameter T indicates the type of the value to store.
 * Template parameter multi indicates if multiple values should be stored.
 * Defaults to false. Note that if multi is false, but the argument can be set
 * multiple times, the value is overwritten each time.
 * Note that this class cannot be used directly, a derived class is to be used.
 */
template<typename char_t, typename T, bool multi>
class TypedArgument: public Argument<char_t> {
    static_assert(!std::is_void<T>::value, "Cannot make void arguments, use plain Argument");
    static_assert(!std::is_const<T>::value, "Cannot make const arguments");
    static_assert(!std::is_volatile<T>::value, "Cannot make volatile arguments");
    static_assert(!std::is_reference<T>::value, "Cannot make reference arguments");
    static_assert(!std::is_pointer<T>::value, "Cannot make pointer arguments");

protected:
    /** Alias for the storage type (which is a vector for MultiValueArgument
     * types) */
    using ST = typename std::conditional< !multi, T, std::vector<T> >::type;

    /** Pointer to value storage */
    mutable ST* m_storage;

    /** Callback for typed check */
    TypedArgumentCheckFunc<char_t, T, multi> m_typedCheckFunc = nullptr;

protected:
    /**
     * Create a TypedArgument with given description and storage pointer. Values
     * will be stored in the variable pointed to.
     * @param description Description of the argument (used in help text)
     * @param storage Pointer to storage variable
     */
    TypedArgument(std::basic_string<char_t> description, ST* storage) :
        Argument<char_t>(std::move(description)), m_storage(storage) {
        Argument<char_t>::m_max = (multi?0:1);
    }

    /**
     * Create a TypedArgument with given description and storage pointer,
     * aliased to the given flag. Values will be stored in the variable pointed
     * to.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param storage Pointer to storage variable
     */
    TypedArgument(std::basic_string<char_t> description, char_t flag, ST* storage) :
        Argument<char_t>(std::move(description), flag), m_storage(storage) {
        Argument<char_t>::m_max = (multi?0:1);
    }

    /**
     * Create a TypedArgument with given description and storage pointer,
     * aliased to the given name. Values will be stored in the variable pointed
     * to.
     * @param description Description of the argument (used in help text)
     * @param name Name identifier of this argument
     * @param storage Pointer to storage variable
     */
    TypedArgument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, ST* storage) :
        Argument<char_t>(std::move(description), name), m_storage(storage) {
        Argument<char_t>::m_max = (multi?0:1);
    }

    /**
     * Create a TypedArgument with given description and storage pointer,
     * aliased to the given flag and name. Values will be stored in the variable
     * pointed to.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param name Name identifier of this argument
     * @param storage Pointer to storage variable
     */
    TypedArgument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, ST* storage) :
        Argument<char_t>(std::move(description), flag, name), m_storage(storage) {
        Argument<char_t>::m_max = (multi?0:1);
    }

public:
    /**
     * See Argument::set()
     */
    void set() const override = 0;

    /**
     * See Argument::takes_value()
     */
    bool takes_value() const override = 0;

    /**
     * Returns the value of this argument.
     * @return Reference to the value of this argument.
     */
    const ST& value() const {
        return *m_storage;
    }

    /**
     * See Argument::check()
     */
    virtual TypedArgument& check_typed(TypedArgumentCheckFunc<char_t, T, multi> typedCheckFunc) {
        m_typedCheckFunc = typedCheckFunc;
        return *this;
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() const & override = 0;

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() && override = 0;

protected:
    /**
     * See Argument::check()
     */
    void
    check() const override {
        doCheck();
    }

    /**
     * See Argument::check()
     */
    template<bool m = multi>
    typename std::enable_if<!m>::type
    doCheck() const {
        if (m_typedCheckFunc != nullptr) {
            m_typedCheckFunc(*this, *m_storage );
        }
    }

    /**
     * See Argument::check()
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
 * Concrete implementation of TypedArgument. Simply takes a value and stores it
 * in a variable.
 */
template<typename char_t, typename T, bool multi = false>
class VariableArgument : public TypedArgument<char_t, T, multi>, public ValueAcceptor<char_t> {
    static_assert(!std::is_void<T>::value, "Cannot make void arguments, use Argument");

protected:
    /** Make TypedArgument::ST accessible. */
    using ST = typename TypedArgument<char_t, T, multi>::ST;

    /** Name of the accepted value, used in the identifier */
    std::basic_string<char_t> m_valueName = std::basic_string<char_t>("value");

public:
    /**
     * Create a VariableArgument with given description and storage pointer.
     * Values will be stored in the variable pointed to.
     * @param description Description of the argument (used in help text)
     * @param storage Pointer to storage variable
     */
    VariableArgument(std::basic_string<char_t> description, ST* storage) :
        TypedArgument<char_t, T, multi>(std::move(description), storage) {
    }

    /**
     * Create a VariableArgument with given description and storage pointer,
     * aliased to the given flag. Values will be stored in the variable pointed
     * to.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param storage Pointer to storage variable
     */
    VariableArgument(std::basic_string<char_t> description, char_t flag, ST* storage) :
        TypedArgument<char_t, T, multi>(std::move(description), flag, storage) {
    }

    /**
     * Create a VariableArgument with given description and storage pointer,
     * aliased to the given name. Values will be stored in the variable pointed
     * to.
     * @param description Description of the argument (used in help text)
     * @param name Name identifier of this argument
     * @param storage Pointer to storage variable
     */
    VariableArgument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, ST* storage) :
        TypedArgument<char_t, T, multi>(std::move(description), name, storage) {
    }

    /**
     * Create a VariableArgument with given description and storage pointer,
     * aliased to the given flag and name. Values will be stored in the variable
     * pointed to.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param name Name identifier of this argument
     * @param storage Pointer to storage variable
     */
    VariableArgument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, ST* storage) :
        TypedArgument<char_t, T, multi>(std::move(description), flag, name, storage) {
    }

    /**
     * Create a VariableArgument with given description and storage reference.
     * Values will be stored in the variable referenced.
     * @param description Description of the argument (used in help text)
     * @param storage Pointer to storage variable
     */
    VariableArgument(std::basic_string<char_t> description, ST& storage) :
        TypedArgument<char_t, T, multi>(std::move(description), &storage) {
    }

    /**
     * Create a VariableArgument with given description and storage reference,
     * aliased to the given flag. Values will be stored in the variable
     * referenced.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param storage Pointer to storage variable
     */
    VariableArgument(std::basic_string<char_t> description, char_t flag, ST& storage) :
        TypedArgument<char_t, T, multi>(std::move(description), flag, &storage) {
    }

    /**
     * Create a VariableArgument with given description and storage reference,
     * aliased to the given name. Values will be stored in the variable
     * referenced.
     * @param description Description of the argument (used in help text)
     * @param name Name identifier of this argument
     * @param storage Pointer to storage variable
     */
    VariableArgument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, ST& storage) :
        TypedArgument<char_t, T, multi>(std::move(description), name, &storage) {
    }

    /**
     * Create a VariableArgument with given description and storage reference,
     * aliased to the given flag and name. Values will be stored in the variable
     * referenced.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param name Name identifier of this argument
     * @param storage Pointer to storage variable
     */
    VariableArgument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, ST& storage) :
        TypedArgument<char_t, T, multi>(std::move(description), flag, name, &storage) {
    }

    /**
     * VariableArgument destructor.
     */
    virtual ~VariableArgument() = default;

    /**
     * VariableArgument copy constructor.
     */
    VariableArgument(const VariableArgument&) = default;

    /**
     * VariableArgument move constructor.
     */
    VariableArgument(VariableArgument&&) = default;

    /**
     * VariableArgument copy assignment operator.
     */
    VariableArgument& operator=(const VariableArgument&) = default;

    /**
     * VariableArgument move assignment operator.
     */
    VariableArgument& operator=(VariableArgument&&) = default;

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() const & override {
        return std::unique_ptr<BaseArgument<char_t>>(new VariableArgument(*this));
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() && override {
        return std::unique_ptr<BaseArgument<char_t>>(new VariableArgument(std::move(*this)));
    }

    /**
     * Assign a human readable description for the value, used when describing
     * the argument. Default is 'value'.
     * @param valueName String to describe the name of the value.
     * @return Reference to this argument
     */
    VariableArgument& valuename(const std::basic_string<char_t>& valueName) {
        m_valueName = valueName;
        return *this;
    }

    /**
     * Return a human readable description for the value, used when describing
     * the argument. Default is 'value'.
     * @return String representing the value argument in help text
     */
    const std::basic_string<char_t>& valuename() {
        return m_valueName;
    }

    /**
     * See Argument::takes_value()
     */
    bool takes_value() const override {
        return !std::is_same<bool, T>::value;
    }

    /**
     * Calling this function is an error, call set(std::basic_string<char_t>) instead.
     */
    void set() const override;

    /**
     * See ValueAcceptor::set()
     */
    void set(const std::basic_string<char_t>& value) const override;

    /**
     * See Argument::usage()
     */
    std::basic_string<char_t> usage() const override;

    /**
     * See Argument::ident()
     */
    std::basic_string<char_t> ident() const override;
};

/**
 * MultiVariableArgument is a VariableArgument that, when allowed to occur
 * multiple times, stores each given value individually in a vector.
 * Alias for VariableArgument with the multi template argument set to true.
 */
template<typename char_t, typename T>
using MultiVariableArgument = VariableArgument<char_t, T, true>;

/**
 * Implementation of TypedArgument that stores a value by itself, based on
 * VariableArgument.
 */
template<typename char_t, typename T, bool multi = false>
class ValueArgument : public VariableArgument<char_t, T, multi> {
protected:
    /** Make TypedArgument::ST accessible. */
    using ST = typename VariableArgument<char_t, T, multi>::ST;

    /** If the ValueArgument owns the variable storage, it is stored here */
    std::shared_ptr<ST> m_ownStorage;

public:
    /**
     * Create a positional ValueArgument using an internal variable to store
     * the value. The parameter list is used to construct the value.
     * @param description Description of the argument (used in help text)
     * @param params Arguments passed to the constructor of the value storage
     */
    template<typename... U, typename = typename std::enable_if< std::is_constructible<ST, U...>::value >::type >
    ValueArgument(std::basic_string<char_t> description, U&&... params) :
        VariableArgument<char_t, T, multi>(std::move(description), new ST(std::forward<U>(params)...)),
        m_ownStorage(TypedArgument<char_t, T, multi>::m_storage)
    {
    }

    /**
     * Create a ValueArgument using an internal variable to store the value,
     * aliased to the given flag. The parameter list is used to construct the
     * value.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param params Arguments passed to the constructor of the value storage
     */
    template<typename... U, typename = typename std::enable_if< std::is_constructible<ST, U...>::value >::type>
    ValueArgument(std::basic_string<char_t> description, char_t flag, U&&... params) :
        VariableArgument<char_t, T, multi>(std::move(description), flag, new ST(std::forward<U>(params)...)),
        m_ownStorage(TypedArgument<char_t, T, multi>::m_storage)
    {
    }

    /**
     * Create a ValueArgument using an internal variable to store the value,
     * aliased to the given flag. The parameter list is used to construct the
     * value.
     * @param description Description of the argument (used in help text)
     * @param name Name identifier of this argument
     * @param params Arguments passed to the constructor of the value storage
     */
    template<typename... U, typename = typename std::enable_if< std::is_constructible<ST, U...>::value >::type >
    ValueArgument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, U&&... params) :
        VariableArgument<char_t, T, multi>(std::move(description), name, new ST(std::forward<U>(params)...)),
        m_ownStorage(TypedArgument<char_t, T, multi>::m_storage)
    {
    }

    /**
     * Create a ValueArgument using an internal variable to store the value,
     * aliased to the given flag and name. The parameter list is used to
     * construct the value.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param name Name identifier of this argument
     * @param params Arguments passed to the constructor of the value storage
     */
    template<typename... U, typename = typename std::enable_if< std::is_constructible<ST, U...>::value >::type >
    ValueArgument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, U&&... params) :
        VariableArgument<char_t, T, multi>(std::move(description), flag, name, new ST(std::forward<U>(params)...)),
        m_ownStorage(TypedArgument<char_t, T, multi>::m_storage)
    {
    }

    /**
     * ValueArgument destructor.
     */
    virtual ~ValueArgument() = default;

    /**
     * ValueArgument copy constructor.
     */
    ValueArgument(const ValueArgument&) = default;

    /**
     * ValueArgument move constructor.
     */
    ValueArgument(ValueArgument&&) = default;

    /**
     * ValueArgument copy assignment operator. Will copy the stored value if
     * the other ValueArgument owns the variable, otherwise simply copies the
     * pointer.
     */
    ValueArgument& operator=(const ValueArgument&) = default;

    /**
     * ValueArgument move assignment operator.
     */
    ValueArgument& operator=(ValueArgument&&) = default;

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() const & override {
        return std::unique_ptr<BaseArgument<char_t>>(new ValueArgument(*this));
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() && override {
        return std::unique_ptr<BaseArgument<char_t>>(new ValueArgument(std::move(*this)));
    }
};

/**
 * MultiValueArgument is a ValueArgument that, when allowed to occur multiple
 * times, stores each given value individually in a vector.
 * Alias for ValueArgument with the multi template argument set to true.
 */
template<typename char_t, typename T>
using MultiValueArgument = ValueArgument<char_t, T, true>;

/**
 * Specialization of TypedArgument that acts as a switch on the command line,
 * but stores an arbitrary constant in the given variable storage. Practical for
 * for instance enumerations.
 */
template<typename char_t, typename T>
class ConstArgument: public TypedArgument<char_t, T, false> {
protected:
    /** Constant to store */
    T m_value;

public:
#ifdef TAP_AUTOFLAG
    /**
     * Create a constant argument with aliases defined by the description, using
     * an external variable to store a predefined value.
     * @param description Description of the argument (used in help text). Must
     *        contain flag or name markers
     * @param storage Variable to store the value in
     * @param value Value to set the variable to when this argument is set
     */
    ConstArgument(std::basic_string<char_t> description, T& storage, const T& value) :
        TypedArgument<char_t, T, false>(std::move(description), &storage), m_value(value) {
    }
#endif

    /**
     * Create a constant argument that is identified by a flag, using an
     * external variable to store a predefined value.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param storage Variable to store the value in
     * @param value Value to set the variable to when this argument is set
     */
    ConstArgument(std::basic_string<char_t> description, char_t flag, T& storage, const T& value) :
        TypedArgument<char_t, T, false>(std::move(description), flag, &storage), m_value(value) {
    }

    /**
     * Create a constant argument that is identified by a name, using an
     * external variable to store a predefined value.
     * @param description Description of the argument (used in help text)
     * @param name Name identifier of this argument
     * @param storage Variable to store the value in
     * @param value Value to set the variable to when this argument is set
     */
    ConstArgument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, T& storage, const T& value) :
        TypedArgument<char_t, T, false>(std::move(description), name, &storage), m_value(value) {
    }

    /**
     * Create a constant argument that is identified by both a flag and a name,
     * using an external variable to store a predefined value.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param name Name identifier of this argument
     * @param storage Variable to store the value in
     * @param value Value to set the variable to when this argument is set
     */
    ConstArgument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, T& storage, const T& value) :
        TypedArgument<char_t, T, false>(std::move(description), flag, name, &storage), m_value(value) {
    }

    /**
     * ConstArgument copy constructor.
     */
    ConstArgument(const ConstArgument&) = default;
    /**
     * ConstArgument move constructor.
     */
    ConstArgument(ConstArgument&&) = default;

    /**
     * ConstArgument destructor.
     */
    virtual ~ConstArgument() {
    }

    /**
     * ConstArgument assignment operator.
     */
    ConstArgument& operator=(const ConstArgument& other) = default;

    /**
     * ConstArgument move assignment operator.
     */
    ConstArgument& operator=(ConstArgument&& other) = default;

    /**
     * See Argument::takes_value()
     */
    bool takes_value() const override {
        return false;
    }

    /**
     * See Argument::set()
     */
    void set() const override {
        Argument<char_t>::set();
        *TypedArgument<char_t, T, false>::m_storage = m_value;
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() const & override {
        return std::unique_ptr<BaseArgument<char_t>>(new ConstArgument(*this));
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() && override {
        return std::unique_ptr<BaseArgument<char_t>>(new ConstArgument(std::move(*this)));
    }
};

/**
 * SwitchArgument is a specialization for TypedArgument, which does not
 * accept values (it switches). When allowed to occur multiple times, the value
 * is inverted each time it occurs. Use this to keep track of the actual value
 * in a variable.
 */
template<typename char_t>
class SwitchArgument : public TypedArgument<char_t, bool, false> {
protected:
    /** If the SwitchArgument owns the variable storage, it is stored here */
    std::shared_ptr<bool> m_ownStorage;

public:
    /////////////////
    // Constructors for storing value externally
    /////////////////
#ifdef TAP_AUTOFLAG
    /**
     * Create a switch argument with aliases defined by the description, using
     * an external variable to store the value.
     * @param description Description of the argument (used in help text). Must
     *        contain flag or name markers
     * @param storage Variable to store the value in
     */
    SwitchArgument(std::basic_string<char_t> description, bool& storage) :
        TypedArgument<char_t, bool, false>(std::move(description), &storage) {
    }
#endif

    /**
     * Create a switch argument that is identified by a flag, using an
     * external variable to store the value.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param storage Variable to store the value in
     */
    SwitchArgument(std::basic_string<char_t> description, char_t flag, bool& storage) :
        TypedArgument<char_t, bool, false>(std::move(description), flag, &storage) {
    }

    /**
     * Create a switch argument that is identified by a name, using an
     * external variable to store the value.
     * @param description Description of the argument (used in help text)
     * @param name Name identifier of this argument
     * @param storage Variable to store the value in
     */
    SwitchArgument(std::basic_string<char_t> description, const std::basic_string<char_t>& name, bool& storage) :
        TypedArgument<char_t, bool, false>(std::move(description), name, &storage) {
    }

    /**
     * Create a switch argument that is identified by both a flag and a name,
     * using an external variable to store the value.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param name Name identifier of this argument
     * @param storage Variable to store the value in
     */
    SwitchArgument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name, bool& storage) :
        TypedArgument<char_t, bool, false>(std::move(description), flag, name, &storage) {
    }

    /////////////////
    // Constructors for storing value internally
    /////////////////
#ifdef TAP_AUTOFLAG
    /**
     * Create a switch argument with aliases defined by the description, using
     * an internal variable to store the value.
     * @param description Description of the argument (used in help text). Must
     *        contain flag or name markers
     */
    SwitchArgument(std::basic_string<char_t> description) :
        TypedArgument<char_t, bool, false>(std::move(description), new bool()),
        m_ownStorage(TypedArgument<char_t, bool, false>::m_storage) {
    }
#endif

    /**
     * Create a switch argument that is identified by a flag, using an
     * external variable to store the value.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     */
    SwitchArgument(std::basic_string<char_t> description, char_t flag) :
        TypedArgument<char_t, bool, false>(std::move(description), flag, new bool()),
        m_ownStorage(TypedArgument<char_t, bool, false>::m_storage) {
    }

    /**
     * Create a switch argument that is identified by a name, using an
     * external variable to store the value.
     * @param description Description of the argument (used in help text)
     * @param name Name identifier of this argument
     */
    SwitchArgument(std::basic_string<char_t> description, const std::basic_string<char_t>& name) :
        TypedArgument<char_t, bool, false>(std::move(description), name, new bool()),
        m_ownStorage(TypedArgument<char_t, bool, false>::m_storage) {
    }

    /**
     * Create a switch argument that is identified by both a flag and a name,
     * using an external variable to store the value.
     * @param description Description of the argument (used in help text)
     * @param flag Flag identifier of this argument
     * @param name Name identifier of this argument
     */
    SwitchArgument(std::basic_string<char_t> description, char_t flag, const std::basic_string<char_t>& name) :
        TypedArgument<char_t, bool, false>(std::move(description), flag, name, new bool()),
        m_ownStorage(TypedArgument<char_t, bool, false>::m_storage)  {
    }

    /**
     * SwitchArgument copy constructor.
     */
    SwitchArgument(const SwitchArgument&) = default;

    /**
     * SwitchArgument move constructor.
     */
    SwitchArgument(SwitchArgument&&) = default;

    /**
     * SwitchArgument destructor.
     */
    virtual ~SwitchArgument() {}

    /**
     * SwitchArgument assignment operator.
     */
    SwitchArgument& operator=(const SwitchArgument& other) = default;

    /**
     * SwitchArgument move assignment operator.
     */
    SwitchArgument& operator=(SwitchArgument&& other) = default;

    /**
     * See Argument::takes_value()
     */
    bool takes_value() const override {
        return false;
    }

    /**
     * See Argument::set()
     */
    void set() const override {
        Argument<char_t>::set();
        *TypedArgument<char_t, bool, false>::m_storage = !*TypedArgument<char_t, bool, false>::m_storage;
    }

    /*
     * See BaseArgument::operator bool()
     */
    explicit operator bool() const {
        return TypedArgument<char_t, bool, false>::is_set();
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() const & override {
        return std::unique_ptr<BaseArgument<char_t>>(new SwitchArgument(*this));
    }

    /**
     * See BaseArgument::clone().
     */
    std::unique_ptr<BaseArgument<char_t>> clone() && override {
        return std::unique_ptr<BaseArgument<char_t>>(new SwitchArgument(std::move(*this)));
    }
};


}


