import argparse
import controller
from myutils import struct
import sys
    
def parse_args():
    parser = argparse.ArgumentParser(
        description='''
        Virtual peripherals for learning programming of embedded systems.
        Connect to an embedded target via a usb/serial port. The target must 
        have "plib" installed.
        ''')
    parser.add_argument('-l', '--list', action='store_true', help='show list \
    of serial interfaces and exit')
    parser.add_argument('-p', '--port', help='use this serial port - can be omitted,\
    if only one port exists')
    parser.add_argument('-b', '--baud', default='9600', type=int, 
    choices=[9600, 19200, 38400, 115200], help='defaults to 9600')
    parser.add_argument('-v', '--verbose', action='count', default=0, help='\
    repeat to be more verbose')
    parser.add_argument('-d', '--debug', action='store_true', help='show debug window')
    parser.add_argument('-t', '--test', default='-1', type=int, 
            choices=[0,1,2,3,4,5,6,7,8,9], help='test a specific screen (development only)')
    parser.add_argument('-m', '--messages', default=0, type=int, 
            help='display a dummy message in the debug window every n iterations (development only)')
    parser.add_argument('--mode', default='system', choices=['system','light','dark'], help='set color mode')         

    args = parser.parse_args()
    if args.verbose >= 3: print('arguments from command line are >{}< (stage 0)'.format(args))
    if args.list or args.port == None:
        # get list of serial interfaces
        import serial.tools.list_ports
        ports = serial.tools.list_ports.comports()
        _p = []
        for port, desc, hwid in sorted(ports):
            if args.list:
                print("{}: {} [{}]".format(port, desc, hwid))
            _p.append(port)
        if args.list:
            if len(_p) == 0: print('no serial ports found')
            sys.exit(0)
        if len(_p) == 1: 
            args.port = _p[0]
            if args.verbose >= 1: print('port >{}< is used'.format(args.port))
        else:
            if args.verbose >= 1: print('no port given, entering dry run mode')
    if args.verbose >= 3: print('arguments from command line are >{}< (stage 1)'.format(args))
    return args
    
if __name__ == '__main__':
    args = parse_args()
    config = struct()
    config.serial_port = args.port
    config.serial_baud = args.baud
    config.verbose = args.verbose
    config.debug = args.debug
    config.test = args.test
    config.appearance_mode = args.mode
    if args.messages < 0:
        print(f'Invalid argument to -m:{args.messages}, must be a positive integer')
        exit(1)
    elif args.messages > 0:
        config.debug = True
    config.messages = args.messages
    c = controller.Controller(config)
    c.run()