import _libpycomps as libcomps
import unittest
import tempfile
import os

class CategoryListTest(unittest.TestCase):
    def setUp(self):
        self.catlist = libcomps.CategoryList()

    #@unittest.skip("skip")
    def test_basic(self):
        c1 = libcomps.Category()
        with self.assertRaises(TypeError):
            c2 = libcomps.Category("cat1", "category1", "cat desc", 1, 3)
        c = libcomps.Category("cat1", "category1", "cat desc", 1)
        self.catlist.append(c)
        c1 = self.catlist[0]
        self.assertTrue(c == self.catlist[0])
        self.assertTrue(len(self.catlist) == 1)
        del self.catlist[0]
        self.assertTrue(len(self.catlist) == 0)
        self.catlist.append(libcomps.Category("cat1", "category1", "cat desc", 1))
        self.catlist[0] = c1
        with self.assertRaises(IndexError):
            x = self.catlist[1]
        self.assertTrue(c != None)

    #@unittest.skip("skip")
    def test_append(self):
        c1 = libcomps.Category("cat1", "category1", "cat desc", 1)
        c2 = libcomps.Category("cat1", "category1", "cat desc", 1)

        with self.assertRaises(TypeError):
            self.catlist.append([0])
        self.catlist.append(c1)
         
        self.assertTrue(c1 == self.catlist[0])
        self.assertTrue(len(self.catlist) == 1)
        self.catlist.append(c2)
        self.assertTrue(c2 == self.catlist[1])
        self.assertTrue(len(self.catlist) == 2)

    #@unittest.skip("skip")
    def test_slice(self):
        self.catlist.append(libcomps.Category("cat1", "category1", "cat desc", 1))
        self.catlist.append(libcomps.Category("cat2", "category2", "cat desc", 1))
        self.catlist.append(libcomps.Category("cat3", "category3", "cat desc", 1))
        self.catlist.append(libcomps.Category("cat4", "category4", "cat desc", 1))
        self.catlist.append(libcomps.Category("cat5", "category5", "cat desc", 1))

        subseq = self.catlist[1:3]
        self.assertTrue(len(subseq) == 2)
        self.assertTrue(self.catlist[1] == subseq[0])
        self.assertTrue(self.catlist[2] == subseq[1])
        subseq = self.catlist[5:8]
        self.assertTrue(len(subseq) == 0)

        newseq = libcomps.CategoryList()
        newseq.append(libcomps.Category("cat6", "category6", "cat desc", 1))
        newseq.append(libcomps.Category("cat7", "category7", "cat desc", 1))
        newseq.append(libcomps.Category("cat8", "category8", "cat desc", 1))

        self.catlist[4:7:1] = newseq

        self.assertTrue(self.catlist[4] == newseq[0])
        self.assertTrue(self.catlist[5] == newseq[1])
        self.assertTrue(self.catlist[6] == newseq[2])

        newseq.append(libcomps.Category("cat9", "category9", "cat desc", 1))
        newseq.append(libcomps.Category("cat10", "category10", "cat desc", 1))

        del self.catlist[1:5]
        self.assertTrue(len(self.catlist) == 3)

        self.catlist.append(libcomps.Category("cat9", "category9",
                                              "cat desc", 1))
        self.catlist.append(libcomps.Category("cat10", "category10",
                                              "cat desc", 1))
        del self.catlist[0:4:2]
        self.assertTrue(self.catlist[0] == newseq[1])
        self.assertTrue(self.catlist[1] == newseq[3])
        self.assertTrue(self.catlist[2] == newseq[4])

    #@unittest.skip("skip")
    def test_concat(self):
        self.catlist.append(libcomps.Category("cat1", "category1", "cat desc", 1))
        self.catlist.append(libcomps.Category("cat2", "category2", "cat desc", 1))
        self.catlist.append(libcomps.Category("cat3", "category3", "cat desc", 1))
        self.catlist.append(libcomps.Category("cat4", "category4", "cat desc", 1))
        self.catlist[3].group_ids.append("g1")
        self.catlist[3].group_ids.append("g2")
        self.catlist[3].group_ids.append("g3")

        newcats = libcomps.CategoryList()
        newcats.append(libcomps.Category("cat1", "category1", "cat desc", 1))
        concated = self.catlist + newcats
        #print set(concated)
        #self.assertTrue(set(concated) == set(self.catlist))
        #self.assertTrue(set(concated) == set(self.catlist))
        newcats.append(libcomps.Category("cat4", "category4", "cat desc", 1))
        newcats.append(libcomps.Category("cat5", "category5", "cat desc", 1))
        newcats[0].group_ids.append("g2")
        newcats[0].group_ids.append("g5")
        newcats[1].group_ids.append("g4")
        newcats[1].group_ids.append("g5")

        concated = self.catlist + newcats
        self.assertFalse(concated == self.catlist)
        self.assertTrue(concated[2] == newcats[0])
        self.assertTrue(len(concated) == 5)

        #print (concated)
        self.assertTrue(len(concated[3].group_ids) == 5)

    #@unittest.skip("skip")
    #def test_repeat(self):
    #    self.catlist.append(libcomps.Category("cat1", "category1", "cat desc", 1))
    #    self.catlist.append(libcomps.Category("cat2", "category2", "cat desc", 1))
    #    self.catlist.append(libcomps.Category("cat3", "category3", "cat desc", 1))
    #    repeated = 3 * self.catlist
    #    self.assertTrue(len(repeated) == 9)
    #    self.assertTrue(repeated[0] == repeated[3] == repeated[6])
    #    self.assertTrue(repeated[1] == repeated[4] == repeated[7])
    #    self.assertTrue(repeated[2] == repeated[5] == repeated[8])

    #@unittest.skip("skip")
    def test_group_ids(self):
        category = libcomps.Category("cat1","category1","cat desc", 1)
        self.assertTrue(len(category.group_ids) == 0)
        category.group_ids.append("g1")
        self.assertTrue(len(category.group_ids) == 1)
        gids = libcomps.IdList()
        gids.append("g2")
        category.group_ids = gids
        self.assertTrue(category.group_ids == gids)
        #print category.group_ids
        #print category

    #@unittest.skip("skip")
    def test_clear(self):
        cl = libcomps.CategoryList()
        cl.clear()
        self.assertTrue(len(cl) == 0)
        cl.append(libcomps.Category("cat1","category1","cat desc", 1))
        cl.clear()
        self.assertTrue(len(cl) == 0)


