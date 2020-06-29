import altiwx
import subprocess
import os

altiwx.info("Processing METEOR LRPT data...")

temp_file = altiwx.filename + ".wav"
command = "sox -t raw -e floating-point -b 32 -c 2 -r " + str(altiwx.samplerate) + " '" + altiwx.input_file + "' -t wav -e signed-integer -b 16 -c 2 -r " + str(altiwx.samplerate) + " '" + temp_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

lrpt_file = altiwx.filename + ".lrpt"
command = "meteor_demod -q -B -s " + str(altiwx.samplerate) + " '" + temp_file + "' -o '" + lrpt_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

os.remove(temp_file)

output_file = altiwx.filename + ".bmp"
command = "meteor_decode -a 66,65,64 '" + lrpt_file + "' -o '" + output_file + "' > /dev/null"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing METEOR LRPT data!")