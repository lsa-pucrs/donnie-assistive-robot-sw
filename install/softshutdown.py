import os
from gpiozero import Button
from signal import pause
button = Button(4,pull_up=False)

def pressed(button):
    print("Pin %s pressed. The system is going to try shutdown now!" % button.pin)
    os.system("shutdown now -h")

button.when_pressed = pressed
pause()