class GroupListTest(unittest.TestCase):
    def setUp(self):
        self.grouplist = libcomps.GroupList()

    #@unittest.skip("skip")
    def test_basic(self):
        c2 = libcomps.Group()
        with self.assertRaises(TypeError):
            c1 = libcomps.Group("group1", "group1", "group desc", 0,0,1,1)
        c = libcomps.Group("group1", "group1", "group desc", 0, 0, 0, "en")
        self.grouplist.append(c)
        z = self.grouplist[0]
        self.assertTrue(c == z)
        self.assertTrue(len(self.grouplist) == 1)
        self.grouplist.append(libcomps.Group("g2", "group2", "group desc",
                                             0, 0, 0, "en"))
        self.grouplist.append(libcomps.Group("g3", "group3", "group desc",
                                             0, 0, 0, "en"))

        del self.grouplist[0]
        self.assertTrue(len(self.grouplist) == 2)
        del self.grouplist[0]
        del self.grouplist[0]
        with self.assertRaises(IndexError):
            x = self.grouplist[0]
        self.assertTrue(c != None)

    #@unittest.skip("skip")
    def test_append(self):
        c1 = libcomps.Group("g1", "group1", "group desc", 0, 0, 0, "en")
        c2 = libcomps.Group("g2", "group2", "group desc", 0, 0, 0, "en")
        self.grouplist.append(c1)
        self.assertTrue(c1 == self.grouplist[0])
        self.assertTrue(len(self.grouplist) == 1)
        self.grouplist.append(c2)
        self.assertTrue(c2 == self.grouplist[1])
        self.assertTrue(len(self.grouplist) == 2)

    #@unittest.skip("skip")
    def test_slice(self):
        self.grouplist.append(libcomps.Group("g1", "group1", "group desc",
                                             0, 0, 0, "en"))
        self.grouplist.append(libcomps.Group("g2", "group2", "group desc",
                                             0, 0, 0, "en"))
        self.grouplist.append(libcomps.Group("g3", "group3", "group desc",
                                             0, 0, 0, "en"))
        self.grouplist.append(libcomps.Group("g4", "group4", "group desc",
                                             0, 0, 0, "en"))
        self.grouplist.append(libcomps.Group("g5", "group5", "group desc",
                                             0, 0, 0, "en"))

        subseq = self.grouplist[1:3]
        self.assertTrue(len(subseq) == 2)
        self.assertTrue(self.grouplist[1] == subseq[0])
        self.assertTrue(self.grouplist[2] == subseq[1])
        subseq = self.grouplist[5:8]
        self.assertTrue(len(subseq) == 0)

        newseq = libcomps.GroupList()
        newseq.append(libcomps.Group("g6", "group6", "group desc", 0, 0, 0, "en"))
        newseq.append(libcomps.Group("g7", "group7", "group desc", 0, 0, 0, "en"))

        self.grouplist[1:3] = newseq
        self.assertTrue(self.grouplist[1] == newseq[0])
        self.assertTrue(self.grouplist[2] == newseq[1])
        #with self.assertRaises(ValueError):
        self.grouplist[7:8] = newseq
        self.assertTrue(self.grouplist[5] == newseq[0])
        self.assertTrue(self.grouplist[6] == newseq[1])

        newseq = libcomps.GroupList()
        newseq.append(libcomps.Group("gX", "groupX", "group desc", 0, 0, 0, "en"))
        self.grouplist[1:5] = newseq

    #@unittest.skip("skip")
    def test_concat(self):
        self.grouplist.append(libcomps.Group("g1", "group1", "group desc",
                                             0, 0, 0, "en"))
        self.grouplist.append(libcomps.Group("g2", "group2", "group desc",
                                             0, 0, 0, "en"))
        self.grouplist.append(libcomps.Group("g3", "group3", "group desc",
                                             0, 0, 0, "en"))
        self.grouplist.append(libcomps.Group("g4", "group4", "group desc",
                                             0, 0, 0, "en"))

        newseq = libcomps.GroupList()

        concated = self.grouplist + newseq
        #print self.grouplist
        #print ""
        #print concated
        self.assertTrue(concated == self.grouplist)
        newseq.append(libcomps.Group("g3", "group3", "group desc", 0, 0, 0, "en"))
        newseq.append(libcomps.Group("g5", "group5", "group desc", 0, 0, 0, "en"))

        concated = self.grouplist + newseq
        self.assertFalse(concated == self.grouplist)
        self.assertTrue(concated[3] == newseq[0])
        self.assertTrue(len(concated) == 5)

    #@unittest.skip("skip")
    #def test_repeat(self):
    #    self.grouplist.append(libcomps.Group("g1", "group1", "group desc",
    #                                         0, 0, "en"))
    #    self.grouplist.append(libcomps.Group("g2", "group2", "group desc",
    #                                         0, 0, "en"))
    #    self.grouplist.append(libcomps.Group("g3", "group3", "group desc",
    #                                         0, 0, "en"))
    #    repeated = 3 * self.grouplist
    #    self.assertTrue(len(repeated) == 9)
    #    self.assertTrue(repeated[0] == repeated[3] == repeated[6])
    #    self.assertTrue(repeated[1] == repeated[4] == repeated[7])
    #    self.assertTrue(repeated[2] == repeated[5] == repeated[8])

    #@unittest.skip("skip")
    def test_group_packages(self):
        group = libcomps.Group("g1","group1","group desc", 0, 0, 0, "en")
        self.assertTrue(len(group.packages) == 0)
        group.packages.append(libcomps.Package("kernel-3.2",
                                               libcomps.PACKAGE_TYPE_MANDATORY))
        self.assertTrue(len(group.packages) == 1)
        packs = libcomps.PackageList()
        packs.append(libcomps.Package())
        group.packages = packs
        self.assertTrue(group.packages == packs)

    #@unittest.skip("skip")
    def test_clear(self):
        gl = libcomps.GroupList()
        gl.clear()
        self.assertTrue(len(gl) == 0)
        gl.append(libcomps.Group("g1","group1","group desc", 0, 0, 0, "en"))
        gl.clear()
        self.assertTrue(len(gl) == 0)

