from TftSimulator import *

def run():
    lcd = TftSimulator()
    lcd.clear(0x001F)
    lcd.setFont(FontFactory.get8x8Font())
    lcd.drawString(0, 0, 'Hello World !', 0xFFFF, 0x001F)
    lcd.run()
    
if __name__ == '__main__':
    sys.exit(run())