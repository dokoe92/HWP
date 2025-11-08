import random
from myutils import *
import segmentdisplay
import tkinter as tk

class ModuleScreen(Screen):
    def __init__(self, id, main_controller):
        super().__init__(id, main_controller)

    def create_frame(self, parent):
        super().create_frame(parent)
        frame = HeaderFrame(parent, 'Alarm Clock', padx=0)
        format = 'dd:dd'
        factor = 8
        c_bg = 'black'
        c_fill = 'white'
        y_text = 30
        y_display = y_text + 30       
        d_width, d_height = segmentdisplay.DigitArray.size(factor, format)
        c_width = d_width + 40
        c_height = y_display + d_height + 20
       
        d_x = c_width/2 - d_width/2
        if d_x < 0:
            raise Exception (f'Width of display ({d_width}) does not fit into canvas ({c_width}')
        if self.main_controller.config.verbose >= 4:
            print(f'display {d_width}x{d_height} in canvas {c_width}x{c_height}')
        self.acd_fgcolor=_acd_fgcolor
        x_alarm = c_width/4
        x_beep = c_width*3/4
        c = tk.Canvas(frame, width=c_width, height=c_height, bg=c_bg, bd=0, highlightthickness=0)
        c.grid(row=1, column=0)
        c.create_text(x_alarm, y_text, text='Alarm', fill=c_fill)
        c.create_text(x_beep, y_text, text='Beep', fill=c_fill)
        self.acd = segmentdisplay.DigitArray(
                c, 
                format, 
                xpos=d_x, ypos=y_display, 
                factor=factor, 
                color=self.acd_fgcolor)
        self.acd.alarm = segmentdisplay.RoundedDot(c, xpos=x_alarm-50, ypos=y_text-13, factor=10, color=self.acd_fgcolor)
        self.acd.beep = segmentdisplay.RoundedDot(c, xpos=x_beep-50, ypos=y_text-13, factor=10, color=self.acd_fgcolor)
        return frame

    def show(self, data):
        # we got new data for the alarmclock display
        alarm = beep = False
        b = list(bytes.fromhex(data[0:8]))
        b.reverse()
        dots = []
        for i in range(len(b)):
            dots.append(b[i] & 0x80)
            b[i] &= 0x7f
        alarm = dots[2]
        beep = dots[0]
        self.acd.set_dots([dots[1]])
        self.acd.set_masks(b)
        self.acd.alarm.set(alarm)
        self.acd.beep.set(beep)
    
    def incoming_setter(self, data, position):
        self.show(data[position:])

    def test_init(self, test_config):
        return 1

    def test_do(self, call_counter, test_config):
        self.acd.set_number(call_counter)

random.seed()
_fgcolors = ['red','lightsalmon1','green','blue','steelblue3','yellow','white']
_acd_fgcolor = _fgcolors[random.randrange(len(_fgcolors))]
