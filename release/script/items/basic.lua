
-- �ӹ���
dorumari_quests = {
	{
		level = 1,
		message = "�ٶ��� 5����, �䳢 5���� ���",
		check = function(user,item)
			local mb1 = user:getquestmob("�ٶ���")
			local mb2 = user:getquestmob("�䳢")
			if mb1 == nil then
				user:setquestmob("�ٶ���",0)
				mb1 = 0
			end
			if mb2 == nil then
				user:setquestmob("�䳢",0)
				mb2 = 0
			end
			if mb1 >= 5 and mb2 >= 5 then
				return true
			end
			local msg = string.format("�ٶ��� %d/5\n�䳢 %d/5",mb1,mb2)
			item:dialog(user,msg,true,false)
			return true
		end,
		money = 500,
		items = {
			{"�����",10}
		}
	}
}
function item_dorumari(user,item)
	local quest_name = "�ӹ��η縶��"
	local q = user:checkquest(quest_name) or 1
	local quest = dorumari_quests[q]
	if quest ~= nil then
		if user.db.level < quest.level then
			item:dialog(user,"�η縶���� ���� ���� �� �� ������ �ʿ��մϴ�",true,false)
			return
		else
			local t= {}
			table.insert(t,quest.message)
			table.insert(t,"\n< ���� >\n")
			if quest.money then
				table.insert(t,string.format("���� %d��\n",quest.money))
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
						table.insert(t,string.format("���� %d��\n",quest.money))
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
					table.insert(t,"ȹ��!")
					user:setquest(quest_name,q+1)
					item:dialog(user,table.concat(t),true,false)

				end
			end
		end
	else
		item:dialog(user,"�ִ� �ӹ��� �����ϴ�.",true,false)
	end
end
