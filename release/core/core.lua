include("core/class")
include("core/object")
include("core/user")
include("core/monster")
include("core/item")
include("core/etc")
include("core/map")
include("core/magic")
include("core/job")
include("core/npc")
include("core/shop")
include("core/command")
include("core/bank")
include("core/luatimer")
include ("core/dbloader")
include("core/pursuit")
server = server or {}


_ERROR_LOG_f = _ERROR_LOG_f or io.open("error_log.txt","a")
_ALERT = function (msg)
	if _ERROR_LOG_f ~= nil then
		print("A")
		_ERROR_LOG_f:write(os.date("[%c] "))
		_ERROR_LOG_f:write(msg .. "\n")
	end
end
function onError(thread,msg)
	warnf("%s",debug.traceback(thread))
end
function onSecond()
	if users ~= nil then
		for k,v in pairs(users) do
			local tm = os.clock() - v.connect_time
			local mnt = math.floor(tm/60)
			local h = math.floor(mnt/60)
			local s = math.floor(tm%60)
			local m,x,y = v:getpos()
			v:notice(string.format("--Developing--\n현재 시간: %s\n접속 시간: %.2d:%.2d:%.2d\n현재 좌표: %d,%d",
			os.date("%X"),
			h,mnt,s,
			x,y))
		end
	end
end
function vartofile(file,name,t)
	if type(t) ~= "table" then
		return false
	end
	--print(file..' open')
	local f = io.open(file,"w")
	if f ~= nil then
		vartofile2(f,name,t)
		io.close(f)
	else return false end
end
function vartofile2(fo,n,t)
	--print(fo,n,t)
	fo:write(n..'={}\n')
	for k,v in pairs(t) do
		local ktype = type(k)
		local vtype = type(v)
		if ktype == "string" then
			k=string.gsub(k,"'","\\'")
			if vtype == "string" then
				v=string.gsub(v,"'","\\'")
				fo:write(n.."['"..k.."']='"..v.."'\n")
			elseif vtype == "number" then
				fo:write(n.."['"..k.."']="..v.."\n")
			elseif vtype == "table" then
				vartofile2(fo,n.."['"..k.."']",v)
			elseif vtype == "boolean" then
				fo:write(n.."['"..k.."']="..tostring(v).."\n")
			end
		elseif ktype == "number" then
			if vtype == "string" then
				v=string.gsub(v,"'","\\'")
				fo:write(n.."["..k.."]='"..v.."'\n")
			elseif vtype == "number" then
				fo:write(n.."["..k.."]="..v.."\n")
			elseif vtype == "table" then
				vartofile2(fo,n.."["..k.."]",v)
			elseif vtype == "boolean" then
				fo:write(n.."["..k.."]="..tostring(v).."\n")
			end
		end
	end
	fo:flush()
end
function onLogin(obj,name)
	UserClass.create(obj,name)
end
function onAttack(obj,per,...)
	local ob = objects[obj]
	per = per or 10
	if ob.state == 1 then
		if ob.type == 1 then
			ob:message(3,"죽은 상태에서는 할 수 없습니다.")
		end
		return
	end
	local targets = {...}
	for i = 1,#targets do
		local tar = objects[targets[i]]
		if tar ~= nil and tar.state ~= 1 then
			ob:attack(tar,per*(11-i)/10)
		end
	end
end
function onClick(user,target)
	local u = objects[user]
	local t = objects[target]
	_set_lastobject(user,target)
	if t.type == 8 and t.db.script ~= nil then
		t.db.script(t,u)
	end
end
function reinit()

	mob_init()
	item_init()
	magic_init()
	shop_init()
	pursuit_init()
	npc_init()

end
function core_init()
	map_init()
	mob_init()
	item_init()
	magic_init()
	npc_init()
	shop_init()
	pursuit_init()
	job_init(15,15)
	npcspawn_init()
	mobspawn_init()
	LuaTimer.create(_WORLD_SAVE_TIME_,world_save,server)
	debugf("- 서버 구동 시작 -")
end

function world_save(s)
	vartofile("_server.lua","server",s)
	return _WORLD_SAVE_TIME_
end
function onDelete(obj)
	local o = objects[obj]
	if o ~= nil and o.type == 1 then
		o:logout()
	end
	if o~= nil and o.type ~= 4 then
		o:disappear_magics()
	end
	objects[obj] = nil
end
function onSay(obj,tp,say)
	print(string.sub(say,1,1))
	if string.sub(say,1,1) == "@" then
		command_shell(obj,say)
	end
	check_pursuit(objects[obj],say)
end