class EnvListTest(unittest.TestCase):
    def setUp(self):
        self.envlist = libcomps.EnvList()

    #@unittest.skip("skip")
    def test_basic(self):
        #with self.assertRaises(TypeError):
        #with self.assertRaises(TypeError):
        with self.assertRaises(TypeError):
            e1 = libcomps.Environment("e1", "env1", "environment desc", 1, 1)
        e2 = libcomps.Environment()
        e = libcomps.Environment("e1", "env1", "environment desc", 1)
        self.envlist.append(e)
        z = self.envlist[0]
        self.assertTrue(e == z)
        self.assertTrue(len(self.envlist) == 1)
        #self.grouplist.append(libcomps.Group("g2", "group2", "group desc", 0,0,0))
        #self.grouplist.append(libcomps.Group("g3", "group3", "group desc", 0,0,0))

        del self.envlist[0]
        self.assertTrue(len(self.envlist) == 0)
        with self.assertRaises(IndexError):
            x = self.envlist[0]
        self.assertTrue(e != None)

    #@unittest.skip("skip")
    def test_append(self):
        e1 = libcomps.Environment("e1", "env1", "Environment desc")
        e2 = libcomps.Environment("e2", "env2", "Environment desc")
        self.envlist.append(e1)
        self.assertTrue(e1 == self.envlist[0])
        self.assertTrue(len(self.envlist) == 1)
        self.envlist.append(e2)
        self.assertTrue(e2 == self.envlist[1])
        self.assertTrue(len(self.envlist) == 2)

    #@unittest.skip("skip")
    def test_slice(self):
        self.envlist.append(libcomps.Environment("g1", "group1", "group desc"))
        self.envlist.append(libcomps.Environment("g2", "group2", "group desc"))
        self.envlist.append(libcomps.Environment("g3", "group3", "group desc"))
        self.envlist.append(libcomps.Environment("g4", "group4", "group desc"))
        self.envlist.append(libcomps.Environment("g5", "group5", "group desc"))

        subseq = self.envlist[1:3]
        self.assertTrue(len(subseq) == 2)
        self.assertTrue(self.envlist[1] == subseq[0])
        self.assertTrue(self.envlist[2] == subseq[1])
        subseq = self.envlist[5:8]
        self.assertTrue(len(subseq) == 0)

        newseq = libcomps.EnvList()
        newseq.append(libcomps.Environment("g6", "group6", "group desc"))
        newseq.append(libcomps.Environment("g7", "group7", "group desc"))

        self.envlist[1:3] = newseq
        self.assertTrue(self.envlist[1] == newseq[0])
        self.assertTrue(self.envlist[2] == newseq[1])
        self.envlist[7:8] = newseq
        self.assertTrue(self.envlist[5] == newseq[0])
        self.assertTrue(self.envlist[6] == newseq[1])

        newseq = libcomps.EnvList()
        newseq.append(libcomps.Environment("gX", "groupX", "group desc"))
        self.envlist[1:5] = newseq

    #@unittest.skip("skip")
    def test_concat(self):
        self.envlist.append(libcomps.Environment("e1", "env1", "env desc"))
        self.envlist.append(libcomps.Environment("e2", "env2", "env desc"))
        self.envlist.append(libcomps.Environment("e3", "env3", "env desc"))
        self.envlist.append(libcomps.Environment("e4", "env4", "env desc"))

        newseq = libcomps.EnvList()

        concated = self.envlist + newseq
        #print newseq
        #print concated
        self.assertTrue(concated == self.envlist)
        newseq.append(libcomps.Environment("e3", "env3", "env desc"))
        newseq.append(libcomps.Environment("e5", "env5", "env desc"))
        concated = self.envlist + newseq
        self.assertFalse(concated == self.envlist)
        self.assertTrue(concated[3] == newseq[0])

    #@unittest.skip("skip")
    #def test_repeat(self):
    #    self.envlist.append(libcomps.Environment("g1", "group1", "group desc"))
    #    self.envlist.append(libcomps.Environment("g2", "group2", "group desc"))
    #    self.envlist.append(libcomps.Environment("g3", "group3", "group desc"))
    #    repeated = 3 * self.envlist
    #    self.assertTrue(len(repeated) == 9)
    #    self.assertTrue(repeated[0] == repeated[3] == repeated[6])
    #    self.assertTrue(repeated[1] == repeated[4] == repeated[7])
    #    self.assertTrue(repeated[2] == repeated[5] == repeated[8])

    #@unittest.skip("skip")
    def test_group_packages(self):
        group = libcomps.Group("g1","group1","group desc", 0, 0, 0, "en")
        self.assertTrue(len(group.packages) == 0)
        group.packages.append(libcomps.Package("kernel-3.2",
                                               libcomps.PACKAGE_TYPE_MANDATORY))
        self.assertTrue(len(group.packages) == 1)
        packs = libcomps.PackageList()
        packs.append(libcomps.Package())
        group.packages = packs
        self.assertTrue(group.packages == packs)

    def test_clear(self):
        el = libcomps.EnvList()
        el.clear()
        self.assertTrue(len(el) == 0)
        el.append(libcomps.Environment("e1","env1","environment desc"))
        el.clear()
        self.assertTrue(len(el) == 0)

