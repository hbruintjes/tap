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

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::One> operator^(basic_argument<char_t>& left, basic_argument<char_t>& right) {
    return basic_argument_constraint<char_t, constraint_type::One>(left, right);
}

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::One> operator^(basic_argument_constraint<char_t, constraint_type::One> left, basic_argument<char_t>& right) {
    left += right;
    return left;
}

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::One> operator^(basic_argument<char_t>& left, basic_argument_constraint<char_t, constraint_type::One>& right) {
    return right | left;
}

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::Any> operator|(basic_argument<char_t>& left, basic_argument<char_t>& right) {
    return basic_argument_constraint<char_t, constraint_type::Any>(left, right);
}

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::Any> operator|(basic_argument_constraint<char_t, constraint_type::Any> left, basic_argument<char_t>& right) {
    left += right;
    return left;
}

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::Any> operator|(basic_argument<char_t>& left, basic_argument_constraint<char_t, constraint_type::Any>& right) {
    return right | left;
}

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::Imp> operator>(basic_argument<char_t>& left, basic_argument<char_t>& right) {
    return basic_argument_constraint<char_t, constraint_type::Imp>(left, right);
}

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::Imp> operator>(basic_argument_constraint<char_t, constraint_type::Imp> left, basic_argument<char_t>& right) {
    left += right;
    return left;
}

template<typename char_t>
inline basic_argument_constraint<char_t, constraint_type::Imp> operator>(basic_argument<char_t>& left, basic_argument_constraint<char_t, constraint_type::Imp>& right) {
    return basic_argument_constraint<char_t, constraint_type::Imp>(left) += right;
}

template<typename char_t>
inline basic_argument<char_t>& operator-(basic_argument<char_t>& arg) {
    arg.set_required(false);
    return arg;
}

template<typename char_t, constraint_type CType>
inline basic_argument_constraint<char_t, CType>& operator-(basic_argument_constraint<char_t, CType>& arg) {
    arg.set_required(false);
    return arg;
}

template<typename char_t>
inline basic_argument<char_t>& operator+(basic_argument<char_t>& arg) {
    arg.set_required(true);
    return arg;
}

template<typename char_t>
inline basic_argument<char_t>&& operator+(basic_argument<char_t>&& arg) {
    arg.set_required(true);
    return std::move(arg);
}

template<typename char_t, constraint_type CType>
inline basic_argument_constraint<char_t, CType>& operator+(basic_argument_constraint<char_t, CType>& arg) {
    arg.set_required(true);
    return arg;
}

template<typename char_t, constraint_type CType>
inline basic_argument_constraint<char_t, CType>&& operator+(basic_argument_constraint<char_t, CType>&& arg) {
    arg.set_required(true);
    return std::move(arg);
}

}


