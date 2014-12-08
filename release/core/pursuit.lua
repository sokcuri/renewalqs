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
	["��"] = pursuit_buy,
	["�Ǵ�"] = pursuit_sell,
	["��"] = pursuit_sell,
	["���Ǵ�"] = pursuit_sellall,
	["���"] = pursuit_kidding,
	["�þ�"] = pursuit_deopsit,
	["�þ���"] = pursuit_deposit,
	["��������"] = pursuit_deposit,
	["����"] = pursuit_deposit,
	["����"] = pursuit_withdraw,
	["������"] = pursuit_withdraw,

	["��"] = pursuit_howmuch,
	["�󸶴�"] = pursuit_howmuch,
	["�󸶴�?"] = pursuit_howmuch
}
_FILTERS = {
	["���"] = pursuit_kidding,
	["����ּ���"] = pursuit_reborn,
	["�����մϴ�"] = pursuit_thankyou,
	["���Ĵ�?"] = pursuit_asksell,
	["�� �Ĵ�?"] = pursuit_asksell,
	["�� �Ĵ�"] = pursuit_asksell,
	["���Ĵ�"] = pursuit_asksell,
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
	local itm,count,token = string.match(say,"(%S+)%s+(%d+)��%s+(%S+)")
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


