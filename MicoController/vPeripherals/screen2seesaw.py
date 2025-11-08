from myutils import *
import tkinter as tk
from coordinates import Coordinate
import math


class _Moveable(ABC):
    def __init__(self, context, position):
        self.context = context
        self.draw(position)

    @abstractmethod
    def _app_position(self, position):
        # Implement this method to convert the position into a app (x, y) coordinate
        pass

    @abstractmethod
    def _draw(self, x, y):
        # Implement this method to draw the object on the canvas
        # x, y are the raw coordinates
        pass
    
    def draw(self, position):
        self.x, self.y = self._app_position(position)
        self._draw(self.x, self.y)

    def move_to(self, position):    
        x,y = self._app_position(position)
        if x != self.x or y != self.y:
            self._move_to(self.context.canvas, self.id, x, y)

    def _move_to(self, canvas, obj_id, x, y):
        current_x, current_y = self.x, self.y

        # Calculate the difference between the current position and the target position
        dx = x - current_x
        dy = -1 * (y - current_y)
        
        # Move the object to the target position
        canvas.move(obj_id, dx, dy)
        self.x, self.y = x, y


class _ReferenceMarker(_Moveable):
    def __init__(self, context, position):
        super().__init__(context, position)

    def _app_position(self, position):
        x = round(position/0.6 * (self.context.pSsSize.x/2 - 1.5*self.context.rSsBall))
        y = self.context.pSsSize.y - 1
        return x,y

    def _draw(self, x, y):
        _abs=[]
        for i in self.context.SsRefRel:
            xr,yr = i.x+x,i.y+y
            xa,ya= self.context.SsBase.to_absolute((xr,yr))
            _abs.append(xa)
            _abs.append(ya)
        self.id = self.context.canvas.create_polygon(_abs, fill=self.context.cSsRef)


class _Ball(_Moveable):
    def __init__(self, context, position):
        super().__init__(context, position)
        
    def _app_position(self, position):
        x = round(position[0]/0.6 * (self.context.pSsSize.x/2 - 1.5*self.context.rSsBall))
        angle = position[1]
        y_mid = self.context.hSsStand
        tan_angle = round(angle * self.context.angle2tan, 2)
        y = -tan_angle * x + y_mid + self.context.rSsBall + round(self.context.wSsRamp/1) 
        return (x, y)
    
    def _draw(self, xa, ya):
        x, y = self.context.SsBase.to_absolute((xa, ya))
        self.id = self.context.canvas.create_oval(
                x-self.context.rSsBall, y-self.context.rSsBall, x+self.context.rSsBall, y+self.context.rSsBall, 
                fill=self.context.cSsBall, outline=self.context.cSsBall)


class _Ramp(_Moveable):
    def __init__(self, context, angle):
        self.id = None
        super().__init__(context, angle)

    def _app_position(self, angle):
        tan_angle = round(angle * self.context.angle2tan, 3)
        y_delta = round(tan_angle * self.context.pSsSize.x/2)
        y_mid = self.context.hSsStand
        return y_mid+y_delta, y_mid-y_delta
    
    def _move_to(self, canvas, id, y_left, y_right):
        self._draw(y_left, y_right)

    def _draw(self, y_left, y_right):
        p1r = (round(-self.context.pSsSize.x/2+2), y_left)
        p2r = (round(+self.context.pSsSize.x/2-1), y_right)  
        p1a = self.context.SsBase.to_absolute(p1r)
        p2a = self.context.SsBase.to_absolute(p2r)
        if self.id is not None:
            self.context.canvas.delete(self.id)
        self.id = self.context.canvas.create_line(p1a, p2a, fill=self.context.cSsRamp, width=self.context.wSsRamp)

    
class _Boing(_Moveable):
    def __init__(self, context, position):
        super().__init__(context, position)

    def _app_position(self, position):
        return position, None

    def _draw(self, x, y):
        c = self.context
        color = c.cSsBoingOn if x else c.cSsBoingOff
        xm, ym = round(-c.pSsSize.x/2 - c.dSsx/2), 10
        xc,yc = c.SsBase.to_absolute((xm, ym))
        font = c.fontBoing
        self.id = c.canvas.create_text(xc, yc, text='Boing!', font=font, fill=color)

    def _move_to(self, canvas, id, x, y):
        color = self.context.cSsBoingOn if x else self.context.cSsBoingOff
        self.x = x
        canvas.itemconfig(id, fill=color)


