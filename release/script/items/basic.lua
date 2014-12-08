
-- 임무서
dorumari_quests = {
	{
		level = 1,
		message = "다람쥐 5마리, 토끼 5마리 사냥",
		check = function(user,item)
			local mb1 = user:getquestmob("다람쥐")
			local mb2 = user:getquestmob("토끼")
			if mb1 == nil then
				user:setquestmob("다람쥐",0)
				mb1 = 0
			end
			if mb2 == nil then
				user:setquestmob("토끼",0)
				mb2 = 0
			end
			if mb1 >= 5 and mb2 >= 5 then
				return true
			end
			local msg = string.format("다람쥐 %d/5\n토끼 %d/5",mb1,mb2)
			item:dialog(user,msg,true,false)
			return true
		end,
		money = 500,
		items = {
			{"노란비서",10}
		}
	}
}
function item_dorumari(user,item)
	local quest_name = "임무두루마리"
	local q = user:checkquest(quest_name) or 1
	local quest = dorumari_quests[q]
	if quest ~= nil then
		if user.db.level < quest.level then
			item:dialog(user,"두루마리를 열기 위해 좀 더 수련이 필요합니다",true,false)
			return
		else
			local t= {}
			table.insert(t,quest.message)
			table.insert(t,"\n< 보상 >\n")
			if quest.money then
				table.insert(t,string.format("금전 %d전\n",quest.money))
			end
			for i = 1,#quest.items do
				local itm = quest.items[i]
				if itm ~= nil then
					local idb = getitemdb(itm[1])
					if idb ~= nil then
						table.insert(t,string.format(idb.fmt,idb.name,itm[2]))
						table.insert(t,"\n")
					end
				end
			end
			local inp = item:dialog(user,table.concat(t),true,false)
			if inp == 2 then
				if quest.check(user,item) then
					t = {}
					if quest.money then
						table.insert(t,string.format("금전 %d전\n",quest.money))
						user:givemoney(quest.money)
					end
					for i = 1,#quest.items do
						local itm = quest.items[i]
						local idb = getitemdb(itm[1])
						if idb ~= nil then
							table.insert(t,string.format(idb.fmt,idb.name,itm[2]))
							table.insert(t,"\n")
							user:giveitem2(idb.name,itm[2])
						end
					end
					table.insert(t,"획득!")
					user:setquest(quest_name,q+1)
					item:dialog(user,table.concat(t),true,false)

				end
			end
		end
	else
		item:dialog(user,"있는 임무가 없습니다.",true,false)
	end
end
