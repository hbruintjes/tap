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
ArgumentConstraint<ConstraintType::One> operator^(Argument& left, Argument& right);

/**
 * Joins two arguments together using the ConstraintType::One operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::One operator
 */
ArgumentConstraint<ConstraintType::One> operator^(ArgumentConstraint<ConstraintType::One> left, Argument& right);

/**
 * Joins two arguments together using the ConstraintType::One operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::One operator
 */
ArgumentConstraint<ConstraintType::One> operator^(Argument& left, ArgumentConstraint<ConstraintType::One>& right);

/**
 * Joins two arguments together using the ConstraintType::Any operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::One operator
 */
ArgumentConstraint<ConstraintType::Any> operator|(Argument& left, Argument& right);

/**
 * Joins two arguments together using the ConstraintType::Any operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::Any operator
 */
ArgumentConstraint<ConstraintType::Any> operator|(ArgumentConstraint<ConstraintType::Any> left, Argument& right);

/**
 * Joins two arguments together using the ConstraintType::Any operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::Any operator
 */
ArgumentConstraint<ConstraintType::Any> operator|(Argument& left, ArgumentConstraint<ConstraintType::Any>& right);

/**
 * Joins two arguments together using the ConstraintType::Imp operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::All operator
 */
ArgumentConstraint<ConstraintType::Imp> operator>(Argument& left, Argument& right);

/**
 * Joins two arguments together using the ConstraintType::Imp operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::All operator
 */
ArgumentConstraint<ConstraintType::Imp> operator>(ArgumentConstraint<ConstraintType::Imp> left, Argument& right);

/**
 * Joins two arguments together using the ConstraintType::Imp operator.
 * @param left Left argument
 * @param right Right argument
 * @return An ArgumentConstraint containing the given arguments, using the
 *         ConstraintType::All operator
 */
ArgumentConstraint<ConstraintType::Imp> operator>(Argument& left, ArgumentConstraint<ConstraintType::Imp>& right);

/**
 * Makes an argument optional.
 * @param arg Argument to make optional
 * @return input Argument
 */
Argument& operator-(Argument& arg);

/**
 * Makes an argument constraint optional.
 * @param arg Argument to make optional
 * @return input Argument
 */
template<ConstraintType CType>
ArgumentConstraint<CType>& operator-(ArgumentConstraint<CType>& arg);

/**
 * Makes an argument required.
 * @param arg Argument to make required
 * @return input Argument
 */
Argument& operator+(Argument& arg);

/**
 * Makes an argument required.
 * @param arg Argument to make required
 * @return input Argument
 */
Argument&& operator+(Argument&& arg);

/**
 * Makes an argument constraint required.
 * @param arg Argument constraint to make required
 * @return input Argument constraint
 */
template<ConstraintType CType>
ArgumentConstraint<CType>& operator+(ArgumentConstraint<CType>& arg);

/**
 * Makes an argument constraint required.
 * @param arg Argument constraint to make required
 * @return input Argument constraint
 */
template<ConstraintType CType>
ArgumentConstraint<CType>&& operator+(ArgumentConstraint<CType>&& arg);

}