class CategoryTest(unittest.TestCase):
    def test_basic(self):
        c = libcomps.Category()
        c.name = "category1"
        c.id = "cat1"
        c.desc = "category 1 description"
        self.assertTrue(c.name == "category1")
        self.assertTrue(c.id == "cat1")
        self.assertTrue(c.desc == "category 1 description")
        c.id = None
        c.name = None
        c.desc = None
        self.assertTrue(c.name == None)
        self.assertTrue(c.id == None)
        self.assertTrue(c.desc == None)
        with self.assertRaises(TypeError):
            c.group_ids.append(None)
            c.group_ids.append(0)
        

    #@unittest.skip("skip")
    def test_add(self):
        c1 = libcomps.Category("cat1", "category 1", "cat desc", 0)
        c1.group_ids.append("g1")
        c1.group_ids.append("g2")
        c1.group_ids.append("g3")
        c2 = libcomps.Category("cat2", "category 2", "cat desc", 0)
        c2.group_ids.append("g1")
        c2.group_ids.append("g4")
        c2.group_ids.append("g5")
        c = c1 + c2
        self.assertTrue(len(c.group_ids) == 5)
        #print c.group_ids[0]
        self.assertTrue(c.group_ids[0] == c1.group_ids[0] and\
                        c.group_ids[1] == c1.group_ids[1] and\
                        c.group_ids[2] == c1.group_ids[2] and\
                        c.group_ids[3] == c2.group_ids[1] and\
                        c.group_ids[4] == c2.group_ids[2])
        c1 = libcomps.Category()
        c2 = libcomps.Category("cat2", "category 2", "cat desc", 0)
        c2.group_ids.append("g1")
        c = c1 + c2
        self.assertTrue(c == c2)
        c1 = libcomps.Category()
        c2 = libcomps.Category()
        c = c1 + c2
        self.assertTrue(c == c1)
        self.assertTrue(c == c2)
        #print c

    def test_langs(self):
        c1 = libcomps.Category("cat1", "category 1", "cat desc", 0)
        c1.name_by_lang["af"]="Administrasienutsgoed"
        c1.name_by_lang["bs"]="Administrativni alati"
        c1.name_by_lang["id"]="Peralatan Administrasi"

        c1.name_by_lang["af"]="Afrikaanse taalsteun"
        self.assertTrue(c1.name_by_lang["af"] == "Afrikaanse taalsteun")

        c2 = libcomps.Category("cat2", "category 2", "cat desc", 0)
        c2.name_by_lang["af"]="Afrikaanse taalsteun"
        c2.name_by_lang["bs"]="Administrativni alati"
        c2.name_by_lang["id"]="Peralatan Administrasi"
        
        self.assertTrue(c1.name_by_lang, c2.name_by_lang)

