import wavefile
from os import sys

# returns the contents of the wav file as a double precision float array
def wav_to_floats(filename):
    w = wavefile.load(filename)
    return w[1][0]

if len(sys.argv) < 3:
  quit("Missing arguments\n")

mul_factor = 1
if len(sys.argv) == 4:
  mul_factor = float(sys.argv[3])

signal = wav_to_floats(sys.argv[1])
print ("read "+str(len(signal))+" frames")
print  ("in the range "+str(min(signal))+" to "+str(min(signal)))

float_audio = open(sys.argv[2], 'w')

for amplitude in signal:
  float_audio.write(str(amplitude*mul_factor)+'\n')

float_audio.close()
