import altiwx
import subprocess


altiwx.info("Processing METEOR M2-x LRPT 72k data...")

output_folder = altiwx.filename
command = "satdump meteor_m2-x_lrpt baseband '" + altiwx.input_file + "' '" + output_folder + "' --samplerate 140e3 --baseband_format f32"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing METEOR M2-x LRPT 72k data!")