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
#include "signal_private.h"
#include <assert.hpp>

namespace meta
{

void ConnectionPrivate::attachToSignal(Connection& self, BaseSignal& signal)
{
    abortIfFail(!self.m_signal);

    self.m_signal = &signal;
}

void ConnectionPrivate::detachFromSignal(Connection& self)
{
    abortIfFail(self.m_signal);
    self.m_signal = nullptr;
}

Connection::Connection(Connection&& other) :
    Callable(std::forward<Connection>(other)),
    m_object(std::move(other.m_object)),
    m_signal(std::move(other.m_signal)),
    m_passConnection(std::move(other.m_passConnection))
{
}
Connection& Connection::operator=(Connection&& other)
{
    Connection tmp(std::forward<Connection>(other));
    swap(tmp);
    return *this;
}

void Connection::swap(Connection& other)
{
    Callable::swap(other);
    m_object.swap(other.m_object);
    std::swap(m_signal, other.m_signal);
    std::swap(m_passConnection, other.m_passConnection);
}

bool Connection::disconnect()
{
    if (!m_signal)
    {
        return false;
    }
    m_signal->disconnect(*this);
    return true;
}

bool Connection::activate(const PackagedArguments& arguments)
{
    auto repacked = PackagedArguments();

    if (m_object.has_value())
    {
        repacked += m_object;
    }
    if (m_passConnection)
    {
        repacked += this;
    }
    repacked.append(arguments);

    try
    {
        Callable::apply(repacked);
        return true;
    }
    catch (std::bad_any_cast&)
    {
        return false;
    }
}

}
