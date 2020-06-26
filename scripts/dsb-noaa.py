import altiwx
import subprocess
import os

altiwx.info("Processing NOAA DSB data...")

frame_file = altiwx.filename + ".txt"
command = "demodPOES -s 48 '" + altiwx.input_file + "' -o '" + frame_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

output_file = altiwx.filename + os.path.basename(altiwx.filename) + "/msa.png"
command = "java -jar NOAA_HIRS_Decoder.jar config.ini '" + os.path.abspath(frame_file) + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1, cwd=os.path.dirname(altiwx.filename)).wait()

altiwx.info("Done processing NOAA DSB data!")