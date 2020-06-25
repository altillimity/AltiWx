import altiwx
import subprocess

output_file = altiwx.filename + ".wav"

command = "sox -e float -t raw -r " + altiwx.samplerate + " -b 32 -c 2 '" + altiwx.input_file + "'  -t wav -e float -b 32 -c 2 -r " + altiwx.samplerate + " '" + output_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()
