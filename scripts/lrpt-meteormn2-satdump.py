import altiwx
import subprocess

altiwx.info("Processing METEOR-M 2 LRPT data...")

output_folder = altiwx.filename
System.IO.Directory.CreateDirectory(output_folder)
command = "satdump meteor_m2_lrpt soft '" + altiwx.input_file + "' '" + output_folder + "' > /dev/null"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing METEOR-M 2 LRPT data!")
