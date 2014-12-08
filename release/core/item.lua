include("script/item")
money_items = {
	{
		dbid = 10000,
		name = "¿±Àü",
		fmt = "%s(%dÀü)",
		max_count = 1,
		type = 19,
		option = 0,
		image = 23+49151,
		color = 0
	},
	{
		dbid = 10001,
		name = "¿±Àü¹¶Ä¡",
		fmt = "%s(%dÀü)",
		max_count = 50,
		type = 19,
		option = 0,
		image = 74+49151,
		color = 0
	},
	{
		dbid = 10002,
		name = "ÀºÀü",
		fmt = "%s(%dÀü)",
		max_count = 100,
		type = 19,
		option = 0,
		image = 70+49151,
		color = 0
	},
	{
		dbid = 10003,
		name = "ÀºÀü¹¶Ä¡",
		fmt = "%s(%dÀü)",
		max_count = 500,
		type = 19,
		option = 0,
		image = 73+49151,
		color = 0
	},
	{
		dbid = 10004,
		name = "±ÝÀü",
		fmt = "%s(%dÀü)",
		max_count = 1000,
		type = 19,
		option = 0,
		image = 71+49151,
		color = 0
	},
	{
		dbid = 10005,
		name = "±ÝÀü¹¶Ä¡",
		fmt = "%s(%dÀü)",
		max_count = 5000,
		type = 19,
		option = 0,
		image = 72+49151,
		color = 0
	},
}
function item_init()
	for k,v in pairs(money_items) do
		itemdb_add(v)
	end
	dbLoader_dofile(_ItemLoader,"db/item_db.txt")

end
function regdb_use(id,tp,name,fmt,img,col,max,price,opt,scr,note)
	local item = item_datas[id] or {}
	item.dbid = id
	item.type = tp
	item.price = price
	item.image = img + 49152
	item.color = col
	item.name = name
	item.fmt = fmt
	item.max_count = max
	item.option = opt
	item.script = _G[scr]
	item.note = note
	itemdb_add(item)
end
function getitemdb(name)
	if string.sub(name,1,1) == "@" then
		return item_datas[tonumber(string.sub(name,2))]
	else
		return item_datas[name]
	end
end
item_datas = item_datas or {}
if formats == nil then
	formats = {}
	formats.lastid = 1
end
function regdb_equip(id,type,name,img,col,eqimg,eqcol,eqcol2,eqcol3,eqopt,ac,opt,might,grace,will,hp,mp,hp_per,mp_per,job,lv,joblv,hit,dam,sdam1,sdam2,ldam1,ldam2,atksnd,damsnd,price,scr,note)
	local item = item_datas[id] or {}
	item.dbid = id
	item.name = name
	item.image = img + 49152
	item.type = type
	item.fmt = "%s"
	item.max_count = 1
	item.color = col or 0
	item.equip_img = eqimg or -1
	item.equip_col = eqcol or 0
	item.equip_col2 = eqcol2 or -1
	item.equip_col3 = eqcol3 or -1
	item.equip_opt = eqopt or 0
	item.ac = ac or 0
	item.option = opt or 2
	item.price = price
	item.might = might or 0
	item.grace = grace or 0
	item.will = will or 0
	item.hp = hp or 0
	item.mp = mp or 0
	item.hp_per = hp_per or 0
	item.mp_per = mp_per or 0
	item.job = job or 0
	item.level = lv or 0
	item.joblv = joblv or 0
	item.note = note
	item.hit = hit or 0
	item.dam = dam or 0
	item.atksound = atksnd or 0
	item.damsound = damsnd or 0
	item.script = _G[scr]
	item.sdam1 = sdam1 or 0
	item.sdam2 = sdam2 or 0
	item.ldam1 = ldam1 or 0
	item.ldam2 = ldam2 or 0
	itemdb_add(item)
end
function itemdb_add(item)
	if formats[item.fmt] == nil then
		_sformat_set(formats.lastid,item.fmt)
		formats[item.fmt] = formats.lastid
		formats.lastid = formats.lastid+1
	end
	_itemdb_set(item.dbid,item.type,item.name,formats[item.fmt],item.image,item.color,item.max_count,item.option)
	if item.type < 14 then
		_itemdb_set_equip(item.dbid,item.equip_img,item.equip_col,item.equip_col2,item.equip_col3,item.equip_opt,item.atksound)

	end
	item_datas[item.dbid] = item
	item_datas[item.name] = item
	item_datas[item] = item
end
function onTakeon(obj,iobj,slot)
	local u = objects[obj]
	local d = true
	local i = ItemClass.get(iobj)
	if i~= nil then
		if i.db.script ~= nil then
			d = i.db.script(2,u,item,slot)
			d = d or true
		end
		if d then
			u:takeon(i,slot)
		end
	end
end
function onTakeoff(obj,iobj,slot)
	local u = objects[obj]
	local d = true
	local i = ItemClass.get(iobj)
	if i~= nil then
		if i.db.script ~= nil then
			d = i.db.script(3,u,item,slot)
			d = d or true
		end
		if d then
			u:takeoff(i,slot)
		end
	end
