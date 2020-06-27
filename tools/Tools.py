# -*- coding:utf-8 -*-

import os
import sys
import math

class Color4f:
    def __init__(self, r, g, b, a):
        self.r = r
        self.g = g
        self.b = b
        self.a = a   
    def __str__(self):
        return "{{{},{}, {}, {}}}".format(self.r, self.g, self.b, self.a)

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    def __str__(self):
        return "{{{},{}}}".format(self.x, self.y)

class V2F_C4B_T2F:
    def __init__(self, vers , colors, texs):
        self.vers =vars
        self.colors = colors
        self.texs = texs
    def __str__(self):
        pass

def createCircleVertices(filenam , center, radius, angle, segments, scalex, scaley, color, boder, bodercolor):
    vers = []
    count = 4

    coef = 2.0 * math.pi / segments
    for i in range(segments):
        rads = i * coef
        j = radius * math.cos(rads + angle) * scalex + center.x
        k = radius * map.sin(rads + angle) * scaley +  center.y
        vers[i].x = j
        vers[i].y = j
    outline = boder > 0 and bodercolor.a > 0
    vertex_count = 0
    if outline :
        vertex_count = 3 * (3 * count -2)
    else:
        vertex_count =  3 * (count -2)
    
    for i in range(count - 2):
        tmp = {
            {vers[0], },
            {vers[i+1], },
            {vers[i+2], }
        }
        

if __name__ == "__main__":
    if len(sys.argv) > 1:
        print(sys.argv[1])
    p = Point(1, 2.0)
    print(p.__str__())
