logger:info("Processing METEOR-M LRPT data...")

temp_file = filename .. ".wav"
command = "sox -t raw -e floating-point -b 32 -c 2 -r " .. samplerate .. " '" .. input_file .. "' -t wav -e signed-integer -b 16 -c 2 -r " .. samplerate .. " '" .. temp_file .. "'"

logger:debug(command)
cmd_output = os.execute(command)
if (not cmd_output == 0) then logger:error("Sox command failed!") end

lrpt_file = filename .. ".lrpt"
command = "meteor_demod -q -B -s " .. samplerate .. " '" .. temp_file .. "' -o '" .. lrpt_file .. "'"

logger:debug(command)
cmd_output = os.execute(command)
if (not cmd_output == 0) then logger:error("meteor_demod command failed!") end

os.remove(temp_file)

output_file = filename .. ".png"
command = "medet '" .. lrpt_file .. "' '" .. filename ..
              "' -r 65 -g 65 -b 64 > /dev/null"

logger:debug(command)
cmd_output = os.execute(command)
if (not cmd_output == 0) then logger:error("medet command failed!") end

if (not file_exists(output_file)) then logger:warn("LRPT file contains no data!") end

logger:info("Done processing METEOR-M LRPT data!");
