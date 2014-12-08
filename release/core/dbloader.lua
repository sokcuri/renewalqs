-- DB Loader
--[[
	$def	typename	~	~	~	$1	$2
	npc

]]--

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
function string.trim(str)
	local a,b = string.find(str,"[^ \t\n].*[^ \t\n]")
	if a== nil then return str
	else return string.sub(str,a,b) end
end
function dbLoader_dofile(_dbLoaders,file)
	local fi = io.open(file)
	if fi ~= nil then
		_dbLoaders.file = file
		_dbLoaders.count = 0
		local loading_line = 1
		local t = {}
		for v in fi:lines() do
			v = string.trim(v)
			if string.sub(v,string.len(v)) == "\\" then
				table.insert(t,v)
			else
				if #t >= 1 then
					table.insert(t,v)
					v = table.concat(t)
					t = {}
				end
				_dbLoaders.line = loading_line
				dbLoader_dostring(_dbLoaders,v)
			end
			loading_line = loading_line + 1
		end
		fi:close()
		debugf("[DBLoader] %d개의 데이터를 읽었습니다 ( %s )",_dbLoaders.count,file)
	end
end

function dbLoader_dostring(_dbLoaders,str)
	str = string.gsub(str,";.*","")
	local t = string.split(str,"\t",2)
	local loader = _dbLoaders[t[1]]
	if #t >= 2 then
		if loader ~= nil then
			local len = string.len(loader.args)
			local pat = loader.pattern or "\t"
			local m = string.split(t[2],pat,loader.count)
			if #m >= len then
				for i = 1,len do
					local ch = string.sub(loader.args,i,i)
					if ch == "i" then
						m[i] = tonumber(m[i])
					end
				end
				local s = loader.func(unpack(m))
				if s ~= nil then
					warnf("[DB Loader Warning %s:%d] %s",_dbLoaders.file,_dbLoaders.line,s)
				else
					_dbLoaders.count = _dbLoaders.count + 1
				end
			end
		elseif t[1] == "include" then
			dbLoader_dofile(_dbLoaders,t[2])
		elseif t[1] == "$def" then
			local m = string.split(t[2],"\t",2)
			if #m >= 2 then
				local p = {}
				p.text = m[2]
				p.args = ""
				p.func = function(...)
					local t = {...}
					for i = 1,#t do
						t[tostring(i)] = t[i]
					end
					local m = string.gsub(p.text,"$(%d+)",t)
					dbLoader_dostring(_dbLoaders,m)
				end
				_dbLoaders[m[1]] = p
			end
		else
			warnf("[DB Loader warning %s:%d] %s",_dbLoaders.file,_dbLoaders.line,"알수 없는 형식자 " .. t[1])
		end
	else

	end

end

