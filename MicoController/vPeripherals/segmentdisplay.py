# segment display implementation based on: https://www.python-forum.de/viewtopic.php?t=2462
from tkinter import *
from math import *
from copy import *

debug = False

class Segment:
    stipple = 'gray12'

    def place_segments(self):
        for segnum,segkey in enumerate(self.segmentkeys):
            self.xpos,self.ypos = self.coordsegment[segnum]
            self.points = deepcopy(self.coordpoints[segkey])
            self.draw_segment(segkey)
        self.set_mask(0)

    def draw_segment(self,segkey):
        self.polypoints = []
        for segment in self.coordpoints[segkey]:
            self.points = deepcopy(segment)
            for x,y,scale in [self.points]:
                if scale == 1:
                    x += self.xpos
                    y += self.ypos

                if scale == 2:
                    x *= self.factor
                    x += self.xpos
                    y += self.ypos

                if scale == 3:
                    x *= self.factor
                    x += self.xpos
                    y *= self.factor
                    y += self.ypos

                if scale == 4:
                    x += self.xpos
                    y *= self.factor
                    y += self.ypos

                if scale == 5:
                    x *= self.factor
                    x += self.xpos
                    y += self.ypos

                # store this scaled point
                self.points[0] = x
                self.points[1] = y

                # build polygon list
                self.polypoints += [self.points[0:2]]

        self.segmentobj.append(self.canvas.create_polygon(
            self.polypoints,
            fill=self.fill,
            outline=self.outline,
            stipple='gray12'
            ))

    def set_mask(self, mask):
        if mask == self.mask:
            return
        changed = mask ^ self.mask
        self.mask = mask
        for segnum in range(len(self.segmentobj)):
            if changed & 1:
                if mask & 1:
                    self.canvas.itemconfigure(self.segmentobj[segnum],fill=self.fill,stipple=self.stipple)
                else:
                    self.canvas.itemconfigure(self.segmentobj[segnum],fill=self.fill,stipple='gray12')
            mask >>= 1
            changed >>= 1

    def update_segment_color(self):
        mask = self.mask
        for segnum,segflag in enumerate(mask):
            if segflag:
                self.canvas.itemconfigure(self.segmentobj[segnum],fill=self.fill,stipple=self.stipple)
            else:
                self.canvas.itemconfigure(self.segmentobj[segnum],fill=self.fill,stipple='gray12')


