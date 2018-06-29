:mod:`libcomps` - Libcomps objects
==================================

.. automodule:: libcomps

    .. autoexception: ParserError
    .. autoexception: XMLGenError

    libcomps.Comps
    --------------
    .. autoclass:: Comps
        :members:

        .. note::

            :members: blacklist, langpacks, whiteout

            are Fedora objects only

    libcomps.GroupList
    ------------------

    .. autoclass:: GroupList
        :inherited-members:
        :members:

        is class mostly similar to normal python list
        with few diferences:

        * accepting only specified type of object.
          :py:class:`GroupList` devours only :py:class:`Group` instance.
        * accepting string argument in getter/setter items methods
          as identificator of objects by id.
        * run validation procedure on absorbed object so you can't
          append/insert/set invalid :py:class:`Group` object

        .. code-block:: python

            g1 = libcomps.Group()
            g1.id = "g1"
            comps.categories.append(g1)

            g2 = libcomps.Group()
            g2.id = "g2"
            comps.categories.append(g2)

            g = comps.groups["g2"]
            comps.categories["g1"] = libcomps.Group()


    libcomps.CategoryList
    ---------------------

    .. autoclass:: CategoryList
        :inherited-members:
        :members:

        is class mostly similar to normal python list
        with few diferences:

        * accepting only specified type of object.
          :py:class:`CategoryList` devours only :py:class:`Category` instance.
        * accepting string argument in getter/setter items methods
          as identificator of objects by id.
        * run validation procedure on absorbed object so you can't
          append/insert/set invalid :py:class:`Category` object

        .. code-block:: python

            c1 = libcomps.Category()
            c1.id = "c1"
            comps.categories.append(c1)

            c2 = libcomps.Category()
            c2.id = "c2"
            comps.categories.append(c2)

            c = comps.groups["c2"]
            comps.categories["c1"] = libcomps.Category()


    libcomps.EnvList
    ---------------------

    .. autoclass:: EnvList
        :inherited-members:
        :members:

        is class mostly similar to normal python list
        with few diferences:

        * accepting only specified type of object.
          :py:class:`EnvList` devours only :py:class:`Environment` instance.
        * accepting string argument in getter/setter items methods
          as identificator of objects by id.
        * run validation procedure on absorbed object so you can't
          append/insert/set invalid :py:class:`Environment` object

        .. code-block:: python

            e1 = libcomps.Environment()
            e1.id = "e1"
            comps.environments.append(e1)

            e2 = libcomps.Environment()
            e2.id = "e2"
            comps.environments.append(e2)

            e = comps.groups["e2"]
            comps.categories["e1"] = libcomps.Environment()

    libcomps.Group
    --------------

    .. autoclass:: Group
        :members:

    libcomps.PackageList
    --------------------

    .. autoclass:: PackageList
        :members:

        is class mostly similar to normal python list
        with few diferences:

        * accepting only specified type of object.
          :py:class:`PackageList` devours only :py:class:`Package` instance.
        * accepting string argument in getter/setter items methods
          as identificator of package name.
        * run validation procedure on absorbed object so you can't
          append/insert/set invalid :py:class:`Package` object


    libcomps.Package
    ----------------

    .. autoclass:: Package
        :members:

    libcomps.Category
    -----------------

    .. autoclass:: Category
        :members:

    libcomps.Environment
    --------------------

    .. autoclass:: Environment
        :members:

    libcomps.IdList
    -----------------

    .. autoclass:: IdList
        :members:

        is class mostly similar to normal python list
        with few diferences:

        * :py:class:`IdList` accepting only
          unicode/string/:py:class:`libcomps.GroupId` (string/unicode object
          are implicitly converted to :py:class:`libcomps.GroupId`)
        * run validation procedure on absorbed object so you can't
          append/insert/set invalid :py:class:`GroupId` object

    libcomps.GroupId
    ----------------

    .. autoclass:: GroupId
        :members:

    libcomps.StrDict
    ----------------
    
    .. autoclass:: StrDict
        :members:
        :inherited-members:

    is very lightweight version of python dict

        * libcomps.StrDict can have only string/unicode keys and
          accepts only string/unicode object
    

    libcomps.MDict
    ----------------

    .. autoclass:: MDict
        :members:
        :inherited-members:

    is class similar to python dict, except of these properties:

        * libcomps.MDict can have only string/unicode keys and accepts only
          list of string or libcomps.StrSeq which is also object returned
          from dict.__get__()
        * output of StrDict.keys() is in alphabetical order ()

    libcomps.StrSeq
    ----------------

    .. autoclass:: StrSeq
        :members:
        :inherited-members:

    is class similar to list, but accepts only string/unicode objects.
