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
#include "luagenerator.h"

#include <QDebug>

LuaGenerator::LuaGenerator()
{
    
}

static const int indentWidth = 4;

QString LuaGenerator::Generate(const QVariant &value, int indent)
{
    QString s;
    //= QString(indent, '\t');

    if (value.type() == static_cast<QVariant::Type>(QMetaType::QString))
    {
        const QString str = value.value<QString>();
        if (str.startsWith("ZSTR:"))
        {
            s += " ZSTR \"" + str.mid(5) + '"';
        }
        else
        {
            s += '"' + str + '"';
        }
    }
    else if (value.type() == static_cast<QVariant::Type>(QMetaType::Int))
    {
        const int i = value.value<int>();
        s += QString::number(i);
    }
    else if (value.type() == static_cast<QVariant::Type>(QMetaType::Double))
    {
        const double d = value.value<double>();
        // A double has 15 digits of precision, so allow all of them to be printed
        s += QString::number(d, 'g', 15);
    }
    else if (value.type() == static_cast<QVariant::Type>(QMetaType::Bool))
    {
        const bool b = value.value<bool>();
        if (b)
        {
            s += "true";
        }
        else
        {
            s += "false";
        }
    }
    else if (value.canConvert<LuaParser::Table>())
    {
        const LuaParser::Table t = value.value<LuaParser::Table>();

        s += "{\n";

        // Print all named items
        const auto keys = t.keys();
        for (auto k : keys)
        {
            if (k.startsWith("hardcover_image"))
            {
                qDebug() << t[k];
            }
            s += QString(indent, ' ') + k + " = " + Generate(t[k], indent + indentWidth);
            s += ",\n";
        }

        // Print all list items
        for (int i = 1; i <= t.hash(); i++)
        {
            s += QString(indent, ' ') + Generate(t[i], indent + indentWidth);
            s += ",\n";
        }

        s += QString(indent - indentWidth, ' ') + "}";
        // s += "}";
    }
    else
    {
        s += "<unknown>";
    }

    return s;
}

QString LuaGenerator::Generate(const LuaParser::NamedVariant &nv, int indent)
{
    QString s;

    if (!nv.first.isEmpty()) s += nv.first + " = ";

    s += Generate(nv.second, indent + indentWidth);

    return s;
}

QString LuaGenerator::Generate(const LuaParser::Table &table, int indent)
{
    return Generate(QVariant::fromValue(table), indent);
}
