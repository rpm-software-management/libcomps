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

#include "comps_types.h"

COMPS_XMLOptions COMPS_XMLDefaultOptions = {
    .empty_groups = false,
    .empty_categories = false,
    .empty_environments = false,
    .empty_langpacks = false,
    .empty_blacklist = false,
    .empty_whiteout = false,
    .empty_packages = false,
    .empty_grouplist = false,
    .empty_optionlist = false,
    .uservisible_explicit = false,
    .biarchonly_explicit = false,
    .default_explicit = false,
    .gid_default_explicit = false,
    .bao_explicit = false,
    .arch_output = false
};


