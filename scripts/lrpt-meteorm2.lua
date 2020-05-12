logger.info("Processing METEOR-M LRPT data...")

lrpt_file = filename .. ".lrpt"
command = "meteor_demod -q -B -s 140000 '" .. input_file .. "' -o '" .. lrpt_file .. "'"

logger.debug(command)
cmd_output = os.execute(command)
if (not cmd_output == 0) then logger.error("meteor_demod command failed!") end

output_file = filename .. ".png"
command = "medet '" .. lrpt_file .. "' '" .. filename .. "' -r 65 -g 65 -b 64 > /dev/null"

logger.debug(command)
cmd_output = os.execute(command)
if (not cmd_output == 0) then logger.error("medet command failed!") end

if (not file_exists(output_file)) then logger.warn("LRPT file contains no data!") end

logger.info("Done processing METEOR-M LRPT data!");