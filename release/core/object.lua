
function id2obj(id)
	return objects[id]
end

ObjectClass = ObjectClass or CreateClass()
objects = objects or {}
function ObjectClass:canmove(m,x,y)
	return _can_move(m.id,x,y)
end
function ObjectClass:warp(m,x,y,eff)
	local s = maps[m]
	if eff ~= nil then
		local bm,bx,by = _getpos(self.id)
		if bm ~= nil then
			_effect_xy(bm,bx,by,eff,0)
		end
	end
	_warp(self.id,s.id,x,y)
end
function ObjectClass:delete()
	objects[self.id] = nil
end
function ObjectClass:side()
	return _getside(self.id)
end
function ObjectClass:getpos()
	local m,x,y = _getpos(self.id)
	return maps[m],x,y
end
function ObjectClass:say(tp,mes)
	if self.type ~= 4 then
		_say(self.id,tp,mes)
	end
end
function ObjectClass:check_sight(obj)
	return _check_sight(self.id,obj.id)
end
ObjectLiveClass = CreateClass(ObjectClass)
function ObjectLiveClass:die()
	self:setstate(0)
end

function ObjectClass:input(user,mes)
	return _input(user.id,self.db.image,self.db.color,mes)
end
function ObjectClass:dialog(user,mes,n,p)
	return _dialog(user.id,self.db.image,self.db.color,mes,false,n or false,p or false)
end
function ObjectClass:list_t(user,mes,n,p,t)
	return _list(user.id,self.db.image,self.db.color,mes,false,n,p,unpack(t))
end
function ObjectClass:ilist(user,mes,t)
	return _ilist(user.id,self.db.image,self.db.color,mes,unpack(t))
end
function ObjectClass:list(user,mes,n,p,...)
	return _list(user.id,self.db.image,self.db.color,mes,false,n,p,...)
end
function ObjectClass:islotlist(user,mes,t)
	return _islotlist(user.id,self.db.image,self.db.color,mes,unpack(t))
end
function ObjectClass:menu(user,mes,...)
	return _menu(user.id,self.db.image,self.db.color,mes,...)
end
function ObjectLiveClass:damage(dam,tp,target)
	--[[
		데미지 타입
		1	: 물리공격
		2	: 마법공격
		4	: 무장무시
		8	: 마법방어무시
	]]--
	if AND(tp,8) == 0 and target.magicac ~= nil and math.random(1,100) < target.magicac*10 then
		return 1
	end
	if AND(tp,4) == 0 then
		dam = dam * (1 + (self.ac / 200))
	end
	self.target = target
	if self.type == 2 or self.type == 16 then
		_set_target(self.id,target.id)
	end
	if dam >= self.hp then
		self:sethp(0)
		self:die(target)
	else
		self:sethp( self.hp - dam )
	end
	self:gage(dam,false)
	return 0
end

function ObjectLiveClass:gage(dam,critical)
	_gage(self.id,dam,(self.hp/self.maxhp),critical)
end
function ObjectLiveClass:check_live()
	return objects[self.id] == self and self.state ~= 1
end
function ObjectLiveClass:setac(ac)
	if ac < -199 then
		ac = -199
	end
	self.ac = ac
	if self.type == 1 then _setac(self.id,ac) end
end
function ObjectLiveClass:setstate(state)
	self.state = state
	_setstate(self.id,state)
end
function ObjectLiveClass:can_damage(target)
	return can_damage(self.id,target.id)
end
function ObjectLiveClass:healhp(hp)
	self:sethp(self.hp + hp)
	self:gage(-hp,false)
end
function ObjectLiveClass:frontenemy()
	local t = {get_frontobj(self.id)}
	if t == nil then return nil end
	for k,v in pairs(t) do
		if can_damage(self.id,v) then
			return objects[v]
		end
	end
	return nil
end
function ObjectLiveClass:effect(eff,snd)
	_effect(self.id,eff,1)
	if snd ~= nil then
		_sound(self.id,snd)
	end
end
function ObjectLiveClass:action(act,tp,tm)
	_action(self.id,act,tp,tm)
end
function FalledMagicTimer(t)
	local tm = t.scr(t.obj,t.lv,false)
	if tm == 0 then
		t.obj.falledmagics[t.magic.name] = nil
		if t.obj.type == 1 then
			t.obj:message(3, t.message .. " 해제")
		end
		return 0
	else
		return tm
	end