end
function onDropMoney(obj,money)
	local ud = objects[obj]
	ud.money = ud.money - money
end
ItemClass = ItemClass or CreateClass(ObjectClass)
function ItemClass.create(dbid,count)
	local o = ItemClass.new()
	o.type = 4
	local db = item_datas[dbid]
	o.dbid = db.dbid
	o.db = db
	o.id = _new_itemobj(dbid)

	o.endurance = db.endurance
	_set_icount(o.id,count or 1)
	objects[o.id] = o
	return o
end
function _food_script(user,item)
	if item.db.hp then
		user:sethp(user.hp + item.db.hp)
	end
	if item.db.mp then
		user:setmp(user.mp + item.db.mp)
	end
	_set_imsg(item.id,2)
	user:action(8,39,0)
	user:sound(6)
end
function regdb_food(id,name,img,col,mx,hp,mp,price,note)
	local item = item_datas[id] or {}
	item.hp = hp
	item.type = 20
	item.mp = mp
	item.max_count = mx
	item.fmt = "%s %d°³"
	item.option = 0
	item.dbid = id
	item.price = price
	item.note = note
	item.name = name
	item.image = img + 49152
	item.color = col
	item.script = _food_script
	itemdb_add(item)
end

_ItemLoader = {

	use = {
		args = "iissiiiiif",
		func = regdb_use
	},
	equip = {
		args = "iisiiiiiiiiissssisssiiif",
		func = function(id,tp,name,img,col,eqimg,eqcol1,eqcol2,eqcol3,eqopt,ac,opt,up_mgw,up_hpmp,up_hpmp_per,job,lv,hitdam,sdam,ldam,atksnd,damsnd,price,scr,note)
			local k = string.split(up_mgw,":",3)
			local might = 0
			local grace = 0
			local will = 0
			local uphp = 0
			local upmp = 0
			local uphp_per = 0
			local upmp_per = 0
			if #k>=3 then
				might = tonumber(k[1])
				will = tonumber(k[2])
				grace = tonumber(k[3])
			end
			local k = string.split(up_hpmp,":",2)
			if #k>=2  then
				uphp = tonumber(k[1])
				upmp = tonumber(k[2])
			end
			local k  =string.split(up_hpmp_per,":",2)
			if #k>=2 then
				uphp_per = tonumber(k[1])
				upmp_per = tonumber(k[2])
			end
			local sdam1 = 0
			local sdam2 = 0
			local ldam1 = 0
			local ldam2 = 0
			local k = string.split(sdam,":",2)
			if #k >=2 then
				sdam1 = tonumber(k[1])
				sdam2 = tonumber(k[2])
			end
			local k = string.split(ldam,":",2)
			if #k >=2 then
				ldam1 = tonumber(k[1])
				ldam2 = tonumber(k[2])
			end
			local joblv = 0
			local k = string.split(job,":",2)
			if #k >= 2 then
				job = tonumber(k[1])
				joblv = tonumber(k[2])
			end
			local dam = 0
			local hit = 0
			local k = string.split(hitdam,":",2)
			if #k >= 2 then
				hit = tonumber(k[1])
				dam = tonumber(k[2])
			end
			regdb_equip(id,tp,name,img,col,eqimg,eqcol1,eqcol2,eqcol3,eqopt,ac,opt,might,grace,will,hp,mp,hp_per,mp_per,job,lv,joblv,hit,dam,sdam1,sdam2,ldam1,ldam2,atksnd,damsnd,price,scr,note)

		end
	},
	food = {
		args = "isiiiiii",
		func = regdb_food
	},
	line = 0,
	file = nil,
}

function ItemClass.get(obj)
	local item = objects[obj]
	if item == nil then
		item = ItemClass.new()
		item.dbid = _getdbid(obj)
		item.type = 4
		local db = item_datas[item.dbid]
		item.db = db
		item.id = obj
		objects[obj] = item
	end
	return item
end
function onPickupMoney(user,mon)
	local ud = objects[user]
	ud.db.money = ud.db.money + mon
end
function onUseItem(user,slot,iobj)
	local ud = objects[user]
	local d = true
	local item = ItemClass.get(iobj)
	_set_lastobject(user,iobj)
	if item ~= nil and item.db.script ~= nil then
		d = item.db.script(ud,item) or true
	end
	if d and item.db.type == 20 then
		item:setcount(item:count()-1)
	end
end
function give_item3(u,itemid,count)
	local r
	local item = ItemClass.create(itemid,count)
	if item ~= nil then
		r = _give_item(u,item.id,false)
		if r ~= 0 then
			item:delete()
		end
		return r
	end
	return nil
end
function ItemClass:setcount(c)
	return _set_icount(self.id,c)
end
function ItemClass:count()
	return _get_icount(self.id)
end
function ItemClass:owner()
	return nil
end
function ItemClass:slot()
	return _getslot(self.id)
end

