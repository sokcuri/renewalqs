ShopClass = ShopClass or CreateClass()
function regdb_shop(id,...)
	local t = {...}
	local shop = ShopClass.create(id)
	for k,v in pairs(t) do
		local itm = {}
		local p = string.split(v,":",3)
		if #p >= 2 then
			local idb = getitemdb(p[1])
			if idb ~= nil then
				itm.item = idb
				itm.price = tonumber(p[2])
				itm.sellprice = itm.price
				if itm.price == nil then
					itm.price = idb.price
					itm.sellprice = math.floor(idb.price / 2)
				end
				shop.items[itm.item.name] = itm
			end
		end
	end
	shop:release()
	shops[id] = shop
end
_ShopLoader = {
	shop = {
		args = "i",
		func = regdb_shop
	}
}
function shop_init()
	dbLoader_dofile(_ShopLoader,"db/shop_db.txt")

end
function getshopdb(id)
	return shops[id]
end
shops = shops or {}
function ShopClass.create(id)
	local o = ShopClass.new()
	o.items = {}
	o.id = id
	return o
end
function ShopClass:release()
	local m = {}
	for k,v in pairs(self.items) do
		table.insert(m,v.item.name)
		table.insert(m,v.item.image)
		table.insert(m,v.item.color)
		table.insert(m,v.price)
		table.insert(m,v.item.note)
	end
	self.ilist = m
end
function ShopClass:buy(obj,user,item,count)
	local itm = self.items[item]
	if itm ~= nil then
		if itm.item.max_count < count then
			obj:say(0,obj.db.name .. " : 그렇게나 많이?")
			return
		end
		local money = itm.price * count
		if money > user.db.money then
			obj:say(0,obj.db.name .. " : 돈이 부족하시네요.")
			return
		end
		if user:giveitem2(item,count) then
			user:setmoney(user.db.money - money)
			obj:say(0,string.format("%s : %s %d개를 금전 %d전에 팔았습니다.",obj.db.name,item,count,money))
		else
			obj:say(0,obj.db.name .. " : 장비창을 확인해주세요.")
		end
	else
		obj:say(0,obj.db.name .. " : 그런 물건 안팝니다.")
	end
end
function ShopClass:sell(obj,user,item,count)
	local itm = self.items[item]
	if itm ~= nil then

		local iobj = user:checkitem(item,0)
		count = math.min(count,iobj:count())
		if count < 0 then
			count = iobj:count()
		end
		local money = itm.sellprice * count
		if user:givemoney(money) then
			iobj:setcount(iobj:count() - count)
			obj:say(0,string.format("%s : %s %d개를 %d전에 팔았습니다.",obj.db.name,item,count,money))
		else
			obj:say(0,obj.db.name .. " : 돈을 더 이상 가질 수 없네요.")
		end
	end
end
function ShopClass:callsell(obj,user)
	::first::
	local slotlist = {}
	for i = 0,25 do
		local itm = user:ownitem(i)
		if itm ~= nil and self.items[itm.db.name] ~= nil then
			table.insert(slotlist,i)
		end
	end
	local sel = obj:islotlist(user,"제가 살 수 있는 물건들입니다. 어느 것을 파시겠습니까?",slotlist)
	if sel ~= nil and sel >= 0 then
		local item = user:ownitem(sel)

		if item ~= nil then
			local itm = self.items[item.db.name]
			::reinput::
			local count = 0
			if item.db.max_count > 1 then
				count = tonumber(obj:input(user,"얼마나 파시겠습니까?"))
				if count == nil then
					goto reinput
				end
				count = math.min(count,item:count())
			end
			local money = itm.sellprice * count
			local sel = obj:list(user,string.format("금전 %d전에 드릴 수 있습니다. 파시겠습니까?",money),true,false,"예","아니오")
			if sel == 1 then
				if user:givemoney(money) then
					item:setcount(item:count() - count)
					obj:say(0,string.format("%s : %s %d개를 금전 %d전에 샀습니다.",obj.db.name,item.db.name,count,money))
					goto first
				else
					if obj:dialog(user,"돈을 더 가질 수 없습니다.",true,false) ==2  then
						goto first
					end
				end
			else
				goto first
			end
		end
	end
end
function ShopClass:callbuy(obj,user)
	::first::
	local sel = obj:ilist(user,"제가 팔고 있는 것들입니다. 무엇을 사시겠습니까?",self.ilist)
	if sel ~= nil then
		local itm = self.items[sel]
		if itm ~= nil then
			local count = 1
			if itm.item.max_count > 1 then
				::reinput::
				count = tonumber(obj:input(user,"얼마나 사시겠습니까?"))
				if count == nil then
					goto reinput
				end
			end
			local money = itm.price * count
			local sel = obj:list(user,string.format("금전 %d전에 드릴 수 있습니다. 사시겠습니까?",money),true,false,"예","아니오")
			if sel == 1 then
				if user.db.money < money then
					if obj:dialog(user,"돈이 부족하시네요.",true,false) == 2 then
						goto first
					end
					return
				end
				if user:giveitem2(itm.item.name,count) then
					user:setmoney(user.db.money - money)
					obj:say(0,string.format("%s : %s %d개를 %d전에 팔았습니다.",obj.db.name,itm.item.name,count,money))
					goto first
				else
					if obj:dialog(user,"장비창을 확인해주세요.",true,false) == 2 then
						goto first
					end
					return
				end
			else
				goto first
			end
		end
	end
end



