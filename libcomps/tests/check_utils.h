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

#ifndef CHECK_UTILS_H
#define CHECK_UTILS_H

#include <stdio.h>
#include "../src/comps_doc.h"

const char * pkg_type2str(COMPS_PackageType type);
void print_package(COMPS_Object *obj);
void print_gid(COMPS_Object *obj);
void print_group(COMPS_Object *obj);
void print_category(COMPS_Object *obj);

#endif
