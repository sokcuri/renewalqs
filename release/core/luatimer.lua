lua_timers = {}

LuaTimer = CreateClass()

function _lua_timer_func(o)
	local r = o.func(o.data)

	if r == nil then
		return o.time
	elseif r < 0 then
		lua_timers[o] = nil
		lua_timers[o.id] = nil
		return -1
	else
		return r
	end
end
function LuaTimer.create(tm,func,data)
	local o = LuaTimer.new()
	o.time = tm
	o.func = func
	o.data = data
	o.id = _timer_add(tm,_lua_timer_func,o)
	lua_timers[o] = o
	lua_timers[o.id] = o
	return o
end
function LuaTimer:delete()
	_timer_del(self.id)
	lua_timers[self] = nil
	lua_timers[self.id] = nil
end


