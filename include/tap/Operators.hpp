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
 * @file Operators.hpp
 * @brief Contains the definitions for basic_argument operators to create
 * basic_argument_constraints.
 */

#pragma once

namespace TAP {
/**
 * Joins two arguments together using the constraint_type::One operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::One operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::One> operator^(basic_argument<char_t>& left, basic_argument<char_t>& right);

/**
 * Joins two arguments together using the constraint_type::One operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::One operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::One> operator^(basic_argument_constraint<char_t, constraint_type::One> left, basic_argument<char_t>& right);

/**
 * Joins two arguments together using the constraint_type::One operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::One operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::One> operator^(basic_argument<char_t>& left, basic_argument_constraint<char_t, constraint_type::One>& right);

/**
 * Joins two arguments together using the constraint_type::Any operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::One operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::Any> operator|(basic_argument<char_t>& left, basic_argument<char_t>& right);

/**
 * Joins two arguments together using the constraint_type::Any operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::Any operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::Any> operator|(basic_argument_constraint<char_t, constraint_type::Any> left, basic_argument<char_t>& right);

/**
 * Joins two arguments together using the constraint_type::Any operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::Any operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::Any> operator|(basic_argument<char_t>& left, basic_argument_constraint<char_t, constraint_type::Any>& right);

/**
 * Joins two arguments together using the constraint_type::Imp operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::All operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::Imp> operator>(basic_argument<char_t>& left, basic_argument<char_t>& right);

/**
 * Joins two arguments together using the constraint_type::Imp operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::All operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::Imp> operator>(basic_argument_constraint<char_t, constraint_type::Imp> left, basic_argument<char_t>& right);

/**
 * Joins two arguments together using the constraint_type::Imp operator.
 * @param left Left basic_argument
 * @param right Right basic_argument
 * @return An basic_argument_constraint containing the given arguments, using the
 *         constraint_type::All operator
 */
template<typename char_t>
basic_argument_constraint<char_t, constraint_type::Imp> operator>(basic_argument<char_t>& left, basic_argument_constraint<char_t, constraint_type::Imp>& right);

/**
 * Makes an basic_argument optional.
 * @param arg basic_argument to make optional
 * @return input basic_argument
 */
template<typename char_t>
basic_argument<char_t>& operator-(basic_argument<char_t>& arg);

/**
 * Makes an basic_argument constraint optional.
 * @param arg basic_argument to make optional
 * @return input basic_argument
 */
template<typename char_t, constraint_type CType>
basic_argument_constraint<char_t, CType>& operator-(basic_argument_constraint<char_t, CType>& arg);

/**
 * Makes an basic_argument required.
 * @param arg basic_argument to make required
 * @return input basic_argument
 */
template<typename char_t>
basic_argument<char_t>& operator+(basic_argument<char_t>& arg);

/**
 * Makes an basic_argument required.
 * @param arg basic_argument to make required
 * @return input basic_argument
 */
template<typename char_t>
basic_argument<char_t>&& operator+(basic_argument<char_t>&& arg);

/**
 * Makes an basic_argument constraint required.
 * @param arg basic_argument constraint to make required
 * @return input basic_argument constraint
 */
template<typename char_t, constraint_type CType>
basic_argument_constraint<char_t, CType>& operator+(basic_argument_constraint<char_t, CType>& arg);

/**
 * Makes an basic_argument constraint required.
 * @param arg basic_argument constraint to make required
 * @return input basic_argument constraint
 */
template<typename char_t, constraint_type CType>
basic_argument_constraint<char_t, CType>&& operator+(basic_argument_constraint<char_t, CType>&& arg);

}


