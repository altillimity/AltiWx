import altiwx
import subprocess

altiwx.info("Processing I/Q data...")

output_file = altiwx.filename + ".wav"

command = "sox -e float -t raw -r " + str(altiwx.samplerate) + " -b 32 -c 2 '" + altiwx.input_file + "'  -t wav -e float -b 32 -c 2 -r " + str(altiwx.samplerate) + " '" + output_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing I/Q data!")