class Digit(Segment):
    # A single 7-segment display.
    
    gap = 2
    
    @classmethod
    def size(cls, factor=10):
        # return the size of the widget for a given factor
        return factor*10 + 2*cls.gap, factor*20 + 4*cls.gap
        
    def __init__(self, canvas, xpos=0.0,ypos=0.0,factor=10.0,color='red',stipple=''):
        self.canvas = canvas    
        # Buffer for Polypoints
        self.segmentobj    =  []
        self.points        =  None
        self.polypoints    =  []
        self.coordsegment  =  []
        self.coordpoints   =  {}
        self.segmentkeys   =  ('A','B','C','D','E','F','G')

        self.xp       = xpos
        self.yp       = ypos
        self.fill     = color
        self.stipple  = stipple
        self.outline  = self.canvas['bg']
        self.gap      = Digit.gap
        self.factor   = factor
        self.mask   = 0
    
        # starting coorinates for all the segments
        # segment A
        self.xpos    = self.xp + self.gap
        self.ypos    = self.yp
        self.coordsegment =  [[self.xpos,self.ypos]]

        # segment B
        self.xpos    = self.xp + (self.factor * 10.0)-(self.factor * 2.0)+ self.gap*2
        self.ypos    = self.yp + self.gap
        self.coordsegment += [[self.xpos,self.ypos]]

        # segment C
        self.xpos    = self.xp + (self.factor * 10.0)-(self.factor * 2.0)+self.gap*2
        self.ypos    = self.yp + (self.factor * 10.0)+(self.factor * 0.0) + self.gap*3
        self.coordsegment += [[self.xpos,self.ypos]]

        # segment D
        self.xpos    = self.xp + self.gap
        self.ypos    = self.yp + (self.factor * 20.0)-(self.factor * 2.0) + self.gap*4
        self.coordsegment += [[self.xpos,self.ypos]]

        # segment E
        self.xpos    = self.xp
        self.ypos    = self.yp + (self.factor * 10.0)+ self.gap*3
        self.coordsegment += [[self.xpos,self.ypos]]

        # segment F
        self.xpos    = self.xp
        self.ypos    = self.yp + self.gap
        self.coordsegment += [[self.xpos,self.ypos]]

        # segment G
        self.xpos    = self.xp + self.gap
        self.ypos    = self.yp + (self.factor * 10.0)-(self.factor * 1.0) + self.gap*2
        self.coordsegment += [[self.xpos,self.ypos]]

        # segment patterns for numbers 0-9
        self.segmentmask  =  [0b0111111,  #  [1,1,1,1,1,1,0] 
                              0b0000110,  #  [0,1,1,0,0,0,0] 
                              0b1011011,  #  [1,1,0,1,1,0,1] 
                              0b1001111,  #  [1,1,1,1,0,0,1] 
                              0b1100110,  #  [0,1,1,0,0,1,1] 
                              0b1101101,  #  [1,0,1,1,0,1,1] 
                              0b1111101,  #  [1,0,1,1,1,1,1] 
                              0b0000111,  #  [1,1,1,0,0,0,0] 
                              0b1111111,  #  [1,1,1,1,1,1,1] 
                              0b1101111,  #  [1,1,1,1,0,1,1] 
                             ]

        # polygons for segments A-G
        self.coordpoints  =  {'A': [[0.0 ,0.0 ,1],  # Polygon-Eckpunkte Segment-A
                                    [10.0,0.0 ,2],
                                    [8.0 ,2.0 ,3],
                                    [2.0 ,2.0 ,3],
                                    ],
                              'B': [[0.0 ,2.0 ,4],  # Polygon-Eckpunkte Segment-B
                                    [2.0 ,0.0 ,5],
                                    [2.0 ,10.0,3],
                                    [0.0 ,9.0 ,4],
                                    ],
                              'C': [[0.0 ,1.0 ,4],  # Polygon-Eckpunkte Segment-C
                                    [2.0 ,0.0 ,5],
                                    [2.0 ,10.0,3],
                                    [0.0 ,8.0 ,4],
                                    ],
                              'D': [[2.0 ,0.0 ,5],  # Polygon-Eckpunkte Segment-D
                                    [8.0 ,0.0 ,5],
                                    [10.0,2.0 ,3],
                                    [0.0 ,2.0 ,4],
                                    ],
                              'E': [[0.0 ,0.0 ,1],  # Polygon-Eckpunkte Segment-E
                                    [0.0 ,10.0,4],
                                    [2.0 ,8.0 ,3],
                                    [2.0 ,1.0 ,3],
                                    ],
                              'F': [[0.0 ,0.0 ,1],  # Polygon-Eckpunkte Segment-F
                                    [0.0 ,10.0,4],
                                    [2.0 ,9.0 ,3],
                                    [2.0 ,2.0 ,3],
                                    ],
                              'G': [[0.0 ,1.0 ,4],  # Polygon-Eckpunkte Segment-G
                                    [2.0 ,0.0 ,2],
                                    [8.0 ,0.0 ,2],
                                    [10.0,1.0 ,3],
                                    [8.0 ,2.0 ,3],
                                    [2.0 ,2.0 ,3],
                                    ]
                             }
        self.place_segments()
                
    def set_number(self, number):
        new_mask = self.segmentmask[number]
        self.set_mask(new_mask)


class DigitArray:
    # An array of Digits in one horizontal row.
    
    gap = 2
    
    @classmethod
    def size(cls, factor=10, content='dd'):
        x = 0
        dix, diy = Digit.size(factor)
        ddx, ddy = DoubleDot.size(factor)
        
        for i in content:
            if i == 'd':    
                x += dix + factor * cls.gap
            elif i == ':':
                x += ddx + factor * cls.gap
            else:
                raise Exception(f'Invalid character "{i}" in content string "{content}", must be "d" or ":".') 
        x -= factor * cls.gap
        return x,diy
            
    def __init__(self, canvas, content, xpos=0.0, ypos=0.0, factor=10.0, color='red', stipple=''):
        self.digits = []
        self.dots = []
        
        dix, diy = Digit.size(factor)
        ddx, ddy = DoubleDot.size(factor)
        
        for i in content:
            if i == 'd':    
                self.digits.append(Digit(canvas, xpos, ypos, factor, color, stipple))
                xpos += dix + factor*self.gap
            elif i == ':':
                self.dots.append(DoubleDot(canvas, xpos, ypos, factor, color, stipple))
                xpos += ddx + factor * self.gap
            else:
                raise Exception(f'Invalid character "{i}" in content string "{content}", must be "d" or ":".') 
        self.digits.reverse()
        self.dots.reverse()
        
    def set_numbers(self, numbers):
        # assume numbers is a list
        L = min(len(numbers), len(self.digits))
        for i in range(L):
            self.digits[i].set_number(numbers[i])
    
    def set_masks(self, masks):
        # assume masks is a list
        L = min(len(masks), len(self.digits))
        for i in range(L):
            self.digits[i].set_mask(masks[i])
            
    def set_dots(self, dots):
        # assume dots is a list
        L = min(len(dots), len(self.dots))
        for i in range(L):
            self.dots[i].set(dots[i])
    
    def set_number(self, number, leading_zero=False):
        # write number to our DigitArray
        base = 1
        for i in range(len(self.digits)):
            v = int(number/base) % 10
            if v or leading_zero or number >= pow(10, i):
                self.digits[i].set_number(v)
            else:
                self.digits[i].set_mask(0)
            base *= 10


