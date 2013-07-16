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
 * along with this program; if not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 */

#include "comps_elem.h"

const char comps_xml_elems_required[] = {
                [COMPS_ELEM_DOC] = 1,
                [COMPS_ELEM_GROUP] =1,
                [COMPS_ELEM_ID] = 1,
                [COMPS_ELEM_NAME] = 1,
                [COMPS_ELEM_DESC] = 1,
                [COMPS_ELEM_DEFAULT] = 1,
                [COMPS_ELEM_LANGONLY] = 0,
                [COMPS_ELEM_USERVISIBLE] = 1,
                [COMPS_ELEM_PACKAGELIST] = 1,
                [COMPS_ELEM_PACKAGEREQ] = 1,
                [COMPS_ELEM_CATEGORY] = 1,
                [COMPS_ELEM_GROUPLIST] = 1,
                [COMPS_ELEM_GROUPID] = 1,
                [COMPS_ELEM_DISPLAYORDER] = 0,
                [COMPS_ELEM_ENV] = 0,
                [COMPS_ELEM_OPTLIST] = 1 ,
                //[COMPS_ELEM_OPTID] = 1,
                [COMPS_ELEM_IGNOREDEP] = 1,
                [COMPS_ELEM_WHITEOUT] = 0,
                [COMPS_ELEM_BLACKLIST] = 0,
                [COMPS_ELEM_PACKAGE] = 1,
                [COMPS_ELEM_LANGPACKS] = 0,
                [COMPS_ELEM_MATCH] = 1
};

char * comps_elem_get_name(const COMPS_ElemType type) {
    switch (type) {
        case COMPS_ELEM_UNKNOWN:
        case COMPS_ELEM_NONE:
            return "";
        break;
        case COMPS_ELEM_DOC:
            return "comps";
        break;
        case COMPS_ELEM_GROUP:
            return "group";
        break;
        case COMPS_ELEM_ID:
            return "id";
        break;
        case COMPS_ELEM_NAME:
            return "name";
        break;
        case COMPS_ELEM_DESC:
            return "description";
        break;
        case COMPS_ELEM_DEFAULT:
            return "default";
        break;
        case COMPS_ELEM_USERVISIBLE:
            return "uservisible";
        break;
        case COMPS_ELEM_PACKAGELIST:
            return "packagelist";
        break;
        case COMPS_ELEM_PACKAGEREQ:
            return "packagereq";
        break;
        case COMPS_ELEM_CATEGORY:
            return "category";
        break;
        case COMPS_ELEM_GROUPLIST:
            return "grouplist";
        break;
        case COMPS_ELEM_GROUPID:
            return "groupid";
        break;
        case COMPS_ELEM_LANGONLY:
            return "langonly";
        break;
        case COMPS_ELEM_DISPLAYORDER:
            return "display_order";
        break;
        case COMPS_ELEM_ENV:
            return "environment";
        break;
        case COMPS_ELEM_OPTLIST:
            return "optionlist";
        break;
        case COMPS_ELEM_IGNOREDEP:
            return "ignoredep";
        break;
        case COMPS_ELEM_WHITEOUT:
            return "whiteout";
        break;
        case COMPS_ELEM_BLACKLIST:
            return "blacklist";
        break;
        case COMPS_ELEM_PACKAGE:
            return "package";
        break;
        case COMPS_ELEM_LANGPACKS:
            return "langpacks";
        break;
        case COMPS_ELEM_MATCH:
            return "match";
        break;
    }
    return NULL;
}