class GroupTest(unittest.TestCase):
    def test_basic(self):
        g = libcomps.Group()
        g.name = "group 1"
        g.id = "group1"
        g.desc = "group 1 description"
        self.assertTrue(g.name == "group 1")
        self.assertTrue(g.id == "group1")
        self.assertTrue(g.desc == "group 1 description")
        g.id = None
        g.name = None
        g.desc = None
        self.assertTrue(g.name == None)
        self.assertTrue(g.id == None)
        self.assertTrue(g.desc == None)

    #@unittest.skip("skip")
    def test_add(self):
        g1 = libcomps.Group("g1", "group 1", "group desc", 0, 0, 0, "en")
        g1.packages.append(libcomps.Package("oss", libcomps.PACKAGE_TYPE_DEFAULT))
        g1.packages.append(libcomps.Package("alsa", libcomps.PACKAGE_TYPE_DEFAULT))
        g1.packages.append(libcomps.Package("pulse", libcomps.PACKAGE_TYPE_DEFAULT))
        g2 = libcomps.Group("cat2", "category 2", "cat desc", 0, 0, 0, "en")
        g2.packages.append(libcomps.Package("oss", libcomps.PACKAGE_TYPE_DEFAULT))
        g2.packages.append(libcomps.Package("jack", libcomps.PACKAGE_TYPE_DEFAULT))
        g2.packages.append(libcomps.Package("portaudio", libcomps.PACKAGE_TYPE_DEFAULT))
        #print g1, g2
        g = g1 + g2
        self.assertTrue(len(g.packages) == 5)
        #print c.group_ids[0]
        self.assertTrue(g.packages[0] == g1.packages[0] and\
                        g.packages[1] == g1.packages[1] and\
                        g.packages[2] == g1.packages[2] and\
                        g.packages[3] == g2.packages[1] and\
                        g.packages[4] == g2.packages[2])
        g1 = libcomps.Group()
        g2 = libcomps.Group("g2", "group 2", "group desc", 0, 0, 0,"en")
        g2.packages.append(libcomps.Package("oss", libcomps.PACKAGE_TYPE_DEFAULT))
        g = g1 + g2
        self.assertTrue(g == g2)
        g1 = libcomps.Group()
        g2 = libcomps.Group()
        g = g1 + g2
        self.assertTrue(g == g1)
        self.assertTrue(g == g2)
        #print c

    def test_langs(self):
        g1 = libcomps.Group("g1", "group 1", "group desc", 0, 0, 0, "en")
        g1.name_by_lang["af"]="Administrasienutsgoed"
        g1.name_by_lang["bs"]="Administrativni alati"
        g1.name_by_lang["id"]="Peralatan Administrasi"

        g1.name_by_lang["af"]="Afrikaanse taalsteun"
        self.assertTrue(g1.name_by_lang["af"] == "Afrikaanse taalsteun")

        g2 = libcomps.Group("g2", "group 2", "group desc", 0, 0, 0, "en")
        g2.name_by_lang["af"]="Afrikaanse taalsteun"
        g2.name_by_lang["bs"]="Administrativni alati"
        g2.name_by_lang["id"]="Peralatan Administrasi"
        
        self.assertTrue(g1.name_by_lang, g2.name_by_lang)


