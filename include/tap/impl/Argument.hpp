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

namespace TAP {

inline void Argument::check_valid() const {
    unsigned c = count();
    if (c == 0) {
        if (m_required) {
            throw argument_count_mismatch(*this, c, 1);
        } else {
            return;
        }
    }
    if (c < m_min) {
        throw argument_count_mismatch(*this, c, m_min);
    } else if (c > m_max && m_max != 0) {
        throw argument_count_mismatch(*this, c, m_max);
    }
}

/**
 * See BaseArgument::usage()
 */
inline std::string Argument::usage() const {
    std::string usageStr;
    if (m_flags.length() > 0u) {
        // Print first flag only, aliases generally not needed
        usageStr = std::string(flagStart) + m_flags.at(0);
    } else if (m_names.size() > 0u) {
        usageStr = std::string(nameStart) + m_names.at(0);
    } else {
        // else positional, needs an override
        throw std::logic_error("Base usage() called on positional argument");
    }

    return usageStr;
}

/**
 * Print a string representation of this argument to the given stream. This
 * is usually represented in the first column of help text.
 * @return String representation.
 */
inline std::string Argument::ident() const {
    std::string ident;
    if (m_flags.length() > 0u) {
        // Print first flag only, aliases generally not needed
        ident += std::string(flagStart) + m_flags.at(0);
    }

    if (m_names.size() > 0u) {
        // Print first name only, aliases generally not needed
        if (m_flags.length() > 0u) {
            ident += ", ";
        }
        ident += std::string(nameStart) + m_names.at(0);
    }

    // if positional, needs override

    return ident;
}

}
