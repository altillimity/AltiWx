import altiwx
import subprocess

altiwx.info("Processing METEOR-M 2 LRPT data...")

output_file = altiwx.filename + ".bmp"
command = "medet '" + lrpt_file + "' '" + altiwx.filename + "'  -r 65 -g 65 -b 64 > /dev/null"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing METEOR-M 2 LRPT data!")
