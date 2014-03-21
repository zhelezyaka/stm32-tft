import sys
import ctypes
from sdl2 import *
from fonts import *

TTF_WIDTH = 240
TTF_HEIGHT = 320    

gram = 0

window = None
renderer = None
background = 0x001F

def run():
    global gram
    global window
    global renderer
    global TTF_WIDTH
    global TTF_HEIGHT
    
    print('PySDL TTF Simulator！')
    SDL_Init(SDL_INIT_VIDEO)
    window = SDL_CreateWindow(b"2.4 TFT 240x320",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              TTF_WIDTH, TTF_HEIGHT, SDL_WINDOW_SHOWN)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255)

    clear(0x001F)
    showString(0, 0, 'Hello World! Nice To MEET YOU')
    
    running = True
    event = SDL_Event()
    while running:
        while SDL_PollEvent(ctypes.byref(event)) != 0:
            if event.type == SDL_QUIT:
                running = False
                break
            
        SDL_RenderPresent(renderer);
        SDL_Delay(200)

    SDL_DestroyWindow(window)
    SDL_Quit()
    return 0

def setColor(color):
    r, g, b = hexToRgb(color)
    SDL_SetRenderDrawColor(renderer, r, g, b, 255)
    
def setBackground(r, g, b):
    global background
    background = (r, g, b)
    
def setCursor(x, y):
    global gram
    gram = y * TTF_WIDTH + x
    
def hexToRgb(rgb565):
    r = (rgb565 & 0xF800) >> 11
    g = (rgb565 & 0x07E0) >> 5
    b = (rgb565 & 0x001F)
    r <<= 3
    g <<= 2
    b <<= 3
    return (r, g, b)
 
def writeGram(color):
    global gram
    global window
    global renderer
    global TTF_WIDTH
    global TTF_HEIGHT
    
    setColor(color)
    
    x = gram % TTF_WIDTH
    y = int(gram / TTF_WIDTH)
    SDL_RenderDrawPoint(renderer, x, y)
    gram += 1
    
def clear(color):
    setCursor(0, 0)
    for i in range (0, 76800):
        writeGram(color)

def showChar(x, y, ch):
    ascii = ord(ch)
    ascii -= 32;
    pos = ascii * 24
    drawChar(x, y, ASCII16x24_Table[pos : pos+24]);
    
def drawChar(x, y, chArr):
    for yOffset in range(0, 24):
        setCursor(x, y + yOffset)
        for i in range(0, 16):
            if not (chArr[yOffset] & (1 << i)) == 0x00:
                writeGram(0xFFFF)
            else:
                writeGram(background)
     
def showString(x, y, msg):
    for s in msg:
        showChar(x, y, s)
        if x + 16 > 239 - 16:
            x = 0
            y += 24
        else:
            x += 16
    
def chTest():
    x = 0
    y = 96
    ch = [0x10,0x10,0x08,0x20,0x04,0x48,0x3F,0xFC,
          0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x08,
          0x21,0x08,0x3F,0xF8,0x21,0x00,0x01,0x04,
          0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00]
    for yOffset in range(0, 32):
            setCursor(x, y + yOffset)
            for i in range(0, 16):
                if not (ch[yOffset] & (1 << i)) == 0x00:
                    writeGram(0xFFFF)
                else:
                    writeGram(background)
    
    
if __name__ == '__main__':
    print(hexToRgb(0xFFFF))
    sys.exit(run())