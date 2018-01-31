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
 * @brief Contains the definitions for Argument operators to create
 * ArgumentConstraints.
 */

#pragma once

namespace TAP {
/**
 * Joins two arguments together using the ConstraintType::One operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::One operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::One> operator^(Argument<char_t>& left, Argument<char_t>& right);

/**
 * Joins two arguments together using the ConstraintType::One operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::One operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::One> operator^(ArgumentConstraint<char_t, ConstraintType::One> left, Argument<char_t>& right);

/**
 * Joins two arguments together using the ConstraintType::One operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::One operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::One> operator^(Argument<char_t>& left, ArgumentConstraint<char_t, ConstraintType::One>& right);

/**
 * Joins two arguments together using the ConstraintType::Any operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::One operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::Any> operator|(Argument<char_t>& left, Argument<char_t>& right);

/**
 * Joins two arguments together using the ConstraintType::Any operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::Any operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::Any> operator|(ArgumentConstraint<char_t, ConstraintType::Any> left, Argument<char_t>& right);

/**
 * Joins two arguments together using the ConstraintType::Any operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::Any operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::Any> operator|(Argument<char_t>& left, ArgumentConstraint<char_t, ConstraintType::Any>& right);

/**
 * Joins two arguments together using the ConstraintType::Imp operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::All operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::Imp> operator>(Argument<char_t>& left, Argument<char_t>& right);

/**
 * Joins two arguments together using the ConstraintType::Imp operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::All operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::Imp> operator>(ArgumentConstraint<char_t, ConstraintType::Imp> left, Argument<char_t>& right);

/**
 * Joins two arguments together using the ConstraintType::Imp operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::All operator
 */
template<typename char_t>
ArgumentConstraint<char_t, ConstraintType::Imp> operator>(Argument<char_t>& left, ArgumentConstraint<char_t, ConstraintType::Imp>& right);

/**
 * Makes an argument optional.
 * @param arg Argument to make optional
 * @return input Argument
 */
template<typename char_t>
Argument<char_t>& operator-(Argument<char_t>& arg);

/**
 * Makes an argument constraint optional.
 * @param arg Argument to make optional
 * @return input Argument
 */
template<typename char_t, ConstraintType CType>
ArgumentConstraint<char_t, CType>& operator-(ArgumentConstraint<char_t, CType>& arg);

/**
 * Makes an argument required.
 * @param arg Argument to make required
 * @return input Argument
 */
template<typename char_t>
Argument<char_t>& operator+(Argument<char_t>& arg);

/**
 * Makes an argument required.
 * @param arg Argument to make required
 * @return input Argument
 */
template<typename char_t>
Argument<char_t>&& operator+(Argument<char_t>&& arg);

/**
 * Makes an argument constraint required.
 * @param arg Argument constraint to make required
 * @return input Argument constraint
 */
template<typename char_t, ConstraintType CType>
ArgumentConstraint<char_t, CType>& operator+(ArgumentConstraint<char_t, CType>& arg);

/**
 * Makes an argument constraint required.
 * @param arg Argument constraint to make required
 * @return input Argument constraint
 */
template<typename char_t, ConstraintType CType>
ArgumentConstraint<char_t, CType>&& operator+(ArgumentConstraint<char_t, CType>&& arg);

}


