require ( "conf")
require ("_server")

function include(f) -- reload
	package.loaded[f] = nil;
	require(f)
end
include("core/core")
function can_damage(obj,target)
	local o = objects[obj]
	local t = objects[target]
	if o == nil or t == nil then return false
	elseif o.type == 1 and t.type == 2 then return true
	elseif o.type == 2 and t.type == 1 then return true
	end
	return false
end
function printf(fmt,...)
	print(string.format(fmt,...))
end
function debugf(fmt,...)
	_consolecol(0,15)
	print(string.format(fmt,...))
end
function warnf(fmt,...)
	_consolecol(0,14)
	print(string.format(fmt,...))
end

function errf(fmt,...)
	_consolecol(0,10)
	print(string.format(fmt,...))
end


function reload()
	include("game")
	reinit()
	_eventinit()
end

function filecheck(f)
	local file = io.open(f)
	if file == nil then
		return false
	else
		file:close()
		return true
	end
end

function onOpen()
	core_init()
end
function get_frontobj(obj)
	local side = _getside(obj)
	local m,x,y = _getpos(obj)
	if side == 0 then
		return _getobj_xy(m,x,y-1)
	elseif side == 1 then
		return _getobj_xy(m,x+1,y)
	elseif side == 2 then
		return _getobj_xy(m,x,y+1)
	elseif side == 3 then
		return _getobj_xy(m,x-1,y)
	end
	return nil
end

function npc(user)
::first::
	local msg = dialog( user,32768, 0 ,"¾È´¨!",false, 1, 0)
	if msg == 2 then
	::second::
		msg = dialog(user , 32768, 0 ,"¾ÆÇÏÇÏ?¤¾",false,1,1)
		if msg == 0 then goto first
		elseif msg == 2 then
			local l = list(user,32768,0,"°ñ¶óºÁ",false,1,1,"¸Ş´º1","¸Ş´º2","¸Ş´º3")
			if l ~= nil and l >= 0 then
				msg = dialog(user,32768,0,l .. " ¹øÀ» °ñ¶ú±¸³ª!",false,0,0)
			end
		end
	end
end
