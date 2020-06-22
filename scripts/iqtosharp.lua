logger:info("Processing I/Q data...")

output_file = filename .. ".wav"

command = "sox -e float -t raw -r " .. samplerate .. " -b 32 -c 2 '" .. input_file .. "'  -t wav -e float -b 32 -c 2 -r " .. samplerate .. " '" .. output_file .. "'"

logger:debug(command)
local cmd_output = os.execute(command)
if (not cmd_output == 0) then logger:error("Sox command failed!") end

logger:info("Done processing I/Q data!");
