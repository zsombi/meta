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

#include <gtest/gtest.h>
#include "utils/domain_test_environment.hpp"

#include <meta/meta.hpp>
#include <meta/signal/signal.hpp>
#include <meta/signal/connection.hpp>

namespace
{

class Base
{
public:
    virtual ~Base() = default;

    void baseSlot()
    {
        META_LOG_INFO("Base::" << __FUNCTION__);
    }
    virtual void overridable()
    {
        META_LOG_INFO("Base::" << __FUNCTION__);
    }
};

class SignalTests : public DomainTestEnvironment, public Base
{
protected:
    void SetUp() override
    {
        // Single threaded
        initializeDomain(false, true);
    }

public:
    void voidSlot()
    {
        META_LOG_INFO(__FUNCTION__);
    }
    void selfDisconnect(meta::Connection* connection)
    {
        META_LOG_INFO(__FUNCTION__);
        connection->disconnect();
    }
    void overridable() override
    {
        Base::overridable();
        META_LOG_INFO("SignalTests::" << __FUNCTION__);
    }
};

}

TEST_F(SignalTests, emitWithNoConnection)
{
    meta::Signal<void()> voidSignal{"voidSignal"};
    EXPECT_EQ(0u, voidSignal.emit());
}

TEST_F(SignalTests, emitWithConnectionToLambda)
{
    meta::Signal<void()> voidSignal{"voidSignal"};
    auto lambda = []()
    {
        META_LOG_INFO("lambda");
    };
    auto connection = std::make_unique<meta::Connection>(lambda);
    voidSignal.connect(std::move(connection));
    EXPECT_CALL(*m_mockPrinter, log("lambda"));
    EXPECT_EQ(1u, voidSignal.emit());
}

TEST_F(SignalTests, emitSignalWithArnumentsWithConnectionToLambdas)
{
    meta::Signal<void(int)> voidSignal{"voidSignal"};
    auto lambda1 = []()
    {
        META_LOG_INFO("lambda1");
    };
    auto connection1 = std::make_unique<meta::Connection>(lambda1);
    voidSignal.connect(std::move(connection1));
    auto lambda2 = [](int value)
    {
        META_LOG_INFO("lambda2 " << value);
    };
    auto connection2 = std::make_unique<meta::Connection>(lambda2);
    voidSignal.connect(std::move(connection2));

    EXPECT_CALL(*m_mockPrinter, log("lambda1"));
    EXPECT_CALL(*m_mockPrinter, log("lambda2 101"));
    EXPECT_EQ(2u, voidSignal.emit(meta::PackagedArguments(101)));
}

TEST_F(SignalTests, emitWithConnectionToLambdaWithConnectionAsArgument)
{
    meta::Signal<void()> voidSignal{"voidSignal"};
    auto lambda = [](meta::Connection* connection)
    {
        META_LOG_INFO("lambda");
        connection->disconnect();
    };
    auto connection = std::make_unique<meta::Connection>(lambda);
    voidSignal.connect(std::move(connection));
    EXPECT_CALL(*m_mockPrinter, log("lambda"));
    EXPECT_EQ(1u, voidSignal.getConnectionCount());
    EXPECT_EQ(1u, voidSignal.emit());
    EXPECT_EQ(0u, voidSignal.getConnectionCount());
}

TEST_F(SignalTests, emitNoArgSignalWithArguments)
{
    meta::Signal<void()> voidSignal{"voidSignal"};
    EXPECT_EQ(0, voidSignal.emit(meta::PackagedArguments(10)));
}

TEST_F(SignalTests, emitArgSignalWithDifferentArgument)
{
    meta::Signal<void(std::string)> signal{"stringSignal"};
    auto slot = [](meta::Connection* connection)
    {
        META_LOG_INFO("lambda");
        connection->disconnect();
    };
    auto connection = std::make_unique<meta::Connection>(slot);
    signal.connect(std::move(connection));

    EXPECT_EQ(-1, signal.emit(meta::PackagedArguments(10)));
}

TEST_F(SignalTests, emitArgSignalWithMoreArguments)
{
    meta::Signal<void(std::string)> signal{"stringSignal"};
    auto slot = [](meta::Connection* connection)
    {
        META_LOG_INFO("lambda");
        connection->disconnect();
    };
    auto connection = std::make_unique<meta::Connection>(slot);
    signal.connect(std::move(connection));
    EXPECT_CALL(*m_mockPrinter, log("lambda"));
    EXPECT_EQ(1, signal.emit(meta::PackagedArguments(std::string("first"), 10)));
}

TEST_F(SignalTests, emitArgSignalWithOperator)
{
    meta::Signal<void(std::string_view)> signal{"stringSignal"};
    auto slot = [](meta::Connection* connection, std::string_view arg)
    {
        META_LOG_INFO("lambda: " << arg);
        connection->disconnect();
    };
    auto connection = std::make_unique<meta::Connection>(slot);
    signal.connect(std::move(connection));
    EXPECT_CALL(*m_mockPrinter, log("lambda: first"));
    EXPECT_EQ(1, signal("first"));
}

TEST_F(SignalTests, connectToMethodSlot)
{
    meta::Signal<void()> signal("voidSignal");
    auto connection = std::make_unique<meta::Connection>(*this, &SignalTests::voidSlot);
    signal.connect(std::move(connection));
    EXPECT_CALL(*m_mockPrinter, log("voidSlot"));
    EXPECT_EQ(1, signal());
}

TEST_F(SignalTests, connectToBaseSlot)
{
    meta::Signal<void()> signal("voidSignal");
    auto connection = std::make_unique<meta::Connection>(*this, &SignalTests::baseSlot);
    signal.connect(std::move(connection));
    EXPECT_CALL(*m_mockPrinter, log("Base::baseSlot"));
    EXPECT_EQ(1, signal());
}

TEST_F(SignalTests, connectToOverridable)
{
    meta::Signal<void()> signal("voidSignal");
    auto connection = std::make_unique<meta::Connection>(*this, &SignalTests::overridable);
    signal.connect(std::move(connection));
    EXPECT_CALL(*m_mockPrinter, log("Base::overridable"));
    EXPECT_CALL(*m_mockPrinter, log("SignalTests::overridable"));
    EXPECT_EQ(1, signal());
}

TEST_F(SignalTests, connectToSelfDisconnectable)
{
    meta::Signal<void()> signal("voidSignal");
    auto connection = std::make_unique<meta::Connection>(*this, &SignalTests::selfDisconnect);
    signal.connect(std::move(connection));
    EXPECT_CALL(*m_mockPrinter, log("selfDisconnect"));
    EXPECT_EQ(1, signal.getConnectionCount());
    EXPECT_EQ(1, signal());
    EXPECT_EQ(0, signal.getConnectionCount());
}

TEST_F(SignalTests, signalToSignal)
{
    meta::Signal<void(std::string_view)> signal{"stringSignal"};
    meta::Signal<void()> slotSignal("slotSignal");
    auto slot = [](meta::Connection* connection)
    {
        META_LOG_INFO("slot");
        connection->disconnect();
    };
    auto connection = std::make_unique<meta::Connection>(slot);
    slotSignal.connect(std::move(connection));
    auto signalConnection = std::make_unique<meta::Connection>(slotSignal, &meta::Signal<void()>::operator());
    signal.connect(std::move(signalConnection));
    EXPECT_CALL(*m_mockPrinter, log("slot"));
    EXPECT_EQ(1, signal("first"));
}
