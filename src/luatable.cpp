#include "luatable.h"
#include <QDebug>

namespace LuaParser
{
const QVariant &LuaParser::Table::operator[](int index) const
{
    if (index > 0 && index <= list.size())
    {
        return list.at(index - 1);
    }
    else
    {
        qCritical() << "Index out of range:" << index << ", valid const range is 1 to" << this->list.size();
        throw QException();
    }
}

QVariant &Table::operator[](int index)
{
    if (index > 0 && index <= list.size() + 1)  // Allow one to be added!
    {
        return list[index - 1];
    }
    else
    {
        qCritical() << "Index out of range:" << index << ", valid non-const range is 1 to" << this->list.size() + 1;
        throw QException();
    }
}

const QVariant Table::operator[](const QString &index) const
{
    if (dictionary.contains(index))
    {
        return dictionary[index];
    }
    else
    {
        qCritical() << "Invalid index '" + index + "' in const string index lookup.";
        qDebug() << "Valid keys are:" << dictionary.keys().join(',');
        throw QException();
    }
}

QVariant &Table::operator[](const QString &index)
{
    return dictionary[index];
}

void Table::append(const QVariant &value)
{
    list.append(value);
}

int Table::hash() const
{
    return list.size();
}

QList<QString> Table::keys() const
{
    return dictionary.keys();
}


const QVariant Table::getAttr(const QString &attr) const
{
    int pos = attr.indexOf('/');
    if (pos > 0)
    {
        const QString a = attr.left(pos);
        const QString b = attr.mid(pos + 1);

        // More of the path to go
        if (a[0].isDigit())
            return (*this)[a.toInt()].value<Table>().getAttr(b);
        else
            return (*this)[a].value<Table>().getAttr(b);
    }
    else
    {
        // Just been asked for the leaf
        if (attr[0].isDigit())
            return (*this)[attr.toInt()];
        else
            return (*this)[attr];
    }
}

/// @note Currently unable to extend lists or add new attributes with this
///  function (the asserts will fire if you try)
void Table::setAttr(const QString &attr, const QVariant &value)
{
  // qDebug() << "setAttr(" << attr << ", " << value.toString() << ")";
  int pos = attr.indexOf('/');
  if (pos > 0)
  {
    const QString a = attr.left(pos);
    const QString b = attr.mid(pos + 1);

    // More of the path to go.
    // This is more complicated than you might think as QVariant::value<>()
    // returns a converted copy and not a reference, so we have to recurse
    // and set the updated temporary table.
    if (a[0].isDigit())
    {
      const int index = a.toInt();
      Q_ASSERT(index > 0);
      Q_ASSERT(index <= this->list.size());
      // Need to make a copy of the value, update it and then write it back again
      auto t = this->list[index - 1].value<Table>();
      t.setAttr(b, value);
      this->list[index - 1].setValue(t);
    }
    else
    {
      Q_ASSERT(this->dictionary.contains(a));
      auto t = this->dictionary[a].value<Table>();
      t.setAttr(b, value);
      this->dictionary[a].setValue(t);
    }
  }
  else
  {
    // Just been asked for the leaf
    if (attr[0].isDigit())
    {
      const int index = attr.toInt();
      Q_ASSERT(index > 0);
      Q_ASSERT(index <= this->list.size());
      this->list[index - 1].setValue(value);
    }
    else
    {
      Q_ASSERT(this->dictionary.contains(attr));
      this->dictionary[attr].setValue(value);
    }
  }

  // Check we got what we wanted (at least while developing)
  Q_ASSERT(getAttr(attr) == value);
}

QString Table::getString(const QString &attr) const
{
    return getAttr(attr).toString();
}

void Table::setString(const QString &attr, const QString &value)
{
    qDebug() << "setString(" << attr << ", " << value << ")";
    setAttr(attr, value);
}

int Table::getInt(const QString &attr) const
{
    return getAttr(attr).toInt();
}

double Table::getDouble(const QString &attr) const
{
    return getAttr(attr).toDouble();
}

Table Table::getTable(const QString &attr) const
{
    return getAttr(attr).value<Table>();
}

int Table::getSequenceSize(const QString &attr) const
{
    return getTable(attr).hash();
}

};  // namespace LuaParser
