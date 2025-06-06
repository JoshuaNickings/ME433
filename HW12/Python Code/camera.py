import pgzrun # pip install pgzero

import serial
ser = serial.Serial('/dev/tty.usbmodem101') # the name of your port here
print('Opening port: ' + str(ser.name))

import numpy as np

# Set the window size
WIDTH = 400
HEIGHT = 400
def update():
    selection_endline = 'c'+'\n'
     
    # send the command 
    ser.write(selection_endline.encode())
def draw():
    reds = np.zeros((60, 80), dtype=np.uint8)
    greens = np.zeros((60, 80), dtype=np.uint8)
    blues = np.zeros((60, 80), dtype=np.uint8)
    
    
    for t in range(4800):
            dat_str = ser.read_until(b'\n'); 
            i,r,g,b = list(map(int,dat_str.split())) 
            row = i//80
            col = i%80
            reds[row][col] = r
            greens[row][col] = g
            blues[row][col] = b
    print(i)
    print(row)
    print(col)

    screen.fill((0, 0, 0))  # Fill the background with black
    for x in range(60):
         for y in range(80):
              screen.draw.filled_rect(Rect((x, 60-y), (1, 1)), (reds[x][y], greens[x][y], blues[x][y]))

pgzrun.go()
