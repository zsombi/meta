/*
 * Copyright (C) 2017-2019 bitWelder
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
#include <gmock/gmock.h>

#include <meta/meta.hpp>
#include <meta/library_config.hpp>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

//    meta::Domain::instance().initialize(meta::LibraryArguments());

    return RUN_ALL_TESTS();
}
