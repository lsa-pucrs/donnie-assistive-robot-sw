#!/usr/bin/python2.7
# It tells its IP address during start up
import subprocess
ipNumber = subprocess.check_output("ip route get 8.8.8.8 | awk '{print $NF; exit}'", shell=True)
ipNumber = ipNumber.split('\n')[0].split('.')
msg = "My final ip is " + ipNumber[3]
subprocess.call('echo '+msg+'|festival --tts', shell=True)
subprocess.call('echo '+', repeating... is '+ ipNumber[3]+'|festival --tts', shell=True)