COMPS_ElemType comps_elem_get_type(char * name) {
    COMPS_ElemType type;
    if (name == NULL) return COMPS_ELEM_NONE;
    if (strcmp(name, "comps") == 0) type = COMPS_ELEM_DOC;
    else if (strcmp(name, "group") == 0) type = COMPS_ELEM_GROUP;
    else if (strcmp(name, "id") == 0) type = COMPS_ELEM_ID;
    else if (strcmp(name, "name") == 0) type = COMPS_ELEM_NAME;
    else if (strcmp(name, "description") == 0) type = COMPS_ELEM_DESC;
    else if (strcmp(name, "default") == 0) type = COMPS_ELEM_DEFAULT;
    else if (strcmp(name, "uservisible") == 0) type = COMPS_ELEM_USERVISIBLE;
    else if (strcmp(name, "packagelist") == 0) type = COMPS_ELEM_PACKAGELIST;
    else if (strcmp(name, "packagereq") == 0) type = COMPS_ELEM_PACKAGEREQ;
    else if (strcmp(name, "category") == 0) type = COMPS_ELEM_CATEGORY;
    else if (strcmp(name, "grouplist") == 0) type = COMPS_ELEM_GROUPLIST;
    else if (strcmp(name, "groupid") == 0) type = COMPS_ELEM_GROUPID;
    else if (strcmp(name, "display_order") == 0) type = COMPS_ELEM_DISPLAYORDER;
    else if (strcmp(name, "langonly") == 0) type = COMPS_ELEM_LANGONLY;
    else if (strcmp(name, "environment") == 0) type = COMPS_ELEM_ENV;
    else if (strcmp(name, "optionlist") == 0) type = COMPS_ELEM_OPTLIST;
    else if (strcmp(name, "langpacks") == 0) type = COMPS_ELEM_LANGPACKS;
    else if (strcmp(name, "match") == 0) type = COMPS_ELEM_MATCH;
    else if (strcmp(name, "blacklist") == 0) type = COMPS_ELEM_BLACKLIST;
    else if (strcmp(name, "package") == 0) type = COMPS_ELEM_PACKAGE;
    else if (strcmp(name, "whiteout") == 0) type = COMPS_ELEM_WHITEOUT;
    else if (strcmp(name, "ignoredep") == 0) type = COMPS_ELEM_IGNOREDEP;
    else type = COMPS_ELEM_UNKNOWN;
    return type;

}

void comps_elem_attr_destroy(void *attr) {
    if (attr == NULL) return;
    free(((COMPS_ElemAttr*)attr)->name);
    free(((COMPS_ElemAttr*)attr)->val);
    free(attr);
}

COMPS_ElemAttr * comps_elem_attr_create(const char *name, const char *val) {

    COMPS_ElemAttr *attr;
    if ((attr = malloc(sizeof(*attr))) == NULL)
        return NULL;
    if ((attr->name = malloc(sizeof(char)*(strlen(name)+1))) == NULL) {
        free(attr);
        return NULL;
    }
    if ((attr->val = malloc(sizeof(char)*(strlen(val)+1))) == NULL) {
        free(attr->name);
        free(attr);
        return NULL;
    }
    memcpy(attr->name, name, sizeof(char)*(strlen(name)+1));
    memcpy(attr->val, val, sizeof(char)*(strlen(val)+1));
    return attr;
}

COMPS_Elem* comps_elem_create(const char * s, const char ** attrs,
                              COMPS_ElemType type) {
    COMPS_Elem *elem;
    char *val;
    if ((elem = malloc(sizeof(COMPS_Elem))) == NULL)
        return NULL;

    elem->type = type;
    if (type == COMPS_ELEM_UNKNOWN) {
        if ((elem->name = malloc(sizeof(char) * (strlen(s)+1))) == NULL) {
            free(elem);
            return NULL;
        }
        memcpy(elem->name, s, (strlen(s)+1) * sizeof(char));
    } else {
        elem->name = NULL;
    }

    /* Add attributes to elem */
    elem->attrs = comps_dict_create(NULL, NULL, &free);
    if (attrs != NULL) {
        for (; *attrs != NULL; attrs += 2) {
            val = malloc((strlen(*(attrs+1))+1)*sizeof(char));
            if (val == NULL) return NULL;
            memcpy(val, *(attrs+1), sizeof(char) * (strlen(*(attrs+1))+1));
            comps_dict_set(elem->attrs, (char*)*attrs, val);
        }
    }
    return elem;
}

void comps_elem_destroy(void * elem)
{
    if (elem == NULL) return;
    if (((COMPS_Elem*)elem)->name) free(((COMPS_Elem*)elem)->name);
    comps_dict_destroy(((COMPS_Elem*)elem)->attrs);
    free(elem);
}
