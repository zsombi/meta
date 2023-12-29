/*
 * Copyright (C) 2023 bitWelder
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

#ifndef META_CONNECTION_HPP
#define META_CONNECTION_HPP

#include <meta/meta_api.hpp>
#include <utils/function_traits.hpp>
#include <meta/arguments/argument_type.hpp>
#include <meta/metadata/callable.hpp>

namespace meta
{

class BaseSignal;
class Connection;

/// The class represents a connection to a signal. The connection is a token which holds the
/// signal connected, and the function, method, metamethod, functor or lambda the signal is
/// connected to. This function is called slot.
class META_API Connection : protected Callable
{
    friend struct ConnectionPrivate;

public:
    /// Constructor. Creates a connection to an object and its method slot.
    /// \param object The object of the slot.
    /// \param function The function
    template <class Class, class Function>
    explicit Connection(Class& object, Function function) :
        Callable("object_slot", function),
        m_object(static_cast<typename traits::function_traits<Function>::object*>(&object))
    {
        static_assert(std::is_member_function_pointer_v<Function> &&
                      std::is_base_of_v<typename traits::function_traits<Function>::object, Class>,
                      "Object differs from the function");
        if constexpr (traits::function_traits<Function>::arity > 0u)
        {
            m_passConnection = traits::is_same_arg<Function, Connection*, 0u>::value;
        }
    }

    /// Constructor. Creates a connection to a function slot.
    /// \param function The function
    template <class Function>
    explicit Connection(Function function) :
        Callable("function_slot", function)
    {
        if constexpr (traits::function_traits<Function>::arity > 0u)
        {
            m_passConnection = traits::is_same_arg<Function, Connection*, 0u>::value;
        }
    }

    Connection(Connection&& other);
    Connection& operator=(Connection&& other);
    void swap(Connection& other);

    /// Returns the state of the connection.
    /// \return If the connection is connected, \e true, otherwise \e false.
    bool isConnected() const
    {
        return m_signal != nullptr;
    }

    /// The signal of the connection.
    BaseSignal* getSignal() const
    {
        return m_signal;
    }

    /// Disconnects the connection from the connected signal.
    /// \return If the connection
    bool disconnect();

    /// Activates the connection by calling the slot of the connection.
    /// \param args The arguments to pass to the slot.
    bool activate(const PackagedArguments& arguments);

private:
    ArgumentData m_object;
    /// The signal the connection is attached to.
    BaseSignal* m_signal = nullptr;
    /// If the slot's first argument is the connection object itself.
    bool m_passConnection = false;
};

}

#endif
