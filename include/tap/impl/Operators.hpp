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

inline ArgumentConstraint<ConstraintType::One> operator^(Argument& left, Argument& right) {
    return ArgumentConstraint<ConstraintType::One>(left, right);
}

inline ArgumentConstraint<ConstraintType::One> operator^(ArgumentConstraint<ConstraintType::One> left, Argument& right) {
    left += right;
    return left;
}

inline ArgumentConstraint<ConstraintType::One> operator^(Argument& left, ArgumentConstraint<ConstraintType::One>& right) {
    return right | left;
}

inline ArgumentConstraint<ConstraintType::Any> operator|(Argument& left, Argument& right) {
    return ArgumentConstraint<ConstraintType::Any>(left, right);
}

inline ArgumentConstraint<ConstraintType::Any> operator|(ArgumentConstraint<ConstraintType::Any> left, Argument& right) {
    left += right;
    return left;
}

inline ArgumentConstraint<ConstraintType::Any> operator|(Argument& left, ArgumentConstraint<ConstraintType::Any>& right) {
    return right | left;
}

inline ArgumentConstraint<ConstraintType::All> operator&(Argument& left, Argument& right) {
    return ArgumentConstraint<ConstraintType::All>(left, right);
}

inline ArgumentConstraint<ConstraintType::All> operator&(ArgumentConstraint<ConstraintType::All> left, Argument& right) {
    left += right;
    return left;
}

inline ArgumentConstraint<ConstraintType::All> operator&(Argument& left, ArgumentConstraint<ConstraintType::All>& right) {
    return right & left;
}

inline ArgumentConstraint<ConstraintType::None> operator~(Argument& arg) {
    return ArgumentConstraint<ConstraintType::None>(arg);
}

template<ConstraintType CType>
inline ArgumentConstraint<ConstraintType::None> operator~(ArgumentConstraint<CType>& arg) {
    return ArgumentConstraint<ConstraintType::None>(arg);
}

inline Argument& operator-(Argument& arg) {
    arg.set_required(false);
    return arg;
}

template<ConstraintType CType>
inline ArgumentConstraint<CType>& operator-(ArgumentConstraint<CType>& arg) {
    arg.set_required(false);
    return arg;
}

inline Argument& operator+(Argument& arg) {
    arg.set_required(true);
    return arg;
}

inline Argument&& operator+(Argument&& arg) {
    arg.set_required(true);
    return std::move(arg);
}

template<ConstraintType CType>
inline ArgumentConstraint<CType>& operator+(ArgumentConstraint<CType>& arg) {
    arg.set_required(true);
    return arg;
}

template<ConstraintType CType>
inline ArgumentConstraint<CType>&& operator+(ArgumentConstraint<CType>&& arg) {
    arg.set_required(true);
    return std::move(arg);
}

}


