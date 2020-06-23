logger:info("Processing NOAA DSB data...")

frame_file = filename .. ".txt"

outflag = ""
if (northbound) then
    outflag = "-N"
end
if (southbound) then
    outflag = "-S"
end

command = "demodPOES -s 48 '" .. input_file .. "' -o '" .. frame_file .. "'"

logger:debug(command)
local cmd_output = os.execute(command)
if (not cmd_output == 0) then logger:error("demodPOES command failed!") end

command = "java -jar NOAA_HIRS_Decoder.jar config.ini '" .. frame_file .. "'"

logger:debug(command)
local cmd_output = os.execute(command)
if (not cmd_output == 0) then logger:error("NOAA_HIRS_Decoder command failed!") end

output_file = filename .. "/msa.png"

logger:info("Done processing NOAA DSB data!");
