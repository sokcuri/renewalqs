include("script/magic")
function onUseMagic(obj,id,lv,slot,sub)
	local o = objects[obj]
	local m = magic_datas[id]
	if m.type == 2 and m.script ~= nil then
		m.script(o,m,lv,objects[sub])
	else m.script(o,m,lv,sub)
	end
end
function regdb_fall(name,falmsg,script,discript)
	local f = fall_datas[name]
	if f == nil then f = {} end
	f.name = name
	f.msg = falmsg
	f.scr = _G[script]
	f.discr = _G[discript]
	fall_datas[name] = f
end
function magic_init()

	dbLoader_dofile(_MagicLoader,"db/magic_db.txt")
end
fall_datas = fall_datas or {}
magic_datas = magic_datas or {}
function regdb_magic(id,name,tp,message,script,args,...)
	local m = magic_datas[id]
	if m == nil then m = {} end
	m.code = id
	m.name = name
	m.type = tp
	m.message = message
	m.script = _G[script]
	if args ~= nil then
		local len = string.len(args)
		m.sub = {...}
		for i = 1,len do
			local s = string.sub(args,i,i)
			if s == "i" then
				m.sub[i] = tonumber(m.sub[i])
			elseif s == "f" then
				m.sub[i] = _G[m.sub[i]]
			end
		end
	end
	magic_datas[id] = m
	magic_datas[name] = m
	magic_datas[m] = m
	_magicdb_set(id,tp,name,message)
end
_MagicLoader= {

	fall = {
		args = "ssff",
		func = regdb_fall
	},
	magic = {
		args = "isisf",
		func = regdb_magic
	},
	line = 0,
	file = nil,
}
