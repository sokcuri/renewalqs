PursuitClass = CreateClass()
function pursuit_buy(user,item,count)
	local objs = {_getsightobjects(user.id,31)}
	for k,v in pairs(objs) do
		local obj = objects[v]
		if obj.pursuit ~= nil and obj.pursuit.buy ~= nil then
			obj.pursuit.buy:buy(obj,user,item,count)
		end
	end
end
function pursuit_sellall(user,item)
	local objs = {_getsightobjects(user.id,31)}
	for k,v in pairs(objs) do
		local obj = objects[v]
		if obj.pursuit ~= nil and obj.pursuit.sell ~= nil then
			obj.pursuit.sell:sell(obj,user,item,-1)
		end
	end
end
function pursuit_sell(user,item,count)
	local objs = {_getsightobjects(user.id,31)}
	for k,v in pairs(objs) do
		local obj = objects[v]
		if obj.pursuit ~= nil and obj.pursuit.sell ~= nil then
			obj.pursuit.sell:sell(obj,user,item,count)
		end
	end
end
_ITEM_COUNT_TOKENS = {
	["Áà"] = pursuit_buy,
	["ÆÇ´Ù"] = pursuit_sell,
	["ÆÊ"] = pursuit_sell,
	["´ÙÆÇ´Ù"] = pursuit_sellall,
	["ºñ½Î"] = pursuit_kidding,
	["¸Ã¾Æ"] = pursuit_deopsit,
	["¸Ã¾ÆÁà"] = pursuit_deposit,
	["º¸°üÇØÁà"] = pursuit_deposit,
	["º¸°ü"] = pursuit_deposit,
	["µ¹·Á"] = pursuit_withdraw,
	["µ¹·ÁÁà"] = pursuit_withdraw,

	["¾ó¸¶"] = pursuit_howmuch,
	["¾ó¸¶´Ï"] = pursuit_howmuch,
	["¾ó¸¶´Ï?"] = pursuit_howmuch
}
_FILTERS = {
	["ºñ½Î"] = pursuit_kidding,
	["»ì·ÁÁÖ¼¼¿ä"] = pursuit_reborn,
	["°¨»çÇÕ´Ï´Ù"] = pursuit_thankyou,
	["¹¹ÆÄ´Ï?"] = pursuit_asksell,
	["¹¹ ÆÄ´Ï?"] = pursuit_asksell,
	["¹¹ ÆÄ´Ï"] = pursuit_asksell,
	["¹¹ÆÄ´Ï"] = pursuit_asksell,
}
pursuits = pursuits or {}
PursuitClass = CreateClass()

function getpursuit(id)
	return pursuits[id]
end
function PursuitClass.create(id)
	local o = PursuitClass.new()
	o.id = id
	pursuits[id] = o
	return o
end
function regdb_pursuit(id,buy,sell)
	local pur = PursuitClass.create(id)
	pur.buy = getshopdb(buy)
	pur.sell = getshopdb(sell)
end
function PursuitClass:callbuy(obj,user)
	if self.buy ~= nil then
		self.buy:callbuy(obj,user)
	end
end
function PursuitClass:callsell(obj,user)
	if self.sell ~= nil then
		self.sell:callsell(obj,user)
	end
end
_PursuitLoader = {
	pursuit = {
		args = "sii",
		func = regdb_pursuit
	}
}
function pursuit_init()
	dbLoader_dofile(_PursuitLoader,"db/pursuit_db.txt")
end
function check_pursuit(user,say)
	local itm,count,token = string.match(say,"(%S+)%s+(%d+)°³%s+(%S+)")
	if itm == nil then
		itm,token = string.match(say,"(%S+)%s+(%S+)")
		count = 1
	end
	count = tonumber(count)
	if itm ~= nil then
		local f = _ITEM_COUNT_TOKENS[token]
		if f ~= nil then
			f(user,itm,count)
		end
		return
	end
	local filter = _FILTERS[say]
	if filter ~= nil then
		filter(user)
	end
end


