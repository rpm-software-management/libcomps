/* libcomps - C alternative to yum.comps library
 * Copyright (C) 2013 Jindrich Luza
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to  Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA
 */

#ifndef COMPS_TYPES_H
#define COMPS_TYPES_H

typedef struct COMPS_Log COMPS_Log;
typedef struct COMPS_LogEntry COMPS_LogEntry;
//typedef struct COMPS_ElemAttr COMPS_ElemAttr;
//typedef struct COMPS_Elem COMPS_Elem;
//typedef struct COMPS_ElemAttrInfo COMPS_ElemAttrInfo;

/*struct COMPS_ElemAttr {
    char *name;
    char *val;
};

struct COMPS_Elem {
    char *name;
    COMPS_Dict *attrs;
    COMPS_ElemType type;
};

struct COMPS_ElemAttrInfo {
    char *name;
    signed char (*val_check)(const char*);
};
*/
#endif