class EnvTest(unittest.TestCase):
    def test_basic(self):
        e = libcomps.Environment()
        e.name = "environment 1"
        e.id = "environment1"
        e.desc = "environment 1 description"
        self.assertTrue(e.name == "environment 1")
        self.assertTrue(e.id == "environment1")
        self.assertTrue(e.desc == "environment 1 description")
        e.id = None
        e.name = None
        e.desc = None
        self.assertTrue(e.name == None)
        self.assertTrue(e.id == None)
        self.assertTrue(e.desc == None)

    #@unittest.skip("skip")
    def test_add(self):
        e1 = libcomps.Environment("e1", "env 1", "env desc", 0)
        e1.group_ids.append("g1")
        e1.group_ids.append("g2")
        e1.group_ids.append("g3")
        e1.option_ids.append("o2")
        e1.option_ids.append("o3")
        e2 = libcomps.Environment("e2", "env 2", "env desc", 0)
        e2.group_ids.append("g3")
        e2.group_ids.append("g4")
        e2.group_ids.append("g5")
        e2.option_ids.append("o1")
        e2.option_ids.append("o3")
        e = e1 + e2
        self.assertTrue(len(e.group_ids) == 5)
        self.assertTrue(len(e.option_ids) == 3)
        self.assertTrue(e.group_ids[0] == e1.group_ids[0] and\
                        e.group_ids[1] == e1.group_ids[1] and\
                        e.group_ids[2] == e1.group_ids[2] and\
                        e.group_ids[3] == e2.group_ids[1] and\
                        e.group_ids[4] == e2.group_ids[2])
        e1 = libcomps.Environment()
        e2 = libcomps.Environment("e2", "env 2", "env desc", 0)
        e2.group_ids.append("g1")
        e = e1 + e2
        self.assertTrue(e == e2)
        e1 = libcomps.Group()
        e2 = libcomps.Group()
        e = e1 + e2
        self.assertTrue(e == e1)
        self.assertTrue(e == e2)
        #print c

    def test_langs(self):
        e1 = libcomps.Environment("e1", "env 1", "env desc", 0)
        e1.name_by_lang["af"]="Administrasienutsgoed"
        e1.name_by_lang["bs"]="Administrativni alati"
        e1.name_by_lang["id"]="Peralatan Administrasi"

        e1.name_by_lang["af"]="Afrikaanse taalsteun"
        self.assertTrue(e1.name_by_lang["af"] == "Afrikaanse taalsteun")

        e2 = libcomps.Environment("e2", "env 2", "env desc", 0)
        e2.name_by_lang["af"]="Afrikaanse taalsteun"
        e2.name_by_lang["bs"]="Administrativni alati"
        e2.name_by_lang["id"]="Peralatan Administrasi"

        self.assertTrue(e1.name_by_lang, e2.name_by_lang)

