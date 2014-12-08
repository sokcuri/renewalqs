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
			obj:say(0,obj.db.name .. " : �׷��Գ� ����?")
			return
		end
		local money = itm.price * count
		if money > user.db.money then
			obj:say(0,obj.db.name .. " : ���� �����Ͻó׿�.")
			return
		end
		if user:giveitem2(item,count) then
			user:setmoney(user.db.money - money)
			obj:say(0,string.format("%s : %s %d���� ���� %d���� �Ⱦҽ��ϴ�.",obj.db.name,item,count,money))
		else
			obj:say(0,obj.db.name .. " : ���â�� Ȯ�����ּ���.")
		end
	else
		obj:say(0,obj.db.name .. " : �׷� ���� ���˴ϴ�.")
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
			obj:say(0,string.format("%s : %s %d���� %d���� �Ⱦҽ��ϴ�.",obj.db.name,item,count,money))
		else
			obj:say(0,obj.db.name .. " : ���� �� �̻� ���� �� ���׿�.")
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
	local sel = obj:islotlist(user,"���� �� �� �ִ� ���ǵ��Դϴ�. ��� ���� �Ľðڽ��ϱ�?",slotlist)
	if sel ~= nil and sel >= 0 then
		local item = user:ownitem(sel)

		if item ~= nil then
			local itm = self.items[item.db.name]
			::reinput::
			local count = 0
			if item.db.max_count > 1 then
				count = tonumber(obj:input(user,"�󸶳� �Ľðڽ��ϱ�?"))
				if count == nil then
					goto reinput
				end
				count = math.min(count,item:count())
			end
			local money = itm.sellprice * count
			local sel = obj:list(user,string.format("���� %d���� �帱 �� �ֽ��ϴ�. �Ľðڽ��ϱ�?",money),true,false,"��","�ƴϿ�")
			if sel == 1 then
				if user:givemoney(money) then
					item:setcount(item:count() - count)
					obj:say(0,string.format("%s : %s %d���� ���� %d���� ����ϴ�.",obj.db.name,item.db.name,count,money))
					goto first
				else
					if obj:dialog(user,"���� �� ���� �� �����ϴ�.",true,false) ==2  then
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
	local sel = obj:ilist(user,"���� �Ȱ� �ִ� �͵��Դϴ�. ������ ��ðڽ��ϱ�?",self.ilist)
	if sel ~= nil then
		local itm = self.items[sel]
		if itm ~= nil then
			local count = 1
			if itm.item.max_count > 1 then
				::reinput::
				count = tonumber(obj:input(user,"�󸶳� ��ðڽ��ϱ�?"))
				if count == nil then
					goto reinput
				end
			end
			local money = itm.price * count
			local sel = obj:list(user,string.format("���� %d���� �帱 �� �ֽ��ϴ�. ��ðڽ��ϱ�?",money),true,false,"��","�ƴϿ�")
			if sel == 1 then
				if user.db.money < money then
					if obj:dialog(user,"���� �����Ͻó׿�.",true,false) == 2 then
						goto first
					end
					return
				end
				if user:giveitem2(itm.item.name,count) then
					user:setmoney(user.db.money - money)
					obj:say(0,string.format("%s : %s %d���� %d���� �Ⱦҽ��ϴ�.",obj.db.name,itm.item.name,count,money))
					goto first
				else
					if obj:dialog(user,"���â�� Ȯ�����ּ���.",true,false) == 2 then
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



