from sdl2 import *
from fonts import *
import sys
import types
import ctypes

class TftSimulator():
    def __init__(self):
        self.width = 240
        self.height = 320
        self.gram = 0
        self.font = FontFactory.get16x24Font()
        SDL_Init(SDL_INIT_VIDEO)
        self.window = SDL_CreateWindow(b"2.4 TFT 240x320",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              self.width, self.height, SDL_WINDOW_SHOWN)
        self.renderer = SDL_CreateRenderer(self.window, -1, SDL_RENDERER_ACCELERATED)
        SDL_SetRenderDrawColor(self.renderer, 255, 255, 255, 255)
        
    def run(self):
        running = True
        event = SDL_Event()
        while running:
            while SDL_PollEvent(ctypes.byref(event)) != 0:
                if event.type == SDL_QUIT:
                    running = False
                    break
                    
            SDL_RenderPresent(self.renderer);
            SDL_Delay(200)
        
        SDL_DestroyWindow(self.window)
        SDL_Quit()

    def rgb565ToRgb(self, rgb565):
        r = (rgb565 & 0xF800) >> 11
        g = (rgb565 & 0x07E0) >> 5
        b = (rgb565 & 0x001F)
        r <<= 3
        g <<= 2
        b <<= 3
        return (r, g, b)   
         
    def setColor(self, rgb565):
        r, g, b = self.rgb565ToRgb(rgb565)
        SDL_SetRenderDrawColor(self.renderer, r, g, b, 255)
    def setFont(self, font):
        self.font = font
        
    def writeGram(self, color):
        x = self.gram % self.width
        y = int(self.gram / self.width)
        self.setColor(color)
        SDL_RenderDrawPoint(self.renderer, x, y)
        self.gram += 1   
        
    def setCursor(self, x, y):
        self.gram = y * self.width + x
        
    def clear(self, color):
        self.setCursor(0, 0)
        for i in range(0, self.width * self.height):
            self.writeGram(color)
      
    def drawChar(self, x, y, charArr, color, background):
        if isinstance(charArr, str):
            return self.drawChar(x, y, self.font.getDotArray(charArr), color, background)
            
        for yOffset in range(0, self.font.getHeight()):
            self.setCursor(x, y + yOffset)
            for i in range(0, self.font.getWidth()):
                if not (charArr[yOffset] & (1 << i)) == 0x00:
                    self.writeGram(color)
                else:
                    self.writeGram(background)   
                     
    def drawString(self, x, y, msg, color, background):
        for s in msg:
            self.drawChar(x, y, s, color, background)
            if x + self.font.getWidth() > self.width - 17:
                x = 0
                y += self.font.getHeight()
            else:
                x += self.font.getWidth()                    
        
class TftFont():
    def __init__(self, width, height, dotArr):
        self.width = width
        self.height = height
        self.dots = dotArr
        
    def getDotArray(self, ch):
        asci = ord(ch)
        asci -= 32
        pos = asci * self.height
        return self.dots[pos:pos + self.height]
        
    def getWidth(self):
        return self.width
    
    def getHeight(self):
        return self.height
    
class FontFactory():
    @staticmethod
    def get16x24Font():
        font = TftFont(16, 24, ASCII16x24_Table)
        return font
       
    @staticmethod
    def get12x12Font():
        font = TftFont(12, 12, ASCII12x12_Table)
        return font
    
    @staticmethod
    def get8x12Font():
        font = TftFont(8, 12, ASCII8x12_Table)
        return font
    
    @staticmethod    
    def get8x8Font():
        font = TftFont(8, 8, ASCII8x8_Table)
        return font
    
    