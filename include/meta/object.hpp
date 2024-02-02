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

#ifndef META_METAOBJECT_HPP
#define META_METAOBJECT_HPP

#include <meta/forwards.hpp>
#include <meta/meta_api.hpp>
#include <meta/arguments/argument_type.hpp>
#include <meta/metadata/meta_object.hpp>
#include <pimpl.hpp>

#include <memory>
#include <optional>
#include <unordered_map>

namespace meta
{

struct ObjectDescriptor;

/// The base class of any object that defines a meta class.
class META_API Object : public MetaObject, public std::enable_shared_from_this<Object>
{
public:
    /// Creates a meta-object.
    static ObjectPtr create(std::string_view name, const PackagedArguments& = PackagedArguments());

    /// Destructor.
    virtual ~Object();

    /// The metadata of a meta object.
    META_CLASS("meta.Object", Object, MetaObject)
    {
        DYNAMIC_META_CLASS();
    };

    /// Adds an extension to the object. The object takes ownership over the extension. The method
    /// fails if the extension is already added to the object.
    /// \param extension The extension to add to the object.
    /// \return If the extension gets added with success, returns \e true, otherwise \e false.
    bool addExtension(ObjectExtensionPtr extension);

    /// Removes an extension from the object. The extension gets destroyed if the object owns the
    /// extension. The method fails if the extension does not extend the object.
    /// \return If the extension gets removed with success, returns \e true, otherwise \e false.
    bool removeExtension(ObjectExtension& invokable);

    /// Tries to locate the extension with the name.
    /// \param name The extension name to locate.
    /// \return The extension with the name, or \e nullptr if the object has no extension with the
    ///         name registered.
    ObjectExtensionPtr findExtension(std::string_view name) const;

    /// Invokes an extension of the object.
    /// \param name The name of the object extension to invoke.
    /// \param args Optional, the arguments with which to invoke the extension.
    /// \return returns one of the following:
    ///         - If the extension is found, and has a return value, the return value of the extension.
    ///         - If the extension is found, and has no return value, returns an invalid ArgumentData.
    ///         - If the extension is not found, returns nullopt.
    std::optional<ArgumentData> invoke(std::string_view name, const PackagedArguments& args = PackagedArguments());

protected:
    /// Constructor.
    explicit Object(std::string_view name);

private:
    using ExtensionsMap = std::unordered_map<std::string_view, ObjectExtensionPtr>;

    ExtensionsMap m_extensions;
    std::atomic_bool m_sealed = false;
    friend struct ObjectDescriptor;
};

/// Invokes an extension of a object.
/// \param object The object whose extension to invoke.
/// \param name The extension name to invoke.
/// \param arguments The arguments with which to invoke the extension.
/// \return returns one of the following:
///         - If the extension is found, and has a return value, the return value of the extension.
///         - If the extension is found, and has no return value, returns an invalid ArgumentData.
///         - If the extension is not found, returns nullopt.
META_API std::optional<ArgumentData> invoke(ObjectPtr object, std::string_view name, const PackagedArguments& arguments = PackagedArguments());

}

#endif // META_METAOBJECT_HPP
