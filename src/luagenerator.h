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
#ifndef LUAGENERATOR_H
#define LUAGENERATOR_H

#include "luaparser.h"
#include "luatable.h"

class LuaGenerator
{
   public:
    LuaGenerator();

    static QString Generate(const QVariant &value, int indent = 0);

    static QString Generate(const LuaParser::NamedVariant &nv, int indent = 0);

    static QString Generate(const LuaParser::Table &table, int indent = 0);
};

#endif // LUAGENERATOR_H
