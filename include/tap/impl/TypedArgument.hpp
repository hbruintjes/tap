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
/*
 * impl/ValuedArgument.hpp
 */

#pragma once

#include <sstream>

namespace TAP {

namespace detail {
    // This namespace constains some basic functionality
    // Currently value conversion operations


    /**
     * Stream value into storage, throws ArgException on failure.
     */
    template<typename T>
    inline bool setValue(const std::string& value, T& storage) {
        std::istringstream iss(value);
#ifdef TAP_STREAMSAFE
        T local = storage;
        iss >> local;
#else
        iss >> storage;
#endif
        if ( !iss || (iss.peek() != std::istringstream::traits_type::eof()) ) {
            return false;
        }
#ifdef TAP_STREAMSAFE
        storage = local;
#endif
        return true;
    }

    /**
     * Stream value into storage vector by appending as the last element, throws
     * ArgException on failure.
     */
    template<typename T>
    inline bool setValue(const std::string& value, std::vector<T>& storage) {
        T local;
        if (!setValue(value, local)) {
            return false;
        }
        storage.push_back(local);
        return true;
    }

    /**
     * Assign value to storage string
     */
    template<>
    inline bool setValue<std::string>(const std::string& value, std::string& storage) {
        storage = value;
        return true;
    }

    /**
     * Assign value to storage vector by appending as the last element.
     */
    template<>
    inline bool setValue< std::vector<std::string> >(const std::string& value, std::vector<std::string>& storage) {
        storage.push_back(value);
        return true;
    }

    /**
     * Placeholder for boolean assignment, this function should never be called.
     */
    template<>
    inline bool setValue<bool>(const std::string&, bool&) {
        throw std::logic_error("Assigning value to unvalued argument");
    }
}

template<typename T, bool multi>
inline void VariableArgument<T,multi>::set() const {
    throw std::logic_error("Calling set() on valued argument");
}

template<typename T, bool multi>
inline void VariableArgument<T,multi>::set(const std::string& value) const {
    // Load value
    if (!detail::setValue(value, *m_storage)) {
        throw argument_invalid_value(*this, value);
    }
    // Run any configured check function
    TypedArgument<T, multi>::check();
    // Mark argument set
    Argument::set();
}

template<typename T, bool multi>
inline std::string VariableArgument<T,multi>::usage() const {
    std::string usageStr;
    if (!this->m_isPositional) {
        if (this->m_flags.length() > 0u) {
            // Print first flag only, aliases generally not needed
            usageStr = std::string(flagStart) + this->m_flags[0];
        } else {
            usageStr = std::string(nameStart) + this->m_names[0];
        }

        usageStr += " ";
    }
    usageStr += m_valueName;

    if (this->m_isPositional && this->m_max != 1) {
        usageStr += "...";
    }

    return usageStr;
}

template<typename T, bool multi>
inline std::string VariableArgument<T,multi>::ident() const {
    if (!this->m_isPositional) {
        return Argument::ident();
    } else {
        // Identified by just the value name
        return m_valueName;
    }
}

}
