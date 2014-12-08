users = users or {}
user_datas = user_datas or {}

function name2user(name)
	return users[name]
end
UserClass = UserClass or CreateClass(ObjectLiveClass)

function user_save(name)
	local user = users[name]
	if user == nil then
		return -1
	else
		user.db.hp = math.min(user.hp,user.db.maxhp)
		user.db.mp = math.min(user.mp,user.db.maxmp)
		user.db.m,user.db.x,user.db.y = _getpos(user.id)
		user.db.side = _getside(user.id)
		user.db.ownitems = {}
		user.db.equips = {}
		for i = 0,25 do
			local item = user:ownitem(i)
			if item ~= nil then
				local m = {}
				user.db.ownitems[i] = m
				m.dbid = item.dbid
				m.name = item.name
				m.count = item:count()
				if item.db.max_count == 1 then
					m.save = item.save
				end
			end
		end
		for i = 0,13 do
			local item = user:equip(i)
			if item ~= nil then
				local m = {}
				user.db.equips[i] = m
				m.dbid = item.dbid
				m.name = item.name
				m.count = 1
				m.save = item.save
			end
		end
		vartofile("user/" .. user.name .. ".lua","user_datas[\"" .. user.name .."\"]",user.db)

	end
	return _USER_SAVE_TIME_
end
function onPickup(uobj,iobj)
	local user = id2obj(uobj)
	if iobj ~= nil then
		local item = ItemClass.get(iobj)
		local r = user:giveitem(item,true)
		if r == 1 then
			user:message(3,"더 이상 가질 수 없습니다.")
		elseif r == -1 then
			user:message(3,"소지품이 꽉 찼습니다.")
		end
	end
	if user.state == 2 then
		user:disappear_magic("투명")
	end
end
function UserClass:die(target)

end
function UserClass:giveitem2(itemname,count,remain)
	local r = _give_item2(self.id,item_datas[itemname].dbid,count,remain or false)
	if r == 1 then
		self:message(3,"더 이상 가질 수 없습니다.")
		return false
	elseif r == -1 then
		self:message(3,"소지품이 꽉 찼습니다.")
		return false
	end
	return true
end

function UserClass:giveitem(item,remain)
	local r = _give_item(self.id,item.id,remain or false)
	if r == 1 then
		self:message(3,"더 이상 가질 수 없습니다.")
		return false
	elseif r == -1 then
		self:message(3,"소지품이 꽉 찼습니다.")
		return false
	end
	return true
end
function UserClass:checkitem(item,count)
	local idb = item_datas[item]
	local iobj = _check_item(self.id,idb.dbid,count)
	if iobj == nil then return nil
	else return ItemClass.get(iobj) end
end
function UserClass:setEquip(item)
	self:setmight(self.might + item.db.might)
	self:setwill(self.will + item.db.will)
	self:setgrace(self.grace + item.db.grace)
	self:sethit(self.hit + item.db.hit)
	self:setdam(self.dam + item.db.dam)
	self.hp_per = self.hp_per + item.db.hp_per
	self.mp_per = self.mp_per + item.db.mp_per
	self.eqhp = self.eqhp + item.db.hp
	self.eqmp = self.eqmp + item.db.mp
	self:setmaxhp(self.db.maxhp*(1+self.hp_per/100) + self.eqhp)
	self:setmaxmp(self.db.maxmp*(1+self.mp_per/100) + self.eqmp)
	self:setac(self.ac + item.db.ac)
	self:message(3,"무장 " .. (self.ac))
end

function UserClass:takeon(item,slot)
	if item.db.job ~= 0 and item.db.job ~= self.db.job then
		self:message(3,"무기 성격이 맞지않습니다.")
	elseif item.db.level > self.db.level then
		self:message(3,"어디서 감히")
	else
		_equip_takeon(self.id,slot)
		self:setEquip(item)
	end
end
function UserClass:teleport(dist)
	_teleport(self.id,dist)
end
function UserClass:unsetEquip(item)
	self:setmight(self.might - item.db.might)
	self:setwill(self.will - item.db.will)
	self:setgrace(self.grace - item.db.grace)
	self:sethit(self.hit - item.db.hit)
	self:setdam(self.dam - item.db.dam)
	self.hp_per = self.hp_per - item.db.hp_per
	self.mp_per = self.mp_per - item.db.mp_per
	self.eqhp = self.eqhp - item.db.hp
	self.eqmp = self.eqmp - item.db.mp
	self:setmaxhp(self.db.maxhp*(1+self.hp_per/100) + self.eqhp)
	self:setmaxmp(self.db.maxmp*(1+self.mp_per/100) + self.eqmp)
	self:setac(self.ac - item.db.ac)
	self:message(3,"무장 " .. (self.ac))