class COMPSTest(unittest.TestCase):
    def setUp(self):
        self.comps = libcomps.Comps()

    #@unittest.skip("skip")
    def test_xml(self):
        self.comps.groups.append(libcomps.Group("g1", "group1", "group desc",
                                                0, 0, 0, "en"))
        self.comps.groups.append(libcomps.Group("g2", "group2", "group desc",
                                                0, 0, 0, "en"))
        self.comps.groups.append(libcomps.Group("g3", "group3", "group desc",
                                                0, 0, 0, "en"))
        g = self.comps.groups[0]
        g.packages.append(libcomps.Package("kernel", libcomps.PACKAGE_TYPE_MANDATORY))
        g = self.comps.groups[1]
        g.packages.append(libcomps.Package("glibc", libcomps.PACKAGE_TYPE_MANDATORY))
        g = self.comps.groups[2]
        g.packages.append(libcomps.Package("systemd", libcomps.PACKAGE_TYPE_MANDATORY))

        self.comps.categories.append(libcomps.Category("c1", "cat1", "cat desc", 1))
        c = self.comps.categories[0]
        c.group_ids.append("g1")
        self.comps.categories.append(libcomps.Category("c2", "cat2", "cat desc", 1))
        c = self.comps.categories[1]
        c.group_ids.append("g2")
        (h, fname) = tempfile.mkstemp()
        errs = self.comps.xml_f(fname)
        f = open(fname, "r")

        comps2 = libcomps.Comps()
        errs = comps2.fromxml_f(fname)
        self.assertTrue(len(errs) == 0, errs)

        compsdoc = comps2.xml_str()
        compsdoc = compsdoc[0:-5] # make some error
        self.assertTrue(len(comps2.groups) == 3)
        self.assertTrue(len(comps2.categories) == 2)
        self.assertTrue(len(comps2.environments) == 0)

        comps3 = libcomps.Comps()
        errs = comps3.fromxml_str(compsdoc)
        self.assertFalse(len(errs) == 0)
        self.assertTrue(len(comps3.groups) == 3)
        self.assertTrue(len(comps3.categories) == 2)
        self.assertTrue(len(comps3.environments) == 0)
        x = self.comps.xml_str()
        y = comps2.xml_str()
        self.assertTrue(x == y)
        os.remove(fname)

    #@unittest.skip("skip")
    def test_fedora(self):
        comps = libcomps.Comps()
        comps.fromxml_f("fedora_comps.xml")
        comps.xml_f("fed2.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("fed2.xml")
        self.assertTrue(comps == comps2)

    #@unittest.skip("skip")
    def test_sample(self):
        comps = libcomps.Comps()
        comps.fromxml_f("sample_comps.xml")
        comps.xml_f("sample2.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("sample2.xml")
        self.assertTrue(comps == comps2)

    #@unittest.skip("skip")
    def test_main(self):
        comps = libcomps.Comps()
        comps.fromxml_f("main_comps.xml")
        comps.xml_f("main2.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("main2.xml")
        x = comps.xml_str()
        y = comps2.xml_str()
        self.assertTrue(comps == comps2)
        self.assertTrue(y == x)
        #os.remove("main2.xml")

    #@unittest.skip("skip")
    def test_main_union(self):
        c1 = libcomps.Comps()
        c2 = libcomps.Comps()
        c2.fromxml_f('main_comps.xml')
        c = c1 + c2 # <--- sigsegvs here

    def test_main_loc(self):
        comps = libcomps.Comps()
        errors = comps.fromxml_f('main_comps.xml')
        g = comps.groups[0]
        print(g.desc_by_lang['cs'])

        comps = libcomps.Comps()
        errors = comps.fromxml_f('main_comps.xml')
        g = comps.groups[0]
        g1 = comps.groups[0]
        g.desc_by_lang = g1.desc_by_lang
        dbl1 = g.desc_by_lang
        dbl2 = g1.desc_by_lang
        self.assertTrue(g1.desc_by_lang == g.desc_by_lang)

    #@unittest.skip("skip")
    def test_union(self):
        comps = libcomps.Comps()
        comps.fromxml_f("sample_comps.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("sample2.xml")
        comps3 = comps + comps2
        self.assertTrue(comps == comps3)
        comps.fromxml_f("fedora_comps.xml")
        comps3 = comps + comps2
        self.assertFalse(comps == comps3)
        #print comps3.xml_str()
        gids_set = set()
        cids_set = set()
        eids_set = set()

        gids3_set = set()
        cids3_set = set()
        eids3_set = set()

        for g in comps.groups:
            gids_set.add(g.id)
        for c in comps.categories:
            cids_set.add(c.id)
        for e in comps.environments:
            eids_set.add(e.id)

        for g in comps2.groups:
            gids_set.add(g.id)
        for c in comps2.categories:
            cids_set.add(c.id)
        for e in comps2.environments:
            eids_set.add(e.id)

        for g in comps3.groups:
            gids3_set.add(g.id)
        for c in comps3.categories:
            cids3_set.add(c.id)
        for e in comps3.environments:
            eids3_set.add(e.id)

        self.assertTrue(gids3_set == gids_set)
        self.assertTrue(cids3_set == cids_set)
        self.assertTrue(eids3_set == eids_set)

if __name__ == "__main__":
    #unittest.main()
    suite = unittest.TestLoader().loadTestsFromTestCase(CategoryTest)
    unittest.TextTestRunner(verbosity=2).run(suite)
    suite = unittest.TestLoader().loadTestsFromTestCase(GroupTest)
    unittest.TextTestRunner(verbosity=2).run(suite)
    suite = unittest.TestLoader().loadTestsFromTestCase(EnvTest)
    unittest.TextTestRunner(verbosity=2).run(suite)

    suite = unittest.TestLoader().loadTestsFromTestCase(COMPSTest)
    unittest.TextTestRunner(verbosity=2).run(suite)
    suite = unittest.TestLoader().loadTestsFromTestCase(CategoryListTest)
    unittest.TextTestRunner(verbosity=2).run(suite)
    suite = unittest.TestLoader().loadTestsFromTestCase(GroupListTest)
    unittest.TextTestRunner(verbosity=2).run(suite)
    suite = unittest.TestLoader().loadTestsFromTestCase(EnvListTest)
    unittest.TextTestRunner(verbosity=2).run(suite)



