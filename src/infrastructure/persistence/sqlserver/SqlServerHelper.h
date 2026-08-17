#include <array>
#include <string>
#include <Windows.h>
#include <sqltypes.h>
#include <vector>
#include "../../../domain/entities/User.h"
#include "SqlTraits.h"

namespace infrastructure::persistence::sqlserver {

class SqlServerHelper {
public:
    template <typename T> static std::string buildInsertSql() {
        const auto& cols = SqlTraits<T>::columns;

        std::string sql = "INSERT INTO ";
        sql += SqlTraits<T>::table;
        sql += " (";

        for (size_t i = 0; i < cols.size(); ++i) {
            sql += cols[i];
            if (i + 1 < cols.size()) sql += ", ";
        }

        sql += ") VALUES (";

        for (size_t i = 0; i < cols.size(); ++i) {
            sql += "?";
            if (i + 1 < cols.size()) sql += ", ";
        }

        sql += ")";
        return sql;
    }

    template <typename T> static void bindParameters(SQLHSTMT stmt, const T& entity,
        std::vector<std::string>& buffers, std::vector<SQLLEN>& lengths) {
        const auto& sizes = SqlTraits<T>::columnSizes;

        buffers.clear();
        lengths.clear();

        buffers.reserve(sizes.size());
        lengths.reserve(sizes.size());

        for (size_t i = 0; i < sizes.size(); ++i) {
            buffers.push_back(SqlTraits<T>::getField(entity, i));
            lengths.push_back(
                static_cast<SQLLEN>(buffers[i].size())
            );

            auto ret = SQLBindParameter(
                stmt,
                static_cast<SQLUSMALLINT>(i + 1),
                SQL_PARAM_INPUT,
                SQL_C_CHAR,
                SQL_VARCHAR,
                static_cast<SQLULEN>(sizes[i]),
                0,
                const_cast<char*>(buffers[i].c_str()),
                static_cast<SQLLEN>(buffers[i].size()),   // buffer length
                &lengths[i]                                 // length pointer
            );

            if (!SQL_SUCCEEDED(ret)) {
                throw std::runtime_error("Failed to bind parameter for entity insertion");
            }

            // SQLSMALLINT sqlType;
            // SQLULEN colSize;
            // SQLSMALLINT decDigits;
            // SQLSMALLINT nullable;

            // SQLDescribeParam(stmt, i+1, &sqlType, &colSize, &decDigits, &nullable);

            // std::cout << "Param " << i+1
            //         << " expects SQL type " << sqlType
            //         << " size " << colSize << "\n";
        }
    }

    template <typename T> static std::string buildSelectByColumnSql(const std::string& column) {
        const auto& cols = SqlTraits<T>::columns;

        std::string sql = "SELECT ";

        for (size_t i = 0; i < cols.size(); ++i) {
            sql += cols[i];
            if (i + 1 < cols.size()) sql += ", ";
        }

        sql += " FROM ";
        sql += SqlTraits<T>::table;
        sql += " WHERE ";
        sql += column;
        sql += " = ?";

        return sql;
    }

    template <typename T> static void bindOutputColumns(SQLHSTMT stmt, std::vector<std::vector<char>>& buffers) {
    const auto& sizes = SqlTraits<T>::columnSizes;

    for (size_t i = 0; i < sizes.size(); ++i) {
        buffers[i].resize(sizes[i] + 1);

        SQLBindCol(
            stmt,
            static_cast<SQLUSMALLINT>(i + 1),
            SQL_C_CHAR,
            buffers[i].data(),
            sizes[i] + 1,
            nullptr
        );
    }
}

};

}
