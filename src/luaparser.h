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
#ifndef LUAPARSER_H
#define LUAPARSER_H

#include "luatable.h"

#include <QException>
#include <QIODevice>
#include <QPair>
#include <QVariant>
#include <QVector>

namespace LuaParser
{
class ParseError : public QException
{
    QString m_info;
    long long m_where;

   public:
    ParseError(const QString &info = QString(), long long where = 0);

    const QString info() const;
    long long where() const;
};


class NamedVariant : public QPair<QString, QVariant>
{
   public:
    NamedVariant() : QPair<QString, QVariant>(QString(), QVariant()) {}
    NamedVariant(const QString &n, const QVariant &v) : QPair<QString, QVariant>(n, v) {}
    const QString &name() const { return this->first; }
    const QVariant &value() const { return this->second; }
    void setValue(const QVariant &value) { this->second = value; }
};


/// Utility function to read a structure from a file
NamedVariant readLuaStruct(const QString &path);

/// Parses a lua-style structure, as used for storing data in Adobe Lightroom templates
NamedVariant parseLuaStruct(const QString &s);

};  // namespace LuaParser

#endif // LUAPARSER_H
