import altiwx
import subprocess

output_file = altiwx.filename + ".png"

outflag = ""
if altiwx.southbound:
    outflag = "-S"
if altiwx.northbound:
    outflag = "-N"

command = "wxtoimg -q -A " + outflag + " -e HVCT '" + altiwx.input_file + "' '" + output_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()
