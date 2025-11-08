import glob
from pathlib import Path
from abc import ABC, abstractmethod
import tkinter as tk

class struct: # use this as a struct
    pass

class dotdict(dict):
    """dot.notation access to dictionary attributes"""
    __getattr__ = dict.get
    __setattr__ = dict.__setitem__
    __delattr__ = dict.__delitem__

class HeaderFrame(tk.ttk.Labelframe):
    def __init__(self, parent, text='no title given', padx=0, pady=0):
        super().__init__(parent, text=text)

class Screen(ABC):
    def __init__(self, id, main_controller):
        '''
        Base class for screens.
        @param id:  The id number of the screen. We do not retrieve this from the filename
            of the screen, but let it be set by the main_controller.
        @param main_controller: the one to report data to
        '''
        self.id=id
        self.main_controller=main_controller
    
    def create_frame(self, parent):
        '''
        Create the frame for this screen.
        @param parent: The parent widget of this frame
        '''
        self.parent = parent
        
    def send(self, message):
        self.main_controller.outgoing(message)

    @abstractmethod
    def incoming_setter(self, data, position):
        '''
        Method to receive data from the MCU.
        @param data: Data received from the MCU
        '''
        pass
    
    def incoming_requester(self, data, position):
        '''
        Method to receive requests from the MCU.
        @param data: Data received from the MCU
        '''
        return None 

    def test_init(self, test_config=None):
        '''
        Test method to initialize test of this screen.
        @param test_config: Configuration data for testing
        @return: divider for calls of test_do
        '''
        return 1

    def test_do(self, call_counter, test_config=None):
        '''
        Test method, called every iteration of our internal main loop.
        @param test_config: Configuration data of the test
        '''
        pass

def module_for_screen(screen_id):
    name=f'screen{screen_id}*.py'
    files=glob.glob(name)
    if len(files) != 1:
        raise ValueError(f'No module found for screen {screen_id}')
    module_name=str(Path(files[0]).with_suffix(''))
    return __import__(module_name)