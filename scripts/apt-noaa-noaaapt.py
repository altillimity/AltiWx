import altiwx
import subprocess

altiwx.info("Processing NOAA APT data...")

output_file = altiwx.filename + ".png"

outflag = ""
if altiwx.northbound:
    outflag = "-R yes"

command = "noaa-apt '" + altiwx.input_file + "' " + outflag + " -c histogram -o '" + output_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing NOAA APT data!")
