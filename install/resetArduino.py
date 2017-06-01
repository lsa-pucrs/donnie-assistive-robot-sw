from gpiozero import LED
from time import sleep
from signal import pause

led = LED(22)

led.off()
sleep(0.5)
led.on()
