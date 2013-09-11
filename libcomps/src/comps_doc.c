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

#include "comps_doc.h"
#include <stdio.h>
#include <assert.h>

void comps_doc_create(COMPS_Doc* doc, COMPS_Object **args) {
    doc->objects = (COMPS_ObjDict*) comps_object_create(&COMPS_ObjDict_ObjInfo,
                                                       NULL);
    doc->log = comps_log_create(0);
    if (args && args[0]->obj_info == &COMPS_Str_ObjInfo)
        doc->encoding = (COMPS_Str*) comps_object_copy(args[0]);
}
COMPS_CREATE_u(doc, COMPS_Doc)

void comps_doc_copy(COMPS_Doc *doc_dst, COMPS_Doc *doc_src) {
    doc_dst->encoding = (COMPS_Str*) comps_object_copy((COMPS_Object*)doc_src->encoding);
    doc_dst->objects = (COMPS_ObjDict*) comps_object_copy((COMPS_Object*)doc_src->objects);
}
COMPS_COPY_u(doc, COMPS_Doc)

void comps_doc_destroy(COMPS_Doc *doc) {
    if (doc != NULL) {
    comps_object_destroy((COMPS_Object*)doc->objects);
    comps_object_destroy((COMPS_Object*)doc->encoding);
    }
}
COMPS_DESTROY_u(doc, COMPS_Doc)

signed char comps_doc_cmp_u(COMPS_Object *obj1, COMPS_Object *obj2) {
    if (!comps_object_cmp((COMPS_Object*)((COMPS_Doc*)obj1)->encoding,
                          (COMPS_Object*)((COMPS_Doc*)obj2)->encoding))
                            return 0;
    return (!comps_object_cmp((COMPS_Object*)((COMPS_Doc*)obj1)->objects,
                              (COMPS_Object*)((COMPS_Doc*)obj2)->objects));
}

inline void comps_doc_clear(COMPS_Doc *doc) {
    if (doc == NULL) return;
    //comps_objdict_clear(doc->objects);
}

COMPS_DOC_GETOBJLIST(groups)        /*comps_doc.h macro*/
COMPS_DOC_GETOBJLIST(categories)    /*comps_doc.h macro*/
COMPS_DOC_GETOBJLIST(environments)  /*comps_doc.h macro*/
COMPS_DOC_GETOBJDICT(langpacks)     /*comps_doc.h macro*/
COMPS_DOC_GETOBJDICT(blacklist)     /*comps_doc.h macro*/
COMPS_DOC_GETOBJDICT(whiteout)      /*comps_doc.h macro*/

void comps2xml_f(COMPS_Doc * doc, char *filename, char stdoutredirect) {
    xmlDocPtr xmldoc;
    int retc;
    char *str;

    doc->log->redirect2output = stdoutredirect;
    xmlTextWriterPtr writer = xmlNewTextWriterDoc(&xmldoc, 0);
    
    str = comps_object_tostr((COMPS_Object*)doc->encoding);
    retc = xmlTextWriterStartDocument(writer, NULL, str, NULL);
    free(str);
    if (retc<0)
        comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
    comps_doc_xml(doc, writer);
    retc = xmlTextWriterEndDocument(writer);
    if (retc<0)
        comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);

    retc = xmlSaveFormatFileEnc(filename, xmldoc, NULL, 1);
    if (retc<0)
        comps_log_error(doc->log, filename, COMPS_ERR_WRITEF, 0, 0, 0);

    xmlFreeTextWriter(writer);
    xmlFreeDoc(xmldoc);
    xmlCleanupParser();
    xmlMemoryDump();
}

char* comps2xml_str(COMPS_Doc *doc) {
    xmlDocPtr xmldoc;
    const char *xmlstr;
    char *str;
    char *ret;
    int retc;

    xmlBuffer *xmlbuff = xmlBufferCreate();
    xmlOutputBuffer *xmlobuff = xmlOutputBufferCreateBuffer(xmlbuff, 0);
    xmlTextWriterPtr writer = xmlNewTextWriterDoc(&xmldoc, 0);

    str = comps_object_tostr((COMPS_Object*)doc->encoding);
    retc = xmlTextWriterStartDocument(writer, NULL, str, NULL);
    free(str);

    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
    comps_doc_xml(doc, writer);
    retc = xmlTextWriterEndDocument(writer);
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
    xmlSaveFormatFileTo(xmlobuff, xmldoc, NULL, 1);

    xmlFreeTextWriter(writer);
    xmlFreeDoc(xmldoc);

    xmlstr =(char*) xmlBufferContent(xmlbuff);
    ret =  malloc(sizeof(char) * (strlen(xmlstr)+1));
    if (ret == NULL) {
        xmlBufferFree(xmlbuff);
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    } else {
        memcpy(ret, xmlstr, sizeof(char)*(strlen(xmlstr)+1));
    }
    xmlBufferFree(xmlbuff);

    xmlCleanupParser();
    xmlMemoryDump();
    return ret;
}

void comps_doc_xml(COMPS_Doc *doc, xmlTextWriterPtr writer) {
    COMPS_ObjListIt *it;
    COMPS_ObjList *list;
    int retc;

    retc = xmlTextWriterWriteDTD(writer, BAD_CAST "comps",
                                         BAD_CAST "-//Red Hat, Inc.//DTD Comps"
                                                  " info//EN",
                                         BAD_CAST "comps.dtd", NULL);
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);

    retc = xmlTextWriterStartElement(writer, BAD_CAST "comps");
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
    list = comps_doc_groups(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            comps_docgroup_xml((COMPS_DocGroup*)it->comps_obj, writer, doc->log);
        }
    }
    list = comps_doc_categories(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            comps_doccategory_xml((COMPS_DocCategory*)it->comps_obj, writer, doc->log);
        }
    }
    list = comps_doc_environments(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            comps_docenv_xml((COMPS_DocEnv*)it->comps_obj, writer, doc->log);
        }
    }
    retc = xmlTextWriterEndElement(writer);
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
}

COMPS_ObjectInfo COMPS_Doc_ObjInfo = {
    .obj_size = sizeof(COMPS_Doc),
    .constructor = &comps_doc_create_u,
    .destructor = &comps_doc_destroy_u,
    .copy = &comps_doc_copy_u,
    .obj_cmp = &comps_doc_cmp_u
};
