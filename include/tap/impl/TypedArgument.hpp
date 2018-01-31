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
 * impl/Valuedargument.hpp
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
    template<typename char_t, typename T>
    inline bool setValue(const std::basic_string<char_t>& value, T& storage) {
        std::basic_istringstream<char_t> iss(value);
#ifdef TAP_STREAMSAFE
        T local = storage;
        iss >> local;
#else
        iss >> storage;
#endif
        if ( !iss || (iss.peek() != std::basic_istringstream<char_t>::traits_type::eof()) ) {
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
    template<typename char_t, typename T>
    inline bool setValue(const std::basic_string<char_t>& value, std::vector<T>& storage) {
        T local;
        if (!setValue(value, local)) {
            return false;
        }
        storage.push_back(local);
        return true;
    }

    //TODO: Fix specializations for string and bool
    /**
     * Assign value to storage string
     */
     /*
    template<typename char_t>
    inline bool setValue<char_t, std::basic_string<char_t>>(const std::basic_string<char_t>& value, std::basic_string<char_t>& storage) {
        storage = value;
        return true;
    }
  */

    /**
     * Assign value to storage vector by appending as the last element.
     */
/*
    template<typename char_t>
    inline bool setValue< char_t, std::vector<std::basic_string<char_t>> >(const std::basic_string<char_t>& value, std::vector<std::basic_string<char_t>>& storage) {
        storage.push_back(value);
        return true;
    }
*/
    /**
     * Placeholder for boolean assignment, this function should never be called.
     */
/*
    template<typename char_t>
    inline bool setValue<char_t, bool>(const std::basic_string<char_t>&, bool&) {
        throw std::logic_error("Assigning value to unvalued basic_argument");
    }
    */
}

template<typename char_t, typename T, bool multi>
inline void basic_variable_argument<char_t, T, multi>::set() const {
    throw std::logic_error("Calling set() on valued basic_argument");
}

template<typename char_t, typename T, bool multi>
inline void basic_variable_argument<char_t, T, multi>::set(const std::basic_string<char_t>& value) const {
    // Load value
    if (!detail::setValue(value, *typed_argument<char_t, T, multi>::m_storage)) {
        throw argument_invalid_value(*this, value);
    }
    // Run any configured check function
    typed_argument<char_t, T, multi>::check();
    // Mark basic_argument set
    basic_argument<char_t>::set();
}

template<typename char_t, typename T, bool multi>
inline std::basic_string<char_t> basic_variable_argument<char_t, T, multi>::usage() const {
    std::basic_string<char_t> usageStr;
    if (!basic_argument<char_t>::m_isPositional) {
        if (basic_argument<char_t>::m_flags.length() > 0u) {
            // Print first flag only, aliases generally not needed
            usageStr = widen_const<char_t>(flagStart) + basic_argument<char_t>::m_flags[0];
        } else {
            usageStr = widen_const<char_t>(nameStart) + basic_argument<char_t>::m_names[0];
        }

        usageStr += widen_const<char_t>(' ');
    }
    usageStr += m_valueName;

    if (basic_argument<char_t>::m_isPositional && basic_argument<char_t>::m_max != 1) {
        usageStr += widen_const<char_t>("...");
    }

    return usageStr;
}

template<typename char_t, typename T, bool multi>
inline std::basic_string<char_t> basic_variable_argument<char_t, T, multi>::ident() const {
    if (!basic_argument<char_t>::m_isPositional) {
        return basic_argument<char_t>::ident();
    } else {
        // Identified by just the value name
        return m_valueName;
    }
}

}