end
function UserClass:takeoff(item,slot)
	if _equip_takeoff(self.id,slot) == 0 then
		self:unsetEquip(item)
		return true
	end
	return false
end
function UserClass:sethit(hit)
	self.hit = hit
	_sethit(self.id,hit)
end
function UserClass:shutdown()
	_logout ( self.id )
end
function UserClass:setdam(dam)
	self.dam = dam
	_setdam(self.id,dam)
end

function UserClass:setmight(might)
	self.might = might
	_setmight(self.id,might)
end

function UserClass:setgrace(h)
	self.grace = h
	_setgrace(self.id,h)
end
function UserClass:setwill(h)
	self.will = h
	_setwill(self.id,h)
end
function UserClass:dialog(img,col,mes,n,p)
	return _dialog(self.id,img,col,mes,false,n,p)
end
function UserClass:ownitem(slot)
	local iobj = _getslotitem(self.id,slot)
	if iobj ~= nil then return ItemClass.get(iobj) end
	return nil
end
function UserClass:equip(slot)
	local iobj = _getequip(self.id,slot)
	if iobj ~= nil then return ItemClass.get(iobj) end
	return nil
end
function UserClass:attack(target,per)
	local weapon
	local per = per or 1
	local dam = self.might*3
	if target == nil or not can_damage(self.id,target.id) then
		return
	end
	weapon = self:equip(0)
	if weapon ~= nil then
		dam = dam + math.random(weapon.db.ldam1,weapon.db.ldam2)
		if weapon.db.script ~= nil then
			dam = weapon.db.script(1,self,weapon,target,dam)
		end
		target:sound(weapon.db.damsound)
	end
	if dam ~= nil then
		target:damage(dam*per,1,self)
	end
end
function UserClass:givemoney(mon)
	if self.db.money + mon > 2000000000 then
		return false
	end
	self:setmoney(self.db.money + mon)
	return true
end
function UserClass:levelup()

	self.db.level = self.db.level + 1
	_setlevel(self.id,self.db.level)
	local hp,mp,might,grace,will,ac = get_jobup(self.db.job,self.db.level)
	self:setmaxhp(self.maxhp + (hp or 0))
	self:setmaxmp(self.maxmp + (mp or 0))
	self:setmight(self.might + (might or 0))
	self:setwill(self.will + (will or 0))
	self:setgrace(self.grace + (grace or 0))
	self:setac(self.ac + (ac or 0))
	self.db.maxhp = self.db.maxhp + (hp or 0)
	self.db.maxmp = self.db.maxmp + (mp or 0)
	self.db.might = self.db.might + (might or 0)
	self.db.grace = self.db.grace + (grace or 0)
	self.db.will = self.db.will + (will or 0)
	self.db.ac = self.db.ac + (ac or 0)
	self:effect(2)
	self:message(3,"레벨이 올랐습니다!")
end
function UserClass:giveexp(exp)

	if self.db.job == 0 then exp = math.min(exp,1200 - self.db.exp)
	else exp = math.min(exp,0xFFFFFFFF - self.db.exp) end
	if exp <= 0 then
		self:message(3,"더 이상 경험치를 얻을 수 없습니다.")
		return
	end
	self.db.exp = self.db.exp + exp

	local needexp = get_needexp(self.db.job,self.db.level) or 0
	local prevexp = get_needexp(self.db.job,self.db.level-1) or 0
	if needexp == 0 then
		needexp = 0xFFFFFFFF
		prevexp = 0
	else
		while needexp <= self.db.exp do
			if needexp == 0 then
				needexp = 0xFFFFFFFF
				prevexp = 0
				break
			end
			self:levelup()
			prevexp = needexp
			needexp = get_needexp(self.db.job,self.db.level) or 0
		end
	end
	prevexp = math.min(prevexp,self.db.exp)
	self:message(3,string.format("경험치가 %d (%d%%) 올랐습니다.",exp,((self.db.exp - prevexp)/(needexp-prevexp))*100))

	_setexp(self.id,self.db.exp,needexp-exp,(self.db.exp-prevexp)/(needexp-prevexp))
end
function UserClass:setmoney(mon)
	mon = math.min(0xFFFFFFFF,mon)
	self.db.money = mon
	_setmoney(self.id,mon)
