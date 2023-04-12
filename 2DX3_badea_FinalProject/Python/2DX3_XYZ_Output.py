#Abdurahman Bade
#400247875

import serial
import math

s = serial.Serial('COM5',115200)

s.open
s.reset_output_buffer()
s.reset_input_buffer()

f = open("2DX3_Output.xyz", "w") 
step = 0
x = 0 
increment = 250 
num_inc = int(input("Please Enter the Number of Steps:"))
count=0

while(count<num_inc):

    raw = s.readline()
    data = raw.decode("utf-8") 
    data = data[0:-2]
    
    if (data.isdigit() == True):
        angle = (step/512)*2*math.pi 
        r = int(data)
        y = r*math.cos(angle) 
        z = r*math.sin(angle)
        
        print(y)
        print(z)
        
        f.write('{} {} {}\n'.format(x,y,z)) 
        step = step+32
        
    if (step == 512): 
        step = 0
        x = x + increment
        count=count+1
        
    print(data)
    
f.close() 
