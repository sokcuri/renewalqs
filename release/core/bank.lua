
BankClass = BankClass or CreateClass()

function BankClass.create()
	local o = BankClass.new()
	o.items = {}
	o.money = 0
	o.ilist = {}
	return o
end

function BankClass:release()
	local m = {}
	for k,v in pairs(o.items) do
		table.insert(m,v.name)
		table.insert(m,v.image)
		table.insert(m,v.color)
		table.insert(m,v.count)
		table.insert(m,v.note)
	end
end
function call_deposit_item(obj,user)
	local depitems = user.bank.items
	local slots = {}
	for i = 0,25 do
		local iobj = user:ownitem(i)
		if iobj ~= nil then
			table.insert(slots,i)
		end
	end
	local it = obj:islotlist(user,"무엇을 맡기시겠습니까?",slots)
	if it ~= nil then
		local iobj = user:ownitem(i)
		local item = item_datas[getdbid(iobj)]
		local il = user.bank.ilist
		local v = {}
		setimsg(iobj,12)
		local count = 1
		if item.max_count > 1 then
			::reinput::
			local inp = obj:input(user,"얼마나 맡기시겠습니까?")
			if inp == nil then return end
			count = tonumber(inp)
			if count <= 0 then goto reinput end
			count = math.min(count,get_icount(iobj))
		end

	end
	return true
end
