Libcomps unioning policy
========================

Object Unioning
---------------

Complex Object unioning policy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    * Two libcomps.Comps object can be unioned into one by __add__ operator
      like `o = o1 + o2`. This operator can be applicated on `libcomps.Category`
      `libcomps.Group`, `libcomps.Environment`

    * New object have all properties of first operand (o1). If there's some
      properties in second operand(o2) overlaying first operand properties,
      theese properties will be overwritten in result (o) by second operand
      properties (o2).

    * Inner list and dict objects are unioned by rules bellow
    
List Object unioning policy
~~~~~~~~~~~~~~~~~~~~~~~~~~~
    * libcomps list objects are unioned by Append-Replace-Append policy. That
      means:

        - all items from first list is appended to result list
        - if there's some item in second list which is equal some item in
          result list, item in result list will be overwritten by item from
          second list(position of item in result list remans same!).
        - rest of items in second list is appeneded in second list order
          to result list
    * List items are compared by id attribute (group, category, evinronment) or
      by name attribute (package, groupid)

Dict Object unioning policy
~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Unioning two libcomps dict objects means that result will have all
     (key,value) pair from first dict, overlaied pair with second dict is
     overwritten and rest of pairs from second dist is set to result dict.
     (All libcomps dict have inner alphabetical order!)

Examples
--------

.. code-block:: python

   c1 = libcomps.Category()
   c1.id  = "c1"
   c1.desc = "category 1"
   c1.group_list.append("groupid1")
   c2.group_list.append("groupid3")
   c2 = libcomps.Category()
   c2.id  = "c2"
   c2.group_list.append("groupid2")
   c2.group_list.append("groupid3")

   c = c1 + c2
   print c.id
   >>> c2
   print c.description
   >>> category 1
   print c.group_list
   >>> <COMPS_DocGroupId name='g1' default='False'>,
       <COMPS_DocGroupId name='g2' default='False'>,
       <COMPS_DocGroupId name='g3' default='False'>]

   cl1 = libcomps.CategoryList()
   cl2 = libcomps.CategoryList()


   c1 = libcomps.Category(id="c1")
   c1.group_ids.append("gid1 1")
   c1.group_ids.append("gid1 2")

   c3 = libcomps.Category(id="c3")
   c3.group_ids.append("gid3 1")
   c3.group_ids.append("gid3 2")

   c4 = libcomps.Category(id="c4")

   c2 = libcomps.Category(id="c2")
   c2.group_ids.append("gid2 1")
   c2.group_ids.append("gid2 2")
   cl1.append(c1)
   cl1.append(c2)
   cl1.append(c3)
   cl1.append(c4)

   c5 = libcomps.Category(id="c5")

   c3 = libcomps.Category(id="c3")
   c3.group_ids.append("gid3 1")
   c3.group_ids[0].default = True

   c6 = libcomps.Category(id="c6")

   c2 = libcomps.Category(id="c4")
   c2.group_ids.append("gid2 1")
   c2.group_ids.append("gid2 3")

   cl2.append(c2)
   cl2.append(c3)
   cl2.append(c5)
   cl2.append(c6)


   cl = cl1 + cl2
   ids = [x.id for x in cl]
   print ids
   >>> [u'c1', u'c2', u'c3', u'c4', u'c5', u'c6']
   print cl["c2"].name
   >>> c2_2
   print cl["c2"].group_ids
   >>> [<COMPS_DocGroupId name='gid2 1' default='False'>,
        <COMPS_DocGroupId name='gid2 1' default='False'>]
   print cl["c3"].name
   >>> c3_2
   print cl["c3"].group_ids
   >>> [<COMPS_DocGroupId name='gid3 1' default='True'>,
        <COMPS_DocGroupId name='gid3 2' default='False'>]

