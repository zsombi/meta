/*
 * Copyright (C) 2024 bitWelder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/>
 */

#ifndef STEW_VARIABLE_HPP
#define STEW_VARIABLE_HPP

#include <stew/stew_api.hpp>
#include <stew/variable/type_info.hpp>

#include <any>

namespace stew
{

namespace
{
template <typename T>
struct is_any : std::false_type {};

template <>
struct is_any<std::any> : std::true_type {};

template <typename T>
concept not_any = !is_any<T>::value;
}

/// A dynamic type. It can store only copy-constructible types.
class STEW_API Variable
{
    std::any m_data;

public:
    /// Default constructor.
    Variable() = default;

    /// Constructor. Creates a variable from an any.
    Variable(std::any data) :
        m_data(data)
    {
    }
    /// Constructor. Creates a variable from a value that is not an any.
    template <typename T>
        requires not_any<T>
    Variable(T value) :
        m_data(value)
    {
    }

    /// Assignment operator.
    Variable& operator=(const std::any& data)
    {
        m_data = data;
        return *this;
    }
    /// Assignment operator.
    Variable& operator=(std::any&& data)
    {
        m_data = std::forward<std::any>(data);
        return *this;
    }
    /// Assignment operator.
    template <typename T>
        requires not_any<T>
    Variable& operator=(T&& data)
    {
        m_data = std::forward<T>(data);
        return *this;
    }

    /// Returns the current type of the variable.
    TypeInfo getType() const;

    /// Returns if this variable is of RTTI type.
    bool isTypeOf(const std::type_info& type) const;

    /// Returns if this variable is of type T.
    template <typename T>
    bool isTypeOf()
    {
        return isTypeOf(typeid(T));
    }

    /// Cast operator, returns the data stored by the Variable.
    /// \tparam T The type of the casted value.
    /// \return The value stored.
    /// \throws Throws std::bad_any_cast if the type to cast to is not the type the data is stored.
    template <class T>
    operator T() const;
};

//------------------Implementation------------------
template <class T>
Variable::operator T() const
{
    if (typeid(T) == m_data.type())
    {
        return std::any_cast<T>(m_data);
    }
    // TODO: convert.
}

}

#endif // STEW_VARIABLE_HPP