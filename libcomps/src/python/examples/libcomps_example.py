import libcomps

if __name__ == "__main__":
    comps = libcomps.Comps()
    categories_ids = ["cat1", "cat2", "cat3"]
    categories_names = ["category1", "category2", "category3"]
    categories_descriptions = ["category description1", "category_description2",
                               "category description3"]
    categories_group_ids = [["g1", "g2"], ["g1"], ["g2","g3"]]
    for x in range(0,3):
        c = libcomps.Category(categories_ids[x], categories_names[x],
                                categories_descriptions[x])
        for y in categories_group_ids[x]:
            c.group_ids.append(y)
        comps.categories.append(c)
    print (comps.xml_str())
