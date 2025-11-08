from myutils import *
import tkinter as tk
import sv_ttk
import darkdetect

class _View():
    def __init__(self):
        self.controller=None
        self.log_sync_state=True
        self.debug_sync_state=True
        self.led_color_off='darkgreen'
        self.led_color_on='lightgreen'
        self.state=dotdict()
        self.nr={'led': 8, 'switch': 8, 'button': 4}
        for i in ('led', 'switch'):
            nr=self.nr[i]
            a=[]
            for j in range(nr):
                a.append(tk.BooleanVar())
            self.state[i] = a
        a=[]
        for i in range(self.nr['button']):
            a.append(False)
        self.state.button=a
        self.keys_to_monitor={}
        
    def set_controller(self, controller):
        self.controller=controller
        theme=darkdetect.theme()
        if self.controller.main_controller.config.appearance_mode != "system":
            theme=self.controller.main_controller.config.appearance_mode
        theme=theme.lower()
        self.theme=theme
        self.appearance_code = 1 if theme == 'dark' else 0

    def create_frame(self, parent, text='Controls'):
        sv_ttk.set_theme(self.theme)
        wLED=31
       
        frame = HeaderFrame(parent, text=text) 
        frame.grid_columnconfigure((0,10), weight=1)
        row=1
    
        tk.ttk.Label(frame, text='LEDs').grid(row=row+2, column=1)
        tk.ttk.Label(frame, text='Switches').grid(row=row+3, column=1, padx=(0,10))
        tk.ttk.Label(frame, text='Buttons').grid(row=row+4, column=1)
        self.leds=[]
        
        #LEDs and Switches
        for i in range(len(self.state['led'])):
            tk.ttk.Label(frame, text=str(i)).grid(row=row, column=7-i+2)

            cv = tk.Canvas(frame, height=wLED, width=wLED, bd=0) 
            c=cv.create_oval(3,3,wLED-3,wLED-3, width=2, fill=self.led_color_off)
            cv.grid(row=row+2,column=i+2, sticky='ew')
            self.leds.append((cv,c))

            s = tk.ttk.Checkbutton(frame, text='', width=0, 
                    variable=self.state['switch'][i], command=self.switch)
            s.grid(row=row+3,column=7-i+2)
            
        self.leds.reverse()
        row=row+4

        # Buttons
        for i in range(self.nr['button']):
            c = tk.ttk.Button(frame, text=f'{chr(ord("A")+3-i)} ({4-i})', width=0,
                    command=self.button)
            c.grid(row=row, column=8-i*2, columnspan=2, sticky='ew', padx=4, pady=4)
            self.keys_to_monitor[chr(4-i+ord('0'))]=(i,c)
        row=row+1

        #ADC sliders
        nr_scales=2
        commands=[]
        commands.append(self.scale0)
        commands.append(self.scale1)
        if self.controller.main_controller.config.verbose >= 1:
            nr_scales+=1
            commands.append(self.scale2)
        for i in range(nr_scales):
            tk.ttk.Label(frame, text='ADC '+str(i)).grid(row=row, column=1)
            tk.ttk.Scale(frame, 
                    length=250,
                    command=commands[i], 
                    orient=tk.HORIZONTAL, 
                    from_=0, 
                    to=1023,
                    value=511, 
                    ).grid(row=row, column=2, columnspan=8)
            row = row + 1
        if self.controller.main_controller.config.verbose >= 1:
            for i in range(11):
                tk.ttk.Label(frame, text=str(i)).grid(row=row, column=i)
        for i in self.keys_to_monitor:
            frame.bind_all(f'<KeyPress-{i}>', self.on_keypress)
            frame.bind_all(f'<KeyRelease-{i}>', self.on_keyrelease)
        return frame
        
    def on_keypress(self, event):
        if event.char not in self.keys_to_monitor:
            return
        char=event.char
        (nr, widget)=self.keys_to_monitor[char]
        if self.state.button[nr]:
            # supress autorepeat of keyboard
            return
        self.state.button[nr]=True
        widget.state(['pressed'])
        self.button()
        
    def on_keyrelease(self, event):
        if event.char not in self.keys_to_monitor:
            return
        char=event.char
        (nr, widget)=self.keys_to_monitor[char]
        self.state.button[nr]=False
        widget.state(['!pressed'])
        self.button()
        
    def scale0(self, val):
        self.scale(0, val)
        
    def scale1(self, val):
        self.scale(1, val)
        
    def scale2(self, val):
        self.scale(2, val)
        
    def scale(self, id, value):
        if self.controller != None:
            self.controller.scale(id, int(round(float(value))))
            
    def switch(self):
        a = self.state['switch']
        l=len(a)
        value=0
        for i in range(l):
            value*=2
            if a[l-1-i].get(): value+=1
        if self.controller != None:
            self.controller.switch(value)
        
    def button(self):
        a = self.state.button
        l=len(a)
        value=0
        for i in range(l-1,-1,-1):
            value*=2
            key=chr(ord('0')+(4-i))
            widget=self.keys_to_monitor[key][1]
            if widget.instate(['pressed']): 
                value+=1
        if self.controller != None:
            self.controller.button(value)

    def set_led(self, value):
        l = self.state['led']
        for i in range(len(l)):
            onoff=value & (1<<i)
            l[i].set(onoff)
            color='lightgreen' if onoff else 'darkgreen'
            led=self.leds[i]
            led[0].itemconfigure(led[1], fill=color)
            
class _Controller():
    def __init__(self, id, main_controller, model, view):
        self.id=id
        self.main_controller=main_controller
        self.model=model
        self.view=view

    def set_led(self, value):
        self.model.led=value
        self.view.set_led(value)
        
    def scale(self, channel, value):
        id = chr(ord('a') + channel)
        self.model.scale[channel]=value
        self.main_controller.outgoing(f'd{self.id}{id}{value:04x}')
               
    def switch(self, value):
        self.model.switch=value
        self.main_controller.outgoing(f'd{self.id}1{value:02x}')

    def button(self, value):
        self.model.button=value
        message=f'd{self.id}2{value:02x}'
        self.main_controller.outgoing(message)

        
class _Model():
    def __init__(self):
        self.button=0
        self.switch=0
        self.scale=[0,0,0]
        self.led=0
        
class ModuleScreen(Screen):
    def __init__(self, id, main_controller):
        super().__init__(id, main_controller)
        self.view=_View()
        self.model=_Model()
        self.controller=_Controller(id, main_controller, self.model, self.view)
        self.view.set_controller(self.controller)

    def incoming_setter(self, data, position):
        item=data[position]
        if item == '0':
            # decode value:
            try:
                x = int(data[position+1:], 16)
                # pass to view:
                self.controller.set_led(x)
            except ValueError:
                pass
        else: 
            self.send(f'e02{data}:{position}')
        
    def incoming_requester(self, data, position):
        item=data[position]
        if item == '1':
            self.controller.switch(self.model.switch)
        elif item == '2':
            self.controller.button(self.model.button)
        elif item in 'abc':
            channel = ord(item) - ord('a')
            self.controller.scale(channel, self.model.scale[channel])
        else:
            raise ValueError(f'unknown item in {data} at position {position}')
        return None 
    
    def test_init(self, test_config):
        return 1

    def test_do(self, call_counter, test_config):
        value=call_counter & 0xff
        self.controller.set_led(value)

def create_dummy_frame(parent, text='dummy'):
        frame = tk.LabelFrame(parent, text=text, bd=2)
        l = tk.Label(frame, text='Label for ' + text)
        l.grid(row=0, column=0)
        return frame

