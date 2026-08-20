#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

class IServerHelper
{
public:

    using InsertFn =
        std::function<std::string(
            const void* object)>;

    using BindFn =
        std::function<void(
            SQLHSTMT stmt,
            const void* object,
            std::vector<std::string>& buffers,
            std::vector<SQLLEN>& lengths)>;

    using SelectFn =
        std::function<std::string(
            const std::string& column)>;

    using OutputFn =
        std::function<void(
            SQLHSTMT stmt,
            std::vector<std::vector<char>>& buffers)>;

    using ColumnCountFn = 
        std::function<std::size_t()>;   

    struct RegistryEntry
    {
        InsertFn insertFn;
        BindFn bindFn;
        SelectFn selectFn;
        OutputFn outputFn;
        ColumnCountFn columnCountFn;
    };

    virtual ~IServerHelper() = default;

    // ========================================================
    // Registration
    // ========================================================

    void registerType(
        const std::type_info& type,
        InsertFn insertFn,
        BindFn bindFn,
        SelectFn selectFn,
        OutputFn outputFn,
        ColumnCountFn columnCountFn)
    {
        registry_[type.hash_code()] =
        {
            std::move(insertFn),
            std::move(bindFn),
            std::move(selectFn),
            std::move(outputFn),
            std::move(columnCountFn)
        };
    }

    // ========================================================
    // SELECT
    // ========================================================

    template<typename T>
    std::string buildSelectByColumnSql(
        const std::string& column) const
    {
        const auto& entry = getEntry<T>();

        return entry.selectFn(column);
    }

    // ========================================================
    // INSERT
    // ========================================================

    template<typename T>
    std::string buildInsertSql(
        const T& entity) const
    {
        const auto& entry = getEntry<T>();

        return entry.insertFn(
            static_cast<const void*>(&entity));
    }

    // ========================================================
    // Bind input parameters
    // ========================================================

    template<typename T>
    void bindParameters(
        SQLHSTMT stmt,
        const T& entity,
        std::vector<std::string>& buffers,
        std::vector<SQLLEN>& lengths) const
    {
        const auto& entry = getEntry<T>();

        entry.bindFn(
            stmt,
            static_cast<const void*>(&entity),
            buffers,
            lengths);
    }

    // ========================================================
    // Bind output columns
    // ========================================================

    template<typename T>
    void bindOutputColumns(
        SQLHSTMT stmt,
        std::vector<std::vector<char>>& buffers) const
    {
        const auto& entry = getEntry<T>();

        entry.outputFn(
            stmt,
            buffers);
    }

    // ========================================================
    // Column count
    // ========================================================

    template<typename T>
    std::size_t columnCount() const
    {
        return columnCountImpl(typeid(T));
    }

protected:

    virtual std::size_t columnCountImpl(
        const std::type_info& type) const = 0;

    template<typename T>
    const RegistryEntry& getEntry() const
    {
        const auto it =
            registry_.find(typeid(T).hash_code());

        if (it == registry_.end())
        {
            throw std::runtime_error(
                "Type is not registered");
        }

        return it->second;
    }

    std::unordered_map<
        std::size_t,
        RegistryEntry> registry_;
};