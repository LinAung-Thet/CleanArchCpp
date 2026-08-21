#pragma once

#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace infrastructure::persistence
{

class IServerHelper
{
public:

    virtual ~IServerHelper() = default;

    // ========================================================
    // Public type-safe API
    // ========================================================

    template<typename T>
    std::string buildInsertSql(
        const T& entity) const
    {
        return buildInsertSqlImpl(
            typeid(T),
            &entity);
    }

    template<typename T>
    void bindParameters(
        SQLHSTMT stmt,
        const T& entity,
        std::vector<std::string>& buffers,
        std::vector<SQLLEN>& lengths) const
    {
        bindParametersImpl(
            typeid(T),
            stmt,
            &entity,
            buffers,
            lengths);
    }

    template<typename T>
    std::string buildSelectByColumnSql(
        const std::string& column) const
    {
        return buildSelectByColumnSqlImpl(
            typeid(T),
            column);
    }

    template<typename T>
    void bindOutputColumns(
        SQLHSTMT stmt,
        std::vector<std::vector<char>>& buffers) const
    {
        bindOutputColumnsImpl(
            typeid(T),
            stmt,
            buffers);
    }

    template<typename T>
    std::size_t columnCount() const
    {
        return columnCountImpl(
            typeid(T));
    }

protected:

    // ========================================================
    // Registry
    // ========================================================

    using InsertFn =
        std::function<std::string(
            const void*)>;

    using BindParametersFn =
        std::function<void(
            SQLHSTMT,
            const void*,
            std::vector<std::string>&,
            std::vector<SQLLEN>&)>;

    using SelectFn =
        std::function<std::string(
            const std::string&)>;

    using BindOutputFn =
        std::function<void(
            SQLHSTMT,
            std::vector<std::vector<char>>&)>;

    using ColumnCountFn =
        std::function<std::size_t()>;

    struct RegistryEntry
    {
        InsertFn insertFn;
        BindParametersFn bindParametersFn;
        SelectFn selectFn;
        BindOutputFn bindOutputFn;
        ColumnCountFn columnCountFn;
    };

    std::unordered_map<
        std::size_t,
        RegistryEntry> registry_;

    // ========================================================
    // Polymorphic operations
    // ========================================================

    virtual std::string buildInsertSqlImpl(
        const std::type_info& type,
        const void* entity) const = 0;

    virtual void bindParametersImpl(
        const std::type_info& type,
        SQLHSTMT stmt,
        const void* entity,
        std::vector<std::string>& buffers,
        std::vector<SQLLEN>& lengths) const = 0;

    virtual std::string buildSelectByColumnSqlImpl(
        const std::type_info& type,
        const std::string& column) const = 0;

    virtual void bindOutputColumnsImpl(
        const std::type_info& type,
        SQLHSTMT stmt,
        std::vector<std::vector<char>>& buffers) const = 0;

    virtual std::size_t columnCountImpl(
        const std::type_info& type) const = 0;
};

}