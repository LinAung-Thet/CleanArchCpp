#pragma once

#include <libpq-fe.h>

namespace infrastructure::persistence::PostgreDB
{

class PostgreHelper final
    : public ISqlHelper
{
public:

    explicit PostgreHelper(
        PGconn* connection)
        : connection_(connection)
    {
    }

    std::string buildInsertSql(
        const SqlMetadata& metadata) const override
    {
        std::string sql =
            "INSERT INTO " +
            metadata.tableName +
            " (";

        for (std::size_t i = 0;
             i < metadata.columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }

            sql += metadata.columns[i];
        }

        sql += ") VALUES (";

        for (std::size_t i = 0;
             i < metadata.columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }

            sql += "$";
            sql += std::to_string(i + 1);
        }

        sql += ")";

        return sql;
    }

    std::string buildSelectSql(
        const SqlMetadata& metadata,
        const std::string& whereClause = "")
        const override
    {
        std::string sql = "SELECT ";

        for (std::size_t i = 0;
             i < metadata.columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }

            sql += metadata.columns[i];
        }

        sql +=
            " FROM " +
            metadata.tableName;

        if (!whereClause.empty())
        {
            sql += " WHERE ";
            sql += whereClause;
        }

        return sql;
    }

    std::string buildUpdateSql(
        const SqlMetadata& metadata,
        const std::string& whereClause)
        const override
    {
        std::string sql =
            "UPDATE " +
            metadata.tableName +
            " SET ";

        for (std::size_t i = 0;
             i < metadata.columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }

            sql += metadata.columns[i];
            sql += " = $";
            sql += std::to_string(i + 1);
        }

        sql += " WHERE ";
        sql += whereClause;

        return sql;
    }

    std::string buildDeleteSql(
        const SqlMetadata& metadata,
        const std::string& whereClause)
        const override
    {
        return
            "DELETE FROM " +
            metadata.tableName +
            " WHERE " +
            whereClause;
    }

    void executeNonQuery(
        const std::string& sql,
        const SqlParameters& parameters)
        override
    {
        auto result =
            execute(sql, parameters);

        checkResult(
            result.get(),
            PGRES_COMMAND_OK);
    }

    SqlRows executeQuery(
        const std::string& sql,
        const SqlParameters& parameters)
        override
    {
        auto result =
            execute(sql, parameters);

        checkResult(
            result.get(),
            PGRES_TUPLES_OK);

        const int rowCount =
            PQntuples(result.get());

        const int columnCount =
            PQnfields(result.get());

        SqlRows rows;

        rows.reserve(
            static_cast<std::size_t>(
                rowCount));

        for (int rowIndex = 0;
             rowIndex < rowCount;
             ++rowIndex)
        {
            SqlRow row;

            row.reserve(
                static_cast<std::size_t>(
                    columnCount));

            for (int columnIndex = 0;
                 columnIndex < columnCount;
                 ++columnIndex)
            {
                if (PQgetisnull(
                        result.get(),
                        rowIndex,
                        columnIndex))
                {
                    row.emplace_back();
                }
                else
                {
                    row.emplace_back(
                        PQgetvalue(
                            result.get(),
                            rowIndex,
                            columnIndex));
                }
            }

            rows.push_back(
                std::move(row));
        }

        return rows;
    }

private:

    struct ResultDeleter
    {
        void operator()(PGresult* result) const
        {
            if (result)
            {
                PQclear(result);
            }
        }
    };

    using ResultPtr =
        std::unique_ptr<
            PGresult,
            ResultDeleter>;

    PGconn* connection_;

    ResultPtr execute(
        const std::string& sql,
        const SqlParameters& parameters)
    {
        std::vector<const char*> values;

        values.reserve(
            parameters.size());

        for (const auto& parameter :
             parameters)
        {
            if (parameter.isNull)
            {
                values.push_back(nullptr);
            }
            else
            {
                values.push_back(
                    parameter.value.c_str());
            }
        }

        PGresult* result =
            PQexecParams(
                connection_,
                sql.c_str(),
                static_cast<int>(
                    parameters.size()),
                nullptr,
                values.data(),
                nullptr,
                nullptr,
                0);

        if (!result)
        {
            throw std::runtime_error(
                PQerrorMessage(connection_));
        }

        return ResultPtr(result);
    }

    static void checkResult(
        PGresult* result,
        ExecStatusType expected)
    {
        if (PQresultStatus(result) != expected)
        {
            throw std::runtime_error(
                PQresultErrorMessage(result));
        }
    }
};

}