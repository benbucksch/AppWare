BEGIN
{
local temp
local pattern
pattern = toupper(ARGV[2])
temp = toupper(ARGV[1])
gsub("APPWRE32", "appware32", temp)
gsub(pattern, "$", temp)
print temp
}