end
function UserClass:message(tp,mes)
	_msg ( self.id, tp, mes)
end
function UserClass:optSet(opt)
	self.option = OR(self.db.option,opt)
	_set_uoption(self.id, self.db.option)
end
function UserClass:optUnset(opt)
	self.option = AND(self.db.option,NOT(opt))
	_set_uoption(self.id, self.db.option)
end
function UserClass:check_magic(name)
	local m = magic_datas[name]
	if m == nil then return true end
	for k,v in pairs(self.db.magics) do
		if v.dbid == m.code then
			return true
		end
	end
	return false
end
function UserClass:give_magic(name,lv)
	local m = magic_datas[name]
	if m == nil then return end
	return _give_magic(self.id,m.code,lv or 1)
end
function UserClass:notice(s)
	_notice(self.id,s)
end
function UserClass:optCheck(opt)
	return AND(self.db.option,opt) > 0
end
function UserClass:logout()
	if self.group ~= nil then
		self.group:leave(o)
	end
	user_save(self.name)
	users[self.name] = nil
end
function UserClass:optReverse(opt)
	if self:optCheck(opt) then
		self:optUnset(opt)
	else
		self:optSet(opt)
	end
end
function UserClass:checkquest(name)
	return self.db.quest[name] or false
end
function UserClass:setquest(name,c)
	self.db.quest[name] = c
end
function UserClass:setjob(job)
	self.db.job = job
	_setjob(self.id,job)
end
function UserClass:getquestmob(mob)
	return self.killedmob[mob]
end
function UserClass:setquestmob(mob,c)
	self.killedmob[mob] = c
end
function UserClass.create(obj,name)
	if not filecheck("user/"..name..".lua") or users[name] ~= nil then
		_logout(obj)
		return
	else
		if user_datas[id] == nil then
			include("user/" .. name )
		end
	end
	local o = user_datas[name]
	print(o)
	if o == nil then
		_logout(obj)
	end
	local user = UserClass.new()
	user.id = obj
	user.name = name
	user.type = 1
	user.group = nil
	user.db = o
	user.state = o.state or 0
	user.hp = o.hp
	user.automp = false
	user.autohp = false
	user.maxhp = o.maxhp
	user.maxmp = o.maxmp
	user.mp = o.mp
	user.ac = o.ac
	user.might = o.might
	user.grace = o.grace
	user.will = o.will
	user.hit = o.hit
	user.dam = o.dam
	user.eqhp = 0
	user.eqmp = 0
	user.hp_per = 0
	user.connect_time = os.clock()
	user.mp_per = 0
	user.killedmob = {}

	user.falledmagics = {}
	o.quest = o.quest or {}
	if o.magics ~= nil then
		for k,v in pairs(o.magics) do
			_set_magic(obj,k,v.dbid,v.lv or 1)
		end
	else
		o.magics = {}
	end
	if o.ownitems ~= nil then
		for k,v in pairs(o.ownitems) do
			local item = ItemClass.create(v.dbid,v.count)
			item.save = v.save
			_set_ownitem(obj,k,item.id)
		end
	end
	if o.equips ~= nil then
		for k,v in pairs(o.equips) do
			local item = ItemClass.create(v.dbid,1)
			item.save = v.save
			user:setEquip(item)
			_set_equip(obj,item.id)
		end
	end
	local needexp = get_needexp(o.job,o.level) or 0
	local prevexp = get_needexp(o.job,o.level-1) or 0
	if needexp == 0 then
		prevexp = 0
		needexp = 0xFFFFFFFF
	end
	o.exp_reamin = needexp - o.exp
	_set_userdata(obj,name,o.title,o.face,o.hair,o.haircol,
				o.gender,o.nation,o.totem,o.state,o.job,
				o.joblv,o.might,o.will,o.grace,o.level,
				o.ac,o.dam,o.hit,o.personality,o.vote,o.hp,
				o.maxhp,o.mp,o.maxmp,o.money,o.exp,o.exp_reamin,(o.exp-prevexp)*100/(needexp-prevexp),o.option)
	objects[obj] = user
	users[name] = user
	sleep(1)
	if o.m == nil then
		user:warp(_START_MAP_,_START_X_,_START_Y_)
	else
		user:warp(o.m,o.x,o.y)
	end
	if o.side == nil then
		user:setside(2)
	else
		user:setside(o.side)
	end
	LuaTimer.create(_USER_SAVE_TIME_,user_save,user.name)
end

