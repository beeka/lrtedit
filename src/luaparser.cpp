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
#include "luaparser.h"

#include <QBuffer>
#include <QDebug>
#include <QException>
#include <QFile>

namespace LuaParser
{
// A couple of functions are inter-related so need declarations
Table readTable(QIODevice *device);
NamedVariant readVariable(QIODevice *device);

// Implement the ParserError execption
ParseError::ParseError(const QString &info, long long where) : m_info(info), m_where(where) {}
const QString ParseError::info() const { return m_info; }
long long ParseError::where() const { return m_where; }


NamedVariant readLuaStruct(const QString &path)
{
    QFile f(path);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString s = f.readAll();
        qDebug() << "Read" << s.size() << "bytes from" << path;
        return parseLuaStruct(s);
    }
    else
    {
        qCritical() << "Error opening file for reading:" << path;
    }

    return NamedVariant();
}


// Peeks at the next character
char peekChar(QIODevice *device)
{
    Q_ASSERT(device != nullptr);
    Q_ASSERT(!device->atEnd());

    char c = 0;
    device->peek(&c, 1);

    return c;
}


// Gets the next character
char nextChar(QIODevice *device)
{
    Q_ASSERT(device != nullptr);
    Q_ASSERT(!device->atEnd());

    char c = 0;
    device->getChar(&c);

    return c;
}


void ignoreWhitespace(QIODevice *device)
{
    Q_ASSERT(device != nullptr);

    char c;
    while (!device->atEnd())
    {
        device->getChar(&c);
        if (!isspace(c))
        {
            device->ungetChar(c);
            return;
        }
    }
}


void expectLiteral(QIODevice *device, const QString &literal)
{
    Q_ASSERT(device != nullptr);

    QString token;

    char c;
    while (!device->atEnd())
    {
        device->getChar(&c);
        token.append(c);
        if (token == literal)
        {
            return;
        }
        else if (!literal.startsWith(token))
        {
            qCritical() << "Expected literal '" + literal + "' but found '" + token + "' at" << device->pos();
            throw ParseError("Expected literal " + literal, device->pos());
        }
    }

    qCritical() << "Expected literal '" + literal + "' but ran out of chars with '" + token + "' at" << device->pos();
    throw ParseError("Expected literal but ran out of input with" + token);
}


QString readIdentifier(QIODevice *device)
{
    Q_ASSERT(device != nullptr);

    QString identifier;

    char c;
    while (!device->atEnd())
    {
        device->getChar(&c);
        if (isalnum(c) || c == '_')
        {
            identifier.append(c);
        }
        else
        {
            device->ungetChar(c);
            break;
        }
    }

    if (identifier.isEmpty()) qWarning() << "Expected identifier but found nothing at" << device->pos();

    return identifier;
}


// Read a quoted string.
// NB: No escaping is currently implemented (e.g. '\"')
QString readString(QIODevice *device)
{
    Q_ASSERT(device != nullptr);

    expectLiteral(device, "\"");

    QString value;

    while (!device->atEnd() && peekChar(device) != '"')
    {
        value.append(nextChar(device));
    }

    expectLiteral(device, "\"");

    return value;
}


QVariant readNumber(QIODevice *device)
{
    Q_ASSERT(device != nullptr);

    QString number;

    char c;
    while (!device->atEnd())
    {
        c = peekChar(device);
        if (!isdigit(c) && c != '.' && c != '-') break;

        number.append(nextChar(device));
    }

    if (number.isEmpty()) qWarning() << "Expected number but found nothing at" << device->pos();

    bool okay;

    const int i = number.toInt(&okay);
    if (okay) return QVariant::fromValue(i);

    const double d = number.toDouble(&okay);
    if (okay) return QVariant::fromValue(d);

    return QVariant();
}


QVariant readBool(QIODevice *device)
{
    Q_ASSERT(device != nullptr);
    const QString s = readIdentifier(device);
    if (s == "true")
    {
        return QVariant::fromValue(true);
    }
    else if (s == "false")
    {
        return QVariant::fromValue(false);
    }
    else
    {
        qCritical() << "Expected boolean but found" << s;
        throw ParseError(s + " is not a boolean value", device->pos());
    }
}


// Read a typed value
QVariant readValue(QIODevice *device)
{
    QVariant value;
    ignoreWhitespace(device);
    const char c = peekChar(device);
    if (c == '{')
    {
        const Table table = readTable(device);
        value = QVariant::fromValue(table);
    }
    //    else if (isalnum(nextChar(device)))
    else if (isdigit(c) || c == '-')
    {
        // read a number
        value = readNumber(device);
    }
    else if (c == '"')
    {
        value = readString(device);
    }
    else if (c == 'Z')  // Assume this is a ZSTR (whatever one of those is)
    {
        expectLiteral(device, "ZSTR");
        ignoreWhitespace(device);
        value = "ZSTR:" + readString(device);
    }
    else if (c == 't' || c == 'f')  // Assume this is a bool?
    {
        value = readBool(device);
    }
    else
    {
        qCritical() << "Unsupported variable type at" << device->pos();
        throw ParseError("Unsupported variable type", device->pos());
    }

    return value;
}

/// Read a lua table from a device.
/// Table syntax described in https://www.lua.org/pil/3.6.html
Table readTable(QIODevice *device)
{
    Q_ASSERT(device != nullptr);

    Table table;

    ignoreWhitespace(device);

    expectLiteral(device, "{");

    ignoreWhitespace(device);

    // Will either be an identifier or the end of the table
    while (peekChar(device) != '}')
    {
        const char c = peekChar(device);

        if (isalnum(c))
        {
            // Either found bool (true/false) or an identifier.
            // Not needing to support list of bools here, so just assume a variable
            const NamedVariant nv = readVariable(device);
            table[nv.name()] = nv.value();
        }
        else
        {
            // Assume an unamed list item
            const QVariant item = readValue(device);
            table.append(item);
        }

        ignoreWhitespace(device);
        if (peekChar(device) != ',')
        {
            // No trailing comma, so expect the end of the list to be found
            break;
        }
        expectLiteral(device, ",");
        // NB: A comma is not strictly required at the end of list, but usually there on the Adobe files
        // NB: A semi-colon is also fine here, according to the Lua spec (instead of a comma)

        ignoreWhitespace(device);
    }

    ignoreWhitespace(device);
    expectLiteral(device, "}");

    return table;
}

NamedVariant readVariable(QIODevice *device)
{
    Q_ASSERT(device != nullptr);

    ignoreWhitespace(device);

    const QString identifier = readIdentifier(device);

    ignoreWhitespace(device);
    expectLiteral(device, "=");

    const QVariant value = readValue(device);

    // qDebug() << "Read variable:" << identifier << "=" << value;
    return NamedVariant(identifier, value);
}


/// Remove whole line comments
QString uncommented(const QString &s)
{
    QStringList lines = s.split('\n');
    int l = 0;
    while (l < lines.size())
    {
        if (lines[l].trimmed().startsWith("--"))
            lines.removeAt(l);
        else
            l++;
    }
    return lines.join('\n');
}


NamedVariant parseLuaStruct(const QString &s)
{
    NamedVariant nv;

    QByteArray ba(uncommented(s).toUtf8());
    QBuffer buffer(&ba);
    buffer.open(QIODevice::ReadOnly);
    try
    {
        nv = readVariable(&buffer);
    }
    catch (const ParseError &e)
    {
        qCritical() << "There was a problem parsing" << e.info() << "at" << e.where() << ":" << s.mid(e.where(), 5);
        //        throw e;
    }

    return nv;
}

}  // end of namespace LuaParser