end
function ObjectLiveClass:disappear_magic(mg)
	local t = self.falledmagics[mg]
	if t ~= nil then
		if t.discr ~= nil then
			t.discr(t.obj,unpack(t.datas))
		end
		if t.obj.type == 1 then
			t.obj:message(3,t.message .. " 해제")
			_magic_fall(t.obj.id,0,t.message)
		end
		t.timer:delete()
	end
	self.falledmagics[mg] = nil
end
function ObjectLiveClass:disappear_magics()
	for k,v in pairs(self.falledmagics) do
		if v.discr ~= nil then
			v.discr(v.obj,unpack(v.datas))
		end
		if v.obj.type == 1 then
			v.obj:message(3,v.message .. " 해제")
			_magic_fall(v.obj.id,0,v.message)
		end
		v.timer:delete()
	end
	self.falledmagics = {}
end
function _magic_fall_func(t)
	local obj = objects[t.obj.id]
	if obj ~= t.obj then
		return -1
	else

		if t.stop <= os.clock() then
			obj:disappear_magic(t.mg)
			return -1
		end
		local r =  t.scr(obj,unpack(t.datas))
		if r ~= nil then return r
		else return t.stop - os.clock() end
	end
	return -1
end
function ObjectLiveClass:checkfall(mg)
	local m = self.falledmagics[mg]
	if m ~= nil then
		return m.lv
	else
		return nil
	end
end
function ObjectLiveClass:magicfall2(mg,mes,lv,tm,...)
	local m = fall_datas[mg]
	local t = self.falledmagics[mg]
	if t ~= nil then
		return false
	end
	if m ~= nil then
		t = {}
		t.mg = mg
		t.message = mes
		t.fall = m
		t.obj = self
		t.lv = lv
		t.scr = m.scr
		t.discr = m.discr
		t.datas = {...}
		t.stop = os.clock() + tm
		self.falledmagics[mg] = t
		t.timer = LuaTimer.create(0,_magic_fall_func,t)
		if self.type ==1 then
			_magic_fall(self.id,tm,mes)
		end
		return true
	end
	return false
end
function ObjectLiveClass:magicfall(mg,lv,tm,...)
	local m = fall_datas[mg]
	local t = self.falledmagics[mg]
	if t ~= nil then
		return false
	end
	if m ~= nil then
		t = {}
		t.mg = mg
		t.message = m.msg
		t.fall = m
		t.obj = self
		t.lv = lv
		t.scr = m.scr
		t.discr = m.discr
		t.datas = {...}
		t.stop = os.clock() + tm
		self.falledmagics[mg] = t

		t.timer = LuaTimer.create(0,_magic_fall_func,t)
		if self.type ==1 then
			_magic_fall(self.id,tm,m.msg)
		end
		return true
	end
	return false
end
function ObjectLiveClass:healmp(mp)
	self:setmp(self.mp + mp)
end
function ObjectLiveClass:delmp(mp)
	mp = math.min(mp,self.mp)
	self:setmp(self.mp - mp)
end
function ObjectLiveClass:delhp(hp)
	hp = math.min(hp,self.hp)
	self:sethp(self.hp - hp)
end
function ObjectLiveClass:sound(s)
	_sound(self.id,s)
end
function ObjectLiveClass:setmaxhp(hp)
	self.maxhp = hp
	self.hp = math.min(hp,self.hp)
	if self.type == 1 then
		_setmaxhp(self.id,self.maxhp)
		_sethp(self.id,self.hp)
	end
end
function ObjectLiveClass:setmaxmp(mp)
	self.maxmp = mp
	self.mp = math.min(mp,self.mp)
	if self.type == 1 then
		_setmaxmp(self.id,self.maxmp)
		_setmp(self.id,self.mp)
	end
end
function ObjectLiveClass:sethp(hp)
	self.hp = math.min(hp,self.maxhp)
	if self.type == 1 then
		_sethp(self.id,self.hp)
	end
end
function ObjectLiveClass:setmp(mp)
	self.mp = math.min(mp,self.maxmp)
	if self.type == 1 then
		_setmp(self.id,self.mp)
	end
end

function ObjectLiveClass:setside(side)
	_setside(self.id,side)
end
