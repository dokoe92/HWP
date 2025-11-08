import tkinter as tk
from tkinter import ttk
import datetime
from myutils import *

class View:
    def __init__(self):
        self.controller=None
        
        self.log_sync_state = True
        self.debug_sync_state = True

    def set_controller(self, controller):
        self.controller=controller
        

    def set_screen(self, id, screen):
        if id == self.controller.screen_id: return 
        if self.controller.screen_id != 0: # remove current screen
            self.app_frame.grid_forget()
            del self.screen
        
        self.controller.screen_id=id
        self.screen=screen
        if id > 0:
            f=self.screen.create_frame(self.f_screens)
            f.grid(row=0,column=0)
            self.app_frame=f
            if self.controller.config.test == id:
                self.controller.screens[id].screen.test_init(self.controller._test)
                   
    def create_frame(self, content, zero_view):
        self.parent=content      
        content.columnconfigure(1, weight=1)
        content.rowconfigure(0, weight=1)   # enables text-widgets at right side to stretch south
     
        f_left=tk.ttk.Frame(content)
        f_left.grid(row=0, column=0, padx=(10,10), sticky='n')
        f_screens=tk.ttk.Frame(f_left)
        f_screens.grid(row=0, column=0, padx=0, pady=0)
        self.f_screens=f_screens
        
        f_right = ttk.PanedWindow(content, orient=tk.VERTICAL)
        f_right.grid(row=0, column=1, padx=(10,10), sticky='nsew')
        
        screen0 = zero_view.create_frame(f_screens)
        screen0.grid(row=1, column=0, pady=5, sticky='n')
        
        f=self.create_text_frame(content, 
                text='Log', 
                bg='lightblue', 
                cmd_sync=self.log_sync_toggle,
                cmd_clear=self.log_clear
                )
        self.log_text=f.text
        self.log_sync=f.sync
        self.log_clear=f.clear
        f_right.add(f) 
        
        if self.controller.config.debug:
            f=self.create_text_frame(content,
                    text='Debug', 
                    bg='lightyellow', 
                    cmd_sync=self.debug_sync_toggle,
                    cmd_clear=self.debug_clear
                    )
            self.debug_text=f.text
            self.debug_sync=f.sync
            self.debug_clear=f.clear
            f_right.add(f)
        content.minsize(350, 300) 
        return

    def create_text_frame(self, parent, text='dummy', bg='red', bd=2, cmd_sync=None, cmd_clear=None):
        f=HeaderFrame(parent, text, padx=0)
        f.grid_rowconfigure(1, weight=1)
        f.grid_columnconfigure(2, weight=1)
        t=tk.Text(f, width=63, wrap='word', font=('Consolas', 10), height=10)
        sb=tk.ttk.Scrollbar(f, command=t.yview)
        t['yscrollcommand'] = sb.set
        t.grid(row=1, column=0, columnspan=3, sticky='nsew')
        sb.grid(row=1, column=3, sticky='ns')
 
        fb_sync=tk.ttk.Button(f, text='Unsync') 
        if cmd_sync:
            fb_sync.configure(command=cmd_sync)
        fb_sync.grid(row=2, column=0)
        fb_clear=tk.ttk.Button(f, text='Clear') 
        if cmd_clear:
            fb_clear.configure(command=cmd_clear)
        fb_clear.grid(row=2, column=1)
        f.text=t
        f.sync=fb_sync
        f.clear=fb_clear
        return f
         
        
    def log_sync_toggle(self):
        if self.log_sync_state:
            self.log_sync_state = False
            self.log_sync.configure(text='Sync')
        else:
            self.log_sync_state = True
            self.log_sync.configure(text='Unsync')
            self.log_text.see(tk.END)
            
    def log_clear(self):
        self.log_text.delete(1.0, tk.END)
        
    def debug_sync_toggle(self):
        if self.debug_sync_state:
            self.debug_sync_state = False
            self.debug_sync.configure(text='Sync')
        else:
            self.debug_sync_state = True
            self.debug_sync.configure(text='Unsync')
            self.debug_text.see(tk.END)
            
    def debug_clear(self):
        self.debug_text.delete(1.0, tk.END)
        
    def handle_log(self, message):
        now = datetime.datetime.now()
        now_string = now.strftime("%H:%M:%S ")
        self.log_text.insert(tk.END, now_string + message + '\n')
        if self.log_sync_state: self.log_text.see(tk.END)

    def handle_debug(self, message):
        if self.controller.config.debug:
            now = datetime.datetime.now()
            now_string = now.strftime("%H:%M:%S ")
            # todo: check for sync
            self.debug_text.insert(tk.END, now_string + message + '\n')
            if self.debug_sync_state: self.debug_text.see(tk.END)
