logger:info("Processing NOAA DSB data...")

output_file = filename .. ".txt"

outflag = ""
if (northbound) then
    outflag = "-N"
end
if (southbound) then
    outflag = "-S"
end

command = "demodPOES -s 48 '" .. input_file .. "' -o '" .. output_file .. "'"

logger:debug(command)
local cmd_output = os.execute(command)
if (not cmd_output == 0) then logger:error("demodPOES command failed!") end

logger:info("Done processing NOAA DSB data!");
