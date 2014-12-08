
function string.trim(str)
	local a,b = string.find(str,"[^ \t\n].*[^ \t\n]")
	if a== nil then return str
	else return string.sub(str,a,b) end
end
function string.split(str,pattern,m)
	local ret = {}
	local sidx,eidx = string.find(str,pattern)
	if m == nil then m = 0xffffffff end
	if sidx == nil then
		table.insert(ret,str)
		return ret
	elseif sidx>1 then
		table.insert(ret,string.sub(str,1,sidx-1))
	end
	while sidx > 0 and #ret < m-1 do
		local nsidx,neidx = string.find(str,pattern,eidx+1)
		if nsidx == nil then
			table.insert(ret,string.trim(string.sub(str,eidx+1)))
			return ret
		elseif nsidx-1 >= eidx+1 then
			table.insert(ret,string.trim(string.sub(str,eidx+1,nsidx-1)))
		end
		sidx,eidx = nsidx,neidx
	end
	table.insert(ret,string.trim(string.sub(str,eidx+1)))
	return ret
end



fw = io.open("db/warp_db_.txt","w")
file = io.open("db/warp_db.txt")

if file ~= nil then
	for v in file:lines() do
		string.gsub(v,";.*","")
		t = string.split(v,":",2)
		if t[1] == "warp" then
			local s = string.split(t[2],",",8)
			if #s >= 8 then
				fw:write("warp\t")
				fw:write(table.concat(s,"\t"))
				fw:write("\n")
			end
		end
	end
end

file:close()
fw:close()