class _CoordinateRel2Abs:
    # conversion relative coordinates to absolute
    def __init__(self, base, swap_y=True):
        if isinstance(base, tuple) or isinstance(base, list):
            self.x=base[0]
            self.y=base[1]
        elif isinstance(base, Coordinate):
            self.x=base.x
            self.y=base.y
        else:
            raise Exception('need an instance of "Coordinate" or "tuple" or "list" as base')
        self.swap_y=swap_y
       
    def toA(self, rel):
        if not isinstance(rel, Coordinate):
            raise Exception('need an instance of "Coordinate"')
        x=self.x + rel.x
        if self.swap_y:
            y=self.y-rel.y
        else:
            y=self.base.y+rel.y
        result=Coordinate(x,y)
        return result
    

    def to_absolute(self, rel):
        if isinstance(rel, tuple) or isinstance(rel, list):
            xr=rel[0]
            yr=rel[1]
        elif isinstance(rel, Coordinate):
            xr=rel.x
            yr=rel.y
        else:
            raise Exception('need an instance of "Coordinate" or "tuple" or "list"')
        x=self.x + xr
        if self.swap_y:
            y=self.y-yr
        else:
            y=self.y+yr
        return x, y
                            

class ModuleScreen(Screen):
    def __init__(self, id, main_controller):
        super().__init__(id, main_controller)

    def create_frame(self, parent):
        super().create_frame(parent)
        c=struct()
        self.data=c
        dynamics=struct()
        c.dynamics=dynamics
        C=Coordinate
        
        # ------------------- the colors
        c.cFrame='#202020' 
        c.cBG='#404040' # general background of the canvas
        c.cSsBG='#000000' # seesaw's background
        c.cSsRamp='#004de6' # seesaw's ramp
        c.cSsStand='#606060'   # seesaw's stand
        c.cSsBall='#f0f0f0'  # seesaw's ball
        c.cSsRef='#fcf800'  # seesaw's reference marker
        c.cSsBoingOn='#cc0000'  # Boing is on
        c.cSsBoingOff='#606060'
        c.cGraphBG='#000000'     # graph's background
        c.cGraphGrid='#404040'  # graph's grid
        c.cGraphCursor='saddle brown' # vertical moving cursor line
        
        # ------------------- parameterized positions and sizes
        c.pSsSize=Coordinate(420,180)  # size of the seesaw animation area
        c.dBorder=15                  # distance to outer borders
        c.pGraphSize=Coordinate(600, 241)  # size of the graph area, note that y should be odd because of the centerline at y=0
        c.wSsStand=36   # width of the stand
        c.hSsStand=70       # height of the stand
        c.wSsRamp=3    # width of the ramp's line
        c.wSsRef=6                     # width of the reference marker
        c.hSsRef=11                      # height of the reference marker
        c.rSsBall=9                      # radius of the ball
        c.fontBoing='tkDefaultFont 14'  # font for display of boing
        # the reference marker
        c.SsRefRel=[C(-c.wSsRef, 0), C(+c.wSsRef, 0), C(0,-c.hSsRef)]
        c.graph_scale_x=2   # strech in x-axis
        
        # ------------------- calculated positions and sizes
        c.pCanvasSize=Coordinate(max(c.pSsSize.x, c.pGraphSize.x) + 2*c.dBorder, c.pSsSize.y + c.pGraphSize.y + 3*c.dBorder) # the whole canvas
        c.dSsx=int(c.pCanvasSize.x - (c.pSsSize.x + c.dBorder))
        c.dGraphx=int(c.pCanvasSize.x - (c.pGraphSize.x + c.dBorder))
        c.pSsBase=Coordinate(c.dSsx+round(c.pSsSize.x/2), c.dBorder+c.pSsSize.y-1)
        c.SsBase=_CoordinateRel2Abs(c.pSsBase)
        c.pGraphBase=Coordinate(c.dGraphx+1, c.pSsBase.y+c.dBorder+int(c.pGraphSize.y/2)+1)
        c.GraphBase=_CoordinateRel2Abs(c.pGraphBase)
        # we approximate math.tan by the math.tan @ mid position of the ramp:
        c.angle2tan=math.tan(7.5*math.pi/180)/7.5
        
        if self.main_controller.config.verbose >= 5:
            print(f'CanvasSize={c.pCanvasSize}, SsSize={c.pSsSize}, GraphBase={c.pGraphBase}')

        frame=HeaderFrame(parent, 'Seesaw', padx=0)
        cv = tk.Canvas(frame, width=c.pCanvasSize.x, height=c.pCanvasSize.y, 
                bg=c.cBG, highlightthickness=0)
        c.canvas=cv
        # 1 seesaw
        # 1.1 draw BG
        p1x,p1y = c.SsBase.to_absolute((-int(c.pSsSize.x/2), 0))
        p2x,p2y = c.SsBase.to_absolute((int(c.pSsSize.x/2), c.pSsSize.y))
        cv.create_rectangle(p1x, p1y, p2x, p2y, fill=c.cSsBG)
        # 1.2 draw stand
        standRel=[C(-int(c.wSsStand/2),0), C(0,c.hSsStand), C(int(c.wSsStand/2),0)]
        standAbs=[]
        for i in standRel:
            _absx,_absy = c.SsBase.to_absolute((i.x,i.y))
            standAbs.append(_absx)
            standAbs.append(_absy)
        cv.create_polygon(standAbs, fill=c.cSsStand)
        # 1.3 reference marker
        c.dynamics.referenceMarker=_ReferenceMarker(c, 0)
        c.dynamics.ball=_Ball(c, (0,0))
        c.dynamics.ramp=_Ramp(c,0)
        c.dynamics.boing=_Boing(c, False)
        
        # 2 graph
        c.graph=struct
        dy = round(c.pGraphSize.y/2)
        p1x,p1y = -1, -dy-1
        p2x,p2y = c.pGraphSize.x+1, +dy+1
        cv.create_rectangle(
                c.GraphBase.to_absolute((p1x,p1y)), 
                c.GraphBase.to_absolute((p2x,p2y)), 
                fill=c.cGraphBG, 
                outline=c.cBG)
        # grid
        for i in range(0,c.pGraphSize.x+2,50):
            cv.create_line(
                    c.GraphBase.to_absolute((i, +dy)),
                    c.GraphBase.to_absolute((i, -dy)),
                    fill=c.cGraphGrid,
                    width=1,
                    tags='grid')
        x = c.pGraphSize.x
        for i in (-15, -10, -5, 0, 5, 10, 15):
            cv.create_line(
                c.GraphBase.to_absolute((0, round(i/15*dy))),
                c.GraphBase.to_absolute((x, round(i/15*dy))),
                fill=c.cGraphGrid,
                width=1,
                tags='grid')
        if self.main_controller.config.verbose >=5:
            cv.itemconfig('grid', fill='red')            
        # cursor
        c.cursor=cv.create_line(
                c.GraphBase.to_absolute((0,-dy)),
                c.GraphBase.to_absolute((0,+dy)),
                fill=c.cGraphCursor)
        c.dynamics.graph=[]
        template={'ref': None, 'ball': None, 'angle': None}
        for i in range(round(c.pGraphSize.x/c.graph_scale_x) + 1):
            c.dynamics.graph.append(dotdict(template))
        c.graph.i = c.pGraphSize.x  # last drawn position
        
        cv.grid(row=1, column=0)
        return frame
        

    def _seesaw_graph(self, reference, ball, angle):
        '''
        we have to draw one vertical line of the graph
        last drawn position is in c.graph.i
        '''
        c=self.data
        # determine next index
        c.graph.i+=1
        if c.graph.i > round(c.pGraphSize.x/c.graph_scale_x):
            c.graph.i=0
        i_next=c.graph.i+1
        if i_next > round(c.pGraphSize.x/c.graph_scale_x):
            i_next=0
        # delete next row
        test = c.dynamics.graph[i_next]
        if self.main_controller.config.verbose >= 5:
            print(f'del check {test} from slot {i_next}')
        for i in test.keys():
            if test[i] != None:
                if self.main_controller.config.verbose >= 5:
                    print(f'Deleting {i} from slot {i_next}')
                c.canvas.delete(test[i])
                test[i] = None
        # move cursor
        step_x=c.graph_scale_x
        if i_next == 0:
            step_x*=-round(c.pGraphSize.x/c.graph_scale_x)
        c.canvas.move(c.cursor, step_x, 0)
        # safety check: current must be empty
        test = c.dynamics.graph[c.graph.i]
        for i in test.keys():
            if test[i] != None:
                raise Exception('Graph row not empty')
        y = round(c.pGraphSize.y/2)
        y_ref = round(reference/.6 * y)
        y_ball = round(ball/.6 * y)
        y_angle = round(angle/15 * y)
        xa,ya_ref = c.GraphBase.to_absolute((0, y_ref))
        xa,ya_ball = c.GraphBase.to_absolute((0, y_ball))
        xa,ya_angle = c.GraphBase.to_absolute((c.graph.i * c.graph_scale_x, y_angle))
        if c.graph.i == 0:
            xf = xa + c.graph_scale_x
            yf_ref = ya_ref
            yf_ball = ya_ball
            yf_angle = ya_angle
        else:
            xf = xa - c.graph_scale_x
            yf_ref = c.graph.yf_ref
            yf_ball = c.graph.yf_ball
            yf_angle = c.graph.yf_angle
        c.dynamics.graph[c.graph.i].angle = c.canvas.create_line(
                xf, yf_angle, xa, ya_angle, fill=c.cSsRamp, width=2)
        c.dynamics.graph[c.graph.i].ref = c.canvas.create_line(
                xf, yf_ref, xa, ya_ref, fill=c.cSsRef, width=2)
        c.dynamics.graph[c.graph.i].ball = c.canvas.create_line(
                xf, yf_ball, xa, ya_ball, fill=c.cSsBall, width=2)
        c.graph.yf_ref=ya_ref
        c.graph.yf_ball=ya_ball
        c.graph.yf_angle=ya_angle

    def show(self, reference, ball, angle, boing):
        '''
        new data from the MCU arrived, we have to visualize it
        reference: -0.5..+0.5 meters
        position: -0.6..+0.6 meters
        angle: -15..+15 degrees
        boing: True/False
        '''
        c=self.data
        # 1 visualize current state of the seesaw
        c.dynamics.referenceMarker.move_to(reference)
        c.dynamics.ramp.move_to(angle)
        c.dynamics.ball.move_to((ball, angle))
        c.dynamics.boing.move_to(boing)

        # 2 visualize graph
        self._seesaw_graph(reference, ball, angle)

    def incoming_setter(self, message, position):
        data=message[position:]
        # taken from: https://stackoverflow.com/questions/6727875/hex-string-to-signed-int-in-python
        reference = int.from_bytes(bytes.fromhex(data[0:4]), 'big', signed=True) / 50000.0
        ball = int.from_bytes(bytes.fromhex(data[4:8]), 'big', signed=True) / 50000.0
        angle = int.from_bytes(bytes.fromhex(data[8:12]), 'big', signed=True) / 2000.0
        boing = data[12] == 't'
        self.show(reference, ball, angle, boing)
              
    def test_init(self, test_config):
        t=test_config
        t.ss_ref=-0.5
        t.ss_ball=-0.6
        t.ss_angle=-15
        t.ss_boing=False
        return 2

    def test_do(self, call_counter, t):
        t.ss_ref+=.0051
        if t.ss_ref > 0.5:
            t.ss_ref=-0.5
        t.ss_ball+=.0126
        if t.ss_ball > 0.6:
            t.ss_ball=-0.6
        t.ss_angle+=.29
        if t.ss_angle > 15:
            t.ss_angle=-15
        t.ss_boing = t.ss_ball <= 0            
        self.show(t.ss_ref, t.ss_ball, t.ss_angle, t.ss_boing)
  
Coordinate.default_order='xy'