class RoundedDot(Segment):
    # A single dot.
    
    stipple = 'gray12'
    s = 2.414   # size of a dot 
    
    @classmethod
    def size(cls, factor=10):
        # return the size of the widget for a given factor
        return factor * cls.s, factor * cls.s

    def __init__(self, canvas, xpos=0.0, ypos=0.0, factor=10.0, color='red', stipple=''):
        # note that for convenience we use the same ypos as with the corresponding Digits
    
        self.canvas = canvas    
        # Buffer for Polypoints
        self.segmentobj    =  []
        self.points        =  None
        self.polypoints    =  []
        self.coordsegment  =  []
        self.coordpoints   =  {}
        self.segmentkeys   =  ('d')

        self.xp       = xpos
        self.yp       = ypos
        self.fill     = color
        self.stipple  = stipple
        self.outline  = self.canvas['bg']
        self.factor   = factor
        self.mask   = 0
    
        # starting coorinates for all the segments
        # segment d
        self.xpos    = self.xp
        self.ypos    = self.yp 
        self.coordsegment =  [[self.xpos,self.ypos]]


        # segment patterns for numbers 0-9
        self.segmentmask  =  0b1
        
        # polygons for segment d
        self.coordpoints  =  {'d': [[0.707,    0.0 ,   2],      # polygon points Segment d
                                    [1.707, 0.0,    2],
                                    [2.414, 0.707, 3],
                                    [2.414, 1.707, 3],
                                    [1.707, 2.414, 3],
                                    [0.707, 2.414, 3],
                                    [0, 1.707, 4],
                                    [0, 0.707, 4],
                                    ],
                             }
        self.place_segments()

    def set(self, onoff):
        if onoff:
            self.set_mask(1)
        else:
            self.set_mask(0)                            
        
        
class DoubleDot(Segment):
    # The two vertical dots that usually separate hours from minutes on the display.
    
    stipple = 'gray12'
    s = 3   # size of a dot
    h = 3   # distance of dot from middle line = half the distance between the two dots
    
    @classmethod
    def size(cls, factor=10):
        # return the size of the widget for a given factor
        return factor * cls.s, factor * cls.s

    def __init__(self, canvas, xpos=0.0, ypos=0.0, factor=10.0, color='red', stipple=''):
        # note that for convenience we use the same ypos as with the corresponding Digits
    
        self.canvas = canvas    
        # Buffer for Polypoints
        self.segmentobj    =  []
        self.points        =  None
        self.polypoints    =  []
        self.coordsegment  =  []
        self.coordpoints   =  {}
        self.segmentkeys   =  ('N','S')

        self.xp       = xpos
        self.yp       = ypos
        self.fill     = color
        self.stipple  = stipple
        self.outline  = self.canvas['bg']
        self.factor   = factor
        self.mask   = 0
    
        # starting coorinates for all the segments
        # segment N
        self.xpos    = self.xp
        self.ypos    = self.yp + self.factor * (10-self.h-self.s)
        self.coordsegment =  [[self.xpos,self.ypos]]

        # segment S
        self.xpos    = self.xp
        self.ypos    = self.yp + self.factor * (10+self.h)
        self.coordsegment += [[self.xpos,self.ypos]]

        # segment patterns for numbers 0-9
        self.segmentmask  =  0b11
        
        # polygons for segments N, S
        self.coordpoints  =  {'N': [[0.0,    0.0 ,   1],      # polygon points Segment N
                                    [self.s, 0.0,    2],
                                    [self.s, self.s, 3],
                                    [0,      self.s, 4],
                                    ],
                              'S': [[0.0,    0.0 ,   1],      # polygon points Segment S
                                    [self.s, 0.0,    2],
                                    [self.s, self.s, 3],
                                    [0,      self.s, 4],
                                    ],
                             }
        self.place_segments()

    def set(self, onoff):
        if onoff:
            self.set_mask(3)
        else:
            self.set_mask(0)                            

