"""
Canvas.py
Will store the Canvas class required for Synfig canvases
"""

import sys
import settings
sys.path.append("..")


class Canvas:
    """
    Class to keep Canvas for Synfig
    """
    def __init__(self, canvas):
        """
        """
        self.canvas = canvas
        self.defs = {}
        self.extract_defs(self.defs)
        self.layers = []
        self.extract_layers(self.layers)
        self.set_name()

    def set_name(self):
        """
        Sets the name of the canvas
        """
        found = False
        for child in self.canvas:
            if child.tag == "name":
                self.name = child.text
                found = True
        if not found:
            self.name = settings.CANVAS_NAME + str(settings.canvas_count.inc())

    def get_canvas(self):
        """
        This won't be required once the canvas class is fully embedded in the
        code
        Returns the address of original canvas
        """
        return self.canvas

    def extract_defs(self, defs_dict):
        """
        Will store all the defs with there id's as there keys
        """
        for child in self.canvas:
            if child.tag == "defs":
                defs = child

                for child in defs:
                    key = child.attrib["id"]
                    defs_dict[key] = child

    def get_def(self, key):
        """
        Given a key, returns the corresponding child definded in the canvas
        """
        if key in self.defs.keys():
            return self.defs[key]
        return None
        #raise KeyError("No defs %s in Canvas %s", key, self.name)

    def extract_layers(self, layer_list):
        """
        Will store all the layers in a list
        """
        for child in self.canvas:
            if child.tag == "layer":
                layer_list.append(child)

    def __getitem__(self, itr):
        """
        Returns the layer at itr
        """
        return self.layers[itr]

    def get_num_layers(self):
        """
        Returns the number of layers in the canvas
        """
        return len(self.layers)
