import altiwx
import subprocess

frame_file = altiwx.filename + ".txt"
command = "demodPOES -s 48 '" + altiwx.input_file + "' -o '" + frame_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

output_file = altiwx.filename + "/msa.png"
command = "java -jar NOAA_HIRS_Decoder.jar config.ini '" + frame_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()
