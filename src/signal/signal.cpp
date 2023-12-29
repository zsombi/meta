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

#include <meta/signal/connection.hpp>
#include <meta/signal/signal.hpp>
#include <utils/scope_value.hpp>
#include "signal_private.h"
#include <assert.hpp>

namespace meta
{


bool BaseSignal::isValid() const
{
    return true;
}

int BaseSignal::activateConnections(const PackagedArguments& arguments)
{
    auto result = 0;

    {
        utils::ScopeValue<bool> emitGuard(m_emitting, true);
        auto last = m_connections.end();
        for (auto it = m_connections.begin(); it != last; ++it)
        {
            if ((*it)->getSignal() != this)
            {
                continue;
            }

            if ((*it)->activate(arguments))
            {
                ++result;
            }
        }
    }

    // Compact disconnected connections.
    std::erase(m_connections, ConnectionPtr());

    return result;
}

int BaseSignal::emit(const PackagedArguments& arguments)
{
    if (!verifySignature(arguments))
    {
        return -1;
    }

    return activateConnections(arguments);
}

void BaseSignal::connect(ConnectionPtr connection)
{
    abortIfFail(connection && !connection->isConnected());
    ConnectionPrivate::attachToSignal(*connection, *this);
    m_connections.push_back(std::move(connection));
}

void BaseSignal::disconnect(Connection& connection)
{
    abortIfFail(connection.getSignal() == this);

    ConnectionPrivate::detachFromSignal(connection);

    if (m_emitting)
    {
        auto predicate = [&connection](auto& item)
        {
            return &connection == item.get();
        };
        auto it = std::find_if(m_connections.begin(), m_connections.end(), predicate);
        it->reset();
    }
    else
    {
        // Compact disconnected connections.
        std::erase(m_connections, ConnectionPtr());
    }
}

std::size_t BaseSignal::getConnectionCount() const
{
    if (m_emitting)
    {
        auto result = std::size_t(0u);
        for (auto& connection : m_connections)
        {
            if (connection->isConnected())
            {
                ++result;
            }
        }
        return result;
    }

    return m_connections.size();
}

}
