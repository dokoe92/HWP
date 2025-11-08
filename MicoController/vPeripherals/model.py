from myutils import struct

class Model:
    def __init__(self):
        self.state = struct()
               
    def handle_log(self, data):
        self.view.handle_log(data)
        
    def handle_debug(self, data):
        self.view.handle_debug(data)
        
 