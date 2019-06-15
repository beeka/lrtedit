//  This file is part of LrtEdit.
//
// LrtEdit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LrtEdit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LrtEdit.  If not, see <https://www.gnu.org/licenses/>.
#ifndef LUATABLE_H
#define LUATABLE_H

#include <QException>
#include <QIODevice>
#include <QPair>
#include <QVariant>
#include <QVector>

namespace LuaParser
{
/// A Lua table is a mix of a sequence and an associative type.
/// There is scope for making this more uniform (associative array
/// mapping with an "int or string" index type)
class Table
{
   public:
    /// Indexing sequences is unity-indexed
    const QVariant &operator[](int index) const;

    /// Indexing sequences is unity-indexed
    QVariant &operator[](int index);

    const QVariant operator[](const QString &index) const;

    QVariant &operator[](const QString &index);

    void append(const QVariant &value);

    // Return the equivalent of the lua # operator and count the list elements
    int hash() const;

    QList<QString> keys() const;

    /// Path-based variant accessor.
    ///
    const QVariant getAttr(const QString &attr) const;
    void setAttr(const QString &attr, const QVariant &value);

    /// Path-based string accessor
    QString getString(const QString &attr) const;
    void setString(const QString &attr, const QString &value);

    int getInt(const QString &attr) const;

    double getDouble(const QString &attr) const;

    Table getTable(const QString &attr) const;

    /// Return the number of list elements for the Table specified by attr
    int getSequenceSize(const QString &attr) const;

   private:
    QVariantList list;
    QVariantMap dictionary;
};


};  // namespace LuaParser

Q_DECLARE_METATYPE(LuaParser::Table);

#endif // LUATABLE_H
