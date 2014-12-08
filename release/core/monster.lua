mob_datas = mob_datas or {}
_MobLoader = {

	mob = {
		args = "isiiiiiiiiiiif",
		func = function( dbid,name,img,col,tp,mhp,atk1,atk2,ac,sp,exp,mon,mac,scr,...)
			local items = {}
			local t = {...}

			for i = 1,#t do
				local k = string.split(t[i],":",4)
				if #k >= 4 then
					k[1] = tonumber(k[1])
					k[3] = tonumber(k[3])
					k[4] = tonumber(k[4])
					table.insert(items,k)
				end
			end
			regdb_mob(dbid,name,img,col,tp,mhp,atk1,atk2,ac,sp,exp,mon,mac,scr,items)
		end
	},
	line = 0,
	file = nil,
}
_MobSpawnLoader = {

	mobspawn = {
		args = "ssisisii",
		func = function(mob,map,x1,x2,y1,y2,count,time)
			local spawn = {}
			spawn.mob = getmobdb(mob)
			spawn.map = getmap(map)
			if spawn.map ~= nil and spawn.mob ~= nil then
				spawn.x1 = math.min(math.max(x1,0),spawn.map.db.xs-1)

				if x2 == "-" then
					spawn.x2 = spawn.map.db.xs -1
				else
					x2= tonumber(x2) or spawn.map.db.xs-1

					x2 = math.min(math.max(x2,0),spawn.map.db.xs-1)
					spawn.x2 = x2
				end
				spawn.y1 = math.min(math.max(y1,0),spawn.map.db.ys-1)
				if y2 == "-" then
					spawn.y2 = spawn.map.db.ys - 1
				else
					y2 = tonumber(y2) or spawn.map.db.ys-1
					y2 = math.min(math.max(y2,0),spawn.map.db.ys-1)
					spawn.y2 = y2
				end
				spawn.count = count
				spawn.time = time
				LuaTimer.create(0,mob_spawn,spawn)
			end
		end
	},
	line = 0,
	file = nil,
}
function onAI(obj)

end

function getmobdb(name)
	if string.sub(name,1,1) == "@" then
		return mob_datas[string.sub(name,2)]
	else
		return mob_datas[name]
	end
end
function mob_spawn(sp)
	sp.monsters = sp.monsters or {}
	for i = 1, sp.count do
		if sp.monsters[i] == nil or not sp.monsters[i]:check_live() then
			local m = MobClass.create(sp.mob.dbid)
			m:warp(sp.map,math.random(sp.x1,sp.x2),math.random(sp.y1,sp.y2))
			sp.monsters[i] = m
		end
	end
	return sp.time
end

function mob_init()
	dbLoader_dofile(_MobLoader,"db/mob_db.txt")
end
function mobspawn_init()

	dbLoader_dofile(_MobSpawnLoader,"db/mobspawn.txt")
end
function regdb_mob(dbid,name,img,col,tp,mhp,atk1,atk2,ac,sp,exp,mon,mac,scr,items)
	local m = mob_datas[dbid] or {}
	m.dbid = dbid
	m.name = name
	m.image = img + 32768
	m.color = col
	m.type = tp
	m.maxhp = mhp
	m.maxmp = 0
	m.atk1 = atk1
	m.atk2 = atk2
	m.ac = ac
	m.speed = sp
	m.exp = exp
	m.money = mon
	m.magicac = mac
	m.script = _G[scr]
	m.items = items
	mob_datas [m] = m
	mob_datas[ name] = m
	mob_datas[dbid] = m
	_mobdb_set(m.dbid,m.name,m.image,m.color,m.type,m.speed)
end
MobClass = MobClass or CreateClass(ObjectLiveClass)

function MobClass.create(dbid)
	local o = MobClass.new()
	local db = mob_datas[dbid]
	o.dbid = db.dbid
	o.db = db
	o.type = 2
	o.state = 0
	o.id = _new_mobobj(dbid)
	o.falledmagics = {}
	o.hp = db.maxhp
	o.maxhp = db.maxhp
	o.mp = db.maxmp
	o.maxmp = db.maxmp
	o.ac = db.ac
	o.name = db.name
	objects[o.id] = o
	return o
end
function MobClass:die(attacker)
	self.hp = 0
	self:setstate(1)
	if attacker.type == 1 then
		attacker:giveexp(self.db.exp * _GAME_SERVER_EXPRATE_)
	end
	if self.db.money ~= nil and self.db.money > 0 then
		local m,x,y = _getpos(self.id)
		_dropmoney(m,x,y,self.db.money)
	end
	if self.db.items ~= nil then
		for k,v in pairs(self.db.items) do
			print(v[1],v[2])
			if math.random(1,10000) <= v[1]*100 then

				if v[2] == "±ÝÀü" then
					local m,x,y = _getpos(self.id)
					_dropmoney(m,x,y,math.random(v[3],v[4]))
				else
					local item = item_datas[v[2]]

					if item ~= nil then
						local obj = ItemClass.create(item.dbid,math.random(v[3],v[4]))
						obj:warp(_getpos(self.id))
					end
				end
			end
		end
	end
end
function MobClass:attack(target)
	local atk = math.random(self.db.atk1,self.db.atk2)
	print(atk)
	self:action(1,20,0)
	if self.atksound ~= nil then
		self:sound(self.atksound,1)
	end
	target:damage(atk,1,self)
end
