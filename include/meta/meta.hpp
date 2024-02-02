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

#ifndef META_HPP
#define META_HPP

#include <pimpl.hpp>
#include <preprocessor.hpp>
#include <meta/meta_api.hpp>
#include <string_view>

namespace meta
{

class Tracer;
class ThreadPool;
struct LibraryArguments;
class MetaLibraryPrivate;
class ObjectFactory;

/// The class holds the library elements configured for a use case.
class META_API Library
{
public:
    /// Returns the instance of the meta library.
    static Library& instance();

    /// Initializes the meta library.
    /// \param arguments The library initialization arguments.
    void initialize(const LibraryArguments& arguments);

    /// Uninitialize the meta library.
    void uninitialize();

    /// Returns the task scheduler of the library.
    /// \return The task scheduler of the library, or nullptr, if the library is initialized without
    ///         a task scheduler.
    ThreadPool* taskScheduler() const;

    /// Returns the tracer of the library. The method is only available when tracing is eanbled.
    /// \return The tracer of the library.
    Tracer* tracer() const;

    /// Returns the object factory of the.
    /// \return The object factory of the.
    ObjectFactory* objectFactory() const;

private:
    explicit Library();
    ~Library();

    DECLARE_PRIVATE_PTR(MetaLibraryPrivate)
};

/// Checks whether the string passed as argument is a valid metaname. A metaname can only contain
/// numeric and alphanumeric characters, dots, columns, dashes and underscores.
/// \param text The string to check.
/// \return If the string is a valid metaname, returns \e true, otherwise \e false.
META_API bool isValidMetaName(std::string_view text);

} // namespace meta

#endif // META_HPP
