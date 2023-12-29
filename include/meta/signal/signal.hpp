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

#ifndef META_SIGNAL_HPP
#define META_SIGNAL_HPP

#include <meta/meta_api.hpp>
#include <meta/arguments/argument_type.hpp>
#include <meta/metadata/callable.hpp>

#include <deque>

namespace meta
{

/// The connection type.
class Connection;
using ConnectionPtr = std::unique_ptr<Connection>;

/// BaseSignal is the base class of the meta signals.
class META_API BaseSignal
{
    DISABLE_COPY(BaseSignal);
    DISABLE_MOVE(BaseSignal);

public:
    /// Checks whether the signal is valid.
    /// \return If the signal is valid, returns \e true, otherwise \e false.
    bool isValid() const;

    /// Activates the signal with the packaged arguments.
    /// \param arguments The packaged arguments to pass as signal arguments.
    /// \return The number of activated connections. If the value is 0, there was no connection
    ///         activated. If the value is -1, the arguments do not match the signature of the signal.
    int emit(const PackagedArguments& arguments = PackagedArguments());

    /// Adds a connection to the signal.
    /// \param connection The connection to add to the signal.
    void connect(ConnectionPtr connection);

    /// Removes a connection from the signal.
    /// \param connection The connection to remove from the signal.
    void disconnect(Connection& connection);

    /// Returns the number of valid connections.
    /// \return The number of connections which are valid.
    std::size_t getConnectionCount() const;

protected:
    /// Constructor.
    explicit BaseSignal() = default;
    /// Destructor.
    virtual ~BaseSignal() = default;

    /// Activates the connections of the signal.
    /// \param arguments The packaged arguments to pass as signal arguments.
    /// \return The number of activated connections.
    int activateConnections(const PackagedArguments& arguments = PackagedArguments());

    /// Verifies the packaged arguments against the signature of the signal.
    /// \param arguments The packaged arguments to verify against the signature.
    /// \return If the packaged arguments match the signature of the signal, returns \e true, otherwise
    ///         \e false.
    virtual bool verifySignature(const PackagedArguments& arguments) const = 0;

    using ConnectionContainer = std::deque<ConnectionPtr>;
    ConnectionContainer m_connections;
    bool m_emitting = false;
};


template <class Signature>
class Signal;

template <class... Arguments>
class META_TEMPLATE_API Signal<void(Arguments...)> : public BaseSignal
{
public:
    using Signature = void(*)(Arguments...);

    explicit Signal(std::string_view /*name*/)
    {
    }

    int operator()(Arguments&&... arguments)
    {
        auto pack = PackagedArguments(std::forward<Arguments>(arguments)...);
        return activateConnections(pack);
    }

protected:
    bool verifySignature(const PackagedArguments& arguments) const override
    {
        constexpr auto arity = sizeof...(Arguments);
        if (arguments.getSize() < arity)
        {
            return false;
        }

        try
        {
            auto tupleArgs = arguments.toTuple<Signature>();
            return std::is_same_v<typename traits::function_traits<Signature>::arg_types, decltype(tupleArgs)>;
        }
        catch (...)
        {
            return false;
        }
    }
};

}

#endif
