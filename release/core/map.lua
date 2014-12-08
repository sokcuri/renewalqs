map_datas = map_datas or {}
maps = maps or {}
function onLocation(user,id)
	local u = users[user]
	if u ~= nil then
		if loc_datas[id] ~= nil and loc_datas[id].func ~= nil then
			loc_datas[id].func(u)
		end
	end
end
function getmap(name)
	if string.sub(name,1,1) == "@" then
		return maps[tonumber(string.sub(name,2))]
	else
		return maps[name]
	end
end
function map_init()
	dbLoader_dofile(_MapLoader,"db/map_db.txt")
	dbLoader_dofile(_EpfLoader,"db/epf_db.txt")
	dbLoader_dofile(_WarpLoader,"db/warp_db.txt")
end
loc_datas = loc_datas or {}
function regdb_loc(map,x,y,f)
	local l = {}
	l.m = getmap(map)
	if l.m == nil then return end
	l.x = x
	l.y = y
	l.func = f
	l.id = #loc_datas + 1
	_locscript_set(l.id,l.m.id,x,y)

end
function regdb_warp(map,x,y,map2,x2,y2,minlv,maxlv)
	local m = maps[map]
	local m2 = maps[map2]
	if m ~= nil and m2 ~= nil then
		_warp_set(m.id,x,y,m2.id,x2,y2,2,0,0)
		_set_autoeffect(m.id,x,y,364)
	end
end
function regdb_wmap(map,x,y,epf,id,minlv,maxlv)
	local m = maps[map]
	local e = epf_datas[epf]
	if m ~= nil and e ~= nil then
		_wmap_set(m.id,x,y,e.id,id,minlv,0)
		_set_autoeffect(m.id,x,y,364)
	end
end
epf_datas = epf_datas or {}
function regdb_epf(id,name,gcnt)
	local e = epf_datas[id] or {}
	e.id = id
	e.name = name
	e.group_count = gcnt
	e.groups = {}
	for i = 1,gcnt do
		e.groups[i-1] = {}
	end
	_set_epfdb(id,name,gcnt)
	epf_datas[id] = e
	epf_datas[name] = e
	epf_datas[e] = e
	_EpfLoader[name] = {
		args = "iiiissii",
		func = function (g,id,wx,wy,name,map,x,y)
			local group = e.groups[g]
			if group ~= nil then
				local m = getmap(map)
				if m ~= nil then
					local wmap = {}
					wmap.name = name
					wmap.id = id
					wmap.wx = wx
					wmap.wy = wy
					wmap.map = m
					wmap.x = x
					wmap.y = y
					group[id] = wmap
					group[name] = wmap
					_set_wmapdb(e.id,g,id,wx,wy,name,m.id,x,y)
				else
					return "맵을 찾을 수 없습니다 - " .. map
				end
			end
		end
	}
end
function regdb_map(id,pid,name,mapfile,blockfile,bgm,effect)
	local m = map_datas[id]
	bgm = bgm or 0
	effect = effect or 0
	if _mapdb_set(id,name,mapfile,blockfile,bgm,effect) then
		m = m or {}
		m.id = id
		m.pid = pid
		m.name = name
		m.bgm = bgm
		m.effect = effect
		map_datas[id] = m
		map_datas[name] = m
		map_datas[m] = m
		maps[id] = MapClass.create(id,id)
		maps[name] = maps[id]
		maps[maps[id]] = maps[id]

		m.xs ,m.ys = _get_mapsize(id)
	end
end

_MapLoader = {
	map = {
		args = "iisssii",
		func = regdb_map
	},
	line = 0,
	file = nil,
}
_WarpLoader = {

	loc = {
		args = "siif",
		func = regdb_loc
	},
	wmap = {
		args = "siisiii",
		func = regdb_wmap
	},
	warp = {
		args = "siisiiii",
		func = regdb_warp
	},
	line = 0,
	file = nil,
}
_EpfLoader = {

	epf = {
		args = "isi",
		func = regdb_epf
	},
	line = 0,
	file = nil,
}
MapClass = MapClass or CreateClass()

function MapClass.create(id,myid)
	local o = MapClass.new()
	_map_set(id,myid)
	o.id = myid
	o.dbid = id
	o.db = map_datas[id]
	o.name = o.db.name
	return o
end
function MapClass:check(x,y)
	return x >= 0 and y >= 0 and x < self.db.xs and y < self.db.ys
end
function MapClass:parent()
	return maps[self.db.pid]
end
function MapClass:message(tp,msg)
	_map_message(self.id,tp,msg)
end
function MapClass:message2(fc,bc,msg,tmout)
	_map_message2(self.id,fc,bc,msg,tmout)
end
function MapClass:canmove(x,y)
	return _can_move(self.id,x,y)
end
function MapClass:effect(x,y,eff)

	_effect_xy(self.id,x,y,eff,0)

end
