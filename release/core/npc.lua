include ( "script/npc" )
npc_datas = npc_datas or {}
function npc_init()

	dbLoader_dofile(_NpcLoader,"db/npc_db.txt")
end
function regdb_npc(id,name,img,col,scr)
	local n = npc_datas[id] or {}
	n.dbid = id
	n.name = name
	n.image = img + 32768
	n.color = col
	n.script = _G[scr]
	_npcdb_set(n.dbid,n.name,n.image,n.color)
	npc_datas[n.dbid] = n
	npc_datas[n.name] = n
	npc_datas[n] = n
end
_NpcLoader = {

	npc = {
		args = "isiif",
		func = regdb_npc
	},
}
_NpcSpawnLoader = {

	npcspawn = {
		args = "ssiiis",
		func = function( name, map, x,y,side,p)
			local npc = newNpc(name)
			if npc ~= nil then
				npc:warp(map,x,y)
				npc:setside(side)
			end
			npc.pursuit = getpursuit(p)
		end
	},
	line = 0,
	file = nil,
}
function getnpcdb(name)
	if string.sub(name,1,1) == "@" then
		return npc_datas[tonumber(string.sub(name,2))]
	else
		return npc_datas[name]
	end
end
function npcspawn_init()

	dbLoader_dofile(_NpcSpawnLoader,"db/npcspawn.txt")
end
NpcClass = NpcClass or CreateClass(ObjectLiveClass)
function NpcClass:input(user,mes)
	return _input(user.id,self.db.image,self.db.color,mes)
end
function NpcClass:dialog(user,mes,n,p)
	return _dialog(user.id,self.db.image,self.db.color,mes,false,n or false,p or false)
end
function NpcClass:list_t(user,mes,n,p,t)
	return _list(user.id,self.db.image,self.db.color,mes,false,n,p,unpack(t))
end
function NpcClass:list(user,mes,n,p,...)
	return _list(user.id,self.db.image,self.db.color,mes,false,n,p,...)
end
function NpcClass:islotlist(user,mes,t)
	print(unpack(t))
	return _islotlist(user.id,self.db.image,self.db.color,mes,unpack(t))
end
function NpcClass:menu(user,mes,...)
	return _menu(user.id,self.db.image,self.db.color,mes,...)
end
function newNpc(name)
	local db = npc_datas[name]
	return NpcClass.create(db.dbid)
end
function NpcClass.create(dbid)

	local o = NpcClass.new()
	local db = npc_datas[dbid]
	o.dbid = db.dbid
	o.db = db
	o.state = 0
	o.type = 8
	o.falledmagics = {}
	o.id = _new_npcobj(dbid)
	o.name = db.name
	objects[o.id] = o
	return o
end
