
commands = {
	["@¸ó½ºÅÍ"] ={
		args = 2,
		func = function (obj,t)
			if mob_datas[t[1]] ~= nil then
				local cnt = tonumber(t[2])
				for i = 1,cnt do
					local m = MobClass.create(mob_datas[t[1]].dbid)
					m:warp(_getpos(obj.id))
				end
			end
		end
	},
	["@ÀÌÆåÆ®"] = {
		args = 1,
		func = function(obj,t)
			local eff = tonumber(t[1])
			local snd = tonumber(t[2])
			obj:effect(eff,snd)
		end
	},
	["@¸¶¹ý"] = {
		args = 1,
		func = function (obj,t)
			obj:give_magic(t[1])
		end
	},
	["@¾ÆÀÌÅÛ"] = {
		args = 2,
		func = function (obj,t)
			if item_datas[t[1]] ~= nil then
				give_item3(obj.id,item_datas[t[1]].dbid,tonumber(t[2]))
			end
		end
	},
	["@¿öÇÁ"] = {
		args = 3,
		func = function (obj,t)
			if map_datas[t[1]] ~= nil then
				obj:warp(t[1],tonumber(t[2]),tonumber(t[3]))
			end
		end
	},
	["@ÁÂÇ¥"] = {
		func = function ( obj)
			obj:message(3,string.format("ÁÂÇ¥ %d,%d,%d",_getpos(obj.id)))
		end
	},
	["@¸®·Îµå"] ={
		func = function (obj)
			reload()
		end
	},
	["@µ·"] = {
		args = 1,
		func = function(obj,t)
			obj:givemoney(tonumber(t[1]))
		end
	}
}

function command_shell(obj,say)
	local o = objects[obj]
	local t = string.split(say," ",2)
	if #t >= 1 and commands[t[1]] ~= nil then
		local cmd = commands[t[1]]
		if #t>= 2 and cmd.args ~= nil then
			local t = string.split(t[2],",",cmd.args)
			if #t >= cmd.args then
				cmd.func(o,t)
			end
		elseif cmd.args == nil then
			cmd.func(o)
		end
	end
end
