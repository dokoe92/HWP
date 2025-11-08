import sys
import glob
from pathlib import Path
import tkinter as tk
import sv_ttk
from model import Model
import serial
import datetime
from view import View
from myutils import struct, dotdict

class Controller:
    def __init__(self, config):
        self.root = tk.Tk()
        self.config = config
        self.model = Model()
        self.view = View()
        self.view.set_controller(self)
        self.screens=self.load_screens()
        if not 0 in self.screens:
            raise Exception('Screen 0 could not be loaded')
        self.first=True
        self.setters={
                'L': self.incoming_log_setter,
                'S': self.incoming_screen_setter,
                'D': self.incoming_debug_setter,
                }
        self.requesters={
                'T': self.incoming_time_requester,
                'V': self.incoming_version_requester,
                }
        if self.config.serial_port != None:
            self.serial_interface = serial.Serial(
                    self.config.serial_port, 
                    self.config.serial_baud, 
                    timeout=0, 
                    writeTimeout=0) #ensure non-blocking
            self.serial_read_buffer = ''
            
        
    def run(self):
        # instantiate the screens
        for id in self.screens.keys(): 
            entry=self.screens[id]
            entry.screen=entry.module.ModuleScreen(id, self)
        self.screen_id=0
        self.view.create_frame(self.root, self.screens[0].screen.view)
        self.prepare_test()
        
        title = 'vPeripherals '
        if self.config.serial_port == None:
            title += 'dry run (no serial interface)'
        else:
            title += '{}@{}'.format(self.config.serial_port, str(self.config.serial_baud))
        self.root.title(title)
        self.root.deiconify()
        
        self.root.after(100, self.do) # run once, then periodically
        self.root.mainloop()

    def prepare_test(self):  
        if self.config.messages or self.config.test >= 0:
            '''
            activated by -m option from the command line
            @init we have to prepare the log file
            '''
            self._test = struct()
            t = self._test
            t.call_counter=0

        if self.config.test >= 0:
            '''
            activated by -t option from the command line
            @init we have to prepare the switch to a special screen
            and for a divider which does test calls to the special 
            screen every n calls to Model.do()
            '''
            t.call_divided_counter=0
            t.call_divider=self.screens[self.config.test].screen.test_init(t)          
            # activate a special screen
            if self.config.test:    
                self.root.after(500, self.view.set_screen(self.config.test, self.screens[self.config.test].screen))

    def debug_message(self, message):
        # just a helper for local debug messages
        self.view.handle_debug(f'[local] {message}')

    def do(self):
        if self.config.serial_port != None: 
            self.serial_read()
        if self.first:
            self.first = False
            self.outgoing('?S')    # query MCU for current screen     
        if self.config.test >= 0 or self.config.messages:
            t=self._test
            t.call_counter += 1
            if self.config.messages and (t.call_counter % self.config.messages == 0):
                nr=int(t.call_counter/self.config.messages)
                msg=f'message {nr} after {self.config.messages} calls'
                self.debug_message(msg)
            
            if self.config.test >= 0 and self.config.test == self.screen_id:
                if t.call_counter % t.call_divider == 0:
                    t.call_divided_counter+=1
                    self.screens[self.screen_id].screen.test_do(t.call_divided_counter, t)           
        self.root.after(10, self.do)

    def incoming_log_setter(self, message, position):
         self.view.handle_log(message[position:])

    def incoming_debug_setter(self, message, position):
         self.view.handle_debug(message[position:])
         
    def incoming_screen_setter(self, message, position):
        if self.config.verbose >= 2:
            print(f'set screen {message[position:]}')         
        screen_id = ord(message[position]) - ord('0')
        if not screen_id in self.screens.keys():
             raise ValueError(f'Invalid screen ID: {screen_id}')
        s=self.screens[screen_id]
        if s.screen is None:
             s.screen = s.module.ModuleScreen(self.model, self.view, self.config)
        self.view.set_screen(screen_id, s.screen)
     
    def incoming_setter(self, message, position=1):
        # process a setter message 'm' from PC
        c = message[position]
        if c in self.setters.keys():
            self.setters[c](message, position+1)
        elif '0' <= c <= '9':
            # setter to a screen
            id=ord(c) - ord('0')
            if id == 0:
                self.screens[0].screen.incoming_setter(message, position+1)
            elif id == self.screen_id:
                self.screens[self.screen_id].screen.incoming_setter(message, position+1)
            else:
                self.debug_message(f'write to non-active screen {id}: {message}')
        else:                                               # default: report to debug widget
            self.debug_message('unknown item in setter: ' + message)
            
    def incoming_requester(self, message, position=1):
        if self.config.verbose >= 5:
            print(f'requester: {message}, {position}, {self.requesters}')
        # MCU sent a request
        c = message[position]
        if c in self.requesters.keys():
            answer=self.requesters[c](message, position+1)
        elif '0' <= c <= '9':
            # request to a screen
            id=ord(c) - ord('0')
            if id == 0:
                answer = self.screens[0].screen.incoming_requester(message, position+1)
            elif id == self.screen_id:
                answer = self.screens[self.screen_id].screen.incoming_requester(message, position+1)
            else:
                answer = f'e01{message}'
                self.debug_message(f'request to non-active screen {id}: {message}')
        else:                                               # default: report to debug widget
            answer = f'e01{message}'
            self.debug_message('unknown item in requester:' + self.serial_read_buffer)
        if answer is not None:
            self.outgoing(answer)

    def incoming_time_requester(self, message, position):
        now = datetime.datetime.now()
        now_string = now.strftime("%Y%m%d%H%M%S")
        answer = f'dT{now_string}'
        return answer
    
    def incoming_version_requester(self, message, position):
        d = 'd' if self.config.debug else ''
        answer = f'dV03{d}'   # todo: version is currently hard coded
        for id in self.screens.keys():
            answer += chr(id + ord('0'))
        return answer
        
    def serial_read(self):
        while True:
            try:
                c = self.serial_interface.read().decode('utf-8', errors='ignore') 
            except Exception as err:
                print(f'Read from MCU failed: {err}\n-> exiting.')
                sys.exit(1)
                c = ''
            if len(c) == 0:
                break
            if self.config.verbose >= 5:
                print('add >{}< to {}'.format(c, self.serial_read_buffer))

            # check if character is a delimiter
            if c not in '\n\r':
                self.serial_read_buffer+=c
            else:
                #add the line to the log
                if self.config.verbose >= 2:
                    print(f'> {self.serial_read_buffer}') #[:-1]}')
                if len(self.serial_read_buffer) >= 2:    
                    self.incoming(self.serial_read_buffer)
                self.serial_read_buffer = ''

    def incoming(self, message):
        c = message[0]
        # is it a setter?
        if c == 'd':    # set
            self.incoming_setter(message, 1)
        elif c == '?': # request
            self.incoming_requester(message, 1)
        elif c == 'e': # error
            self.debug_message('incoming error message:' + message)
        else:   
            pass # ignore erroneous messages
 
    def load_screens(self):
        mandatory_screens={0: 'screen0', 1: 'screen1alarmclock', 2: 'screen2seesaw'}
        # we search for files named 'screen[0-9]*.py in the current directory
        screens={}
        for screen_id in range(10):
            if screen_id in mandatory_screens.keys():
                module_name=mandatory_screens[screen_id]
            else:
                name=f'screen{screen_id}*.py'
                files=glob.glob(name)
                if len(files) == 0: 
                    continue
                if len(files) > 1:
                    raise ValueError(f'Multiple modules found for screen {screen_id}: {files}')
                module_name=str(Path(files[0]).with_suffix(''))
            screens[screen_id]=dotdict({'module': __import__(module_name), 'screen': None}) 
        if self.config.verbose >= 3:
            print(f'loaded screens: {screens}')
        return screens

    def outgoing(self, message):
        if self.config.verbose >= 2:
            print(f'<\t{message}')
        if self.config.serial_port != None:
            try:
                self.serial_interface.write(message.encode('utf-8'))
                self.serial_interface.write(b'\n')
            except Exception as err:
                print(f'Write to MCU failed: {err}\n-> exiting.')
                sys.exit(1)
  