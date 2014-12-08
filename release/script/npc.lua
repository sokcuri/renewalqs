function npc_job(npc,user,name,job,magics)

	if user.db.job == 0 then
		local sel = npc:menu(user,"�賭�� ���� �ȴ� �����ڿ�,\n���� �Ϸ� ���� ã���̼�?" ,"����������","�ٸ� ���� �̵��ϱ�")
		if sel == 1 then
			if user.db.level < 5 then
				npc:dialog(user,"�ڳ״� ������ ���� �����ϱ�. �� �� ������ �װ� ������.",true,false)
				return
			end
			local sel = npc:menu(user,name .. "�� ���� ������ �� ����\n�ͼ��� �ؾ��ϴ���","��","�ƴϿ�")
			if sel == 1 then
				local sel = npc:menu(user,"ù°��, �ϴÿ��� ����� ���\nõ��(���) �տ� ������ �ͼ��ϰڴ���?","��","�ƴϿ�")
				if sel == 1 then
					local sel = npc:menu(user,"��°��, �賭�� "..name.." ������ �濡\n���� ����� ��ĥ ����\n�ͼ��ϰڴ���?","��","�ƴϿ�")
					if sel == 1 then
						local sel = npc:menu(user,"��°��, ���Ǹ� ���� �׳�\n����ġ�� ���� ���� �ͼ��ϰڴ���?", "��", "�ƴϿ�")
						if sel == 1 then
							user:setjob(job)
							npc:dialog(user,"�ڳ״� ����" .. name .. "�ν��� ������ ���� �Ȱ� �Ǿ���!",true,false)
							npc:dialog(user,"�� ���� �ְ� �賭�� �״�,\n ������ ������ �ϰŶ�.",true,false)
							return
						end
					end
				end
			end
			npc:dialog(user,"�׷��ٸ� " .. name .. "�� �� �� ���ٳ�.")
			return
		elseif sel == 2 then
			local sel = npc:menu(user,"��� ���� ���ڴ°�?","����","����","�ּ���","����")
			if sel == 1 then
				user:warp("�˼� ����",6,9)
			elseif sel == 2 then
				user:warp("�¼� ����",6,9)
			elseif sel == 3 then
				user:warp("���� ó��",6,9)
			elseif sel == 4 then
				user:warp("�ż� ����",6,9)
			end
			return
		else return
		end
	elseif user.db.job == job then
		local sel = npc:list(user,"�賭�� ���� �ȴ� �����ڿ�,\n���� �Ϸ� ���� ã���̼�?",false,true,"���� ����","���� �����")
		if sel == 1 then
			local l = {}
			local t = {}
			for k,v in pairs(magics) do
				if user.db.level < v[2] then
					break
				end
				if not user:check_magic(v[1]) then
					table.insert(l,v[1])
					t[#l] = v
				end
			end
			local sel = npc:list_t(user,"��, �ڳ״� ������ ������ ��� �� �ְڱ�. � ������ �ñ��Ѱ�?",false,true,l)
			if sel > 0 and sel <= #l then
				local v = t[sel]
				local msg = {}
				if #v>= 4 then
					table.insert(msg,v[1] .. " ������ ���� ���ؼ��� ")
					if v[3] > 0 then
						table.insert(msg,"���� " .. v[3])
						if #v > 3 then
							table.insert(msg,"���� ")
						else
							table.insert(msg,"�� ")
						end
					end
					for i = 4,#v do
						table.insert(msg,v[i][1])
						table.insert(msg," ")
						table.insert(msg,v[i][2])
						table.insert(msg,"��")
						if i < #v then
							table.insert(msg,", ")
						end
					end
					table.insert(msg,"�� �ʿ��ϴٳ�. ������ڴ°�? ")
					sel = npc:list(user,table.concat(msg),false,true,"��","�ƴϿ�")
				else
					sel = npc:list(user,v[1] .. " ������ ������ڴ°�? �� ������ �繰�� �ʿ���ٳ�.","��","�ƴϿ�")
				end
				if sel == 1 then
					if user.db.money < v[3] then
						npc:dialog(user,"���� �����ϱ�.",true,false)
						return
					end
					local items = {}
					for i = 4,#v do
						local r = user:checkitem(v[i][1],v[i][2])
						if r == nil then
							npc:dialog(user,"��ᰡ �� �ִ��� ����� Ȯ�����ְ�.",true,false)
							return
						end
						table.insert(items,r)
					end
					if user:give_magic(v[1]) >= 0 then
						user:setmoney(user.db.money - v[3])
						for i = 1,#items do
							items[i]:setcount(items[i]:count() - v[3+i][2])
						end
						npc:dialog(user,"������ ������ �־��ٳ�.",true,false)
					else
						npc:dialog(user,"������ �� ���� �� �̻� ������ �� ���� ����.",true,false)
					end
				elseif sel == 2 then
					npc:dialog(user,"�غ� ���� �� �Ǿ�������? ���� ������ �ٽ� ������ ã�ƿ��Ŷ�.",true,false)
				end
			end
		end
	else
		npc:dialog(user,"���� �Ϸ� ���̴°�?")
	end
	return
end
function npc_warrior(npc,user)
	local magics = {
		{"�񿵻�õ��",5,0,{"���丮",10}},
		{"�����Ǳ��",5,0,{"���丮",10}},
		{"��Ȳ��",5,0,{"���丮",10}}
	}
	npc_job(npc,user,"����",1,magics)

end
function npc_thief(npc,user)
	local magics = {
		{"�񿵻�õ��",5,0,{"���丮",10}},
		{"�����Ǳ��",5,0,{"���丮",10}},
		{"��Ȳ��",5,0,{"���丮",10}}
	}
	npc_job(npc,user,"����",2,magics)
end
function npc_healer(npc,user)
	local magics = {
		{"�񿵻�õ��",5,0,{"���丮",10}},
		{"�����Ǳ��",5,0,{"���丮",10}},
		{"��Ȳ��",5,0,{"���丮",10}}
	}
	npc_job(npc,user,"����",4,magics)
end
function npc_magician(npc,user)
	local magics = {
		{"�񿵻�õ��",5,0,{"���丮",10}},
		{"�����Ǳ��",5,0,{"���丮",10}},
		{"��Ȳ��",5,0,{"���丮",10}},
		{"���ǿ��",5,0,{"���丮",10}},
		{"�󸮱�",12,0,{"���丮",10}},
		{"�ڰ�",25,0},
		{"ǳ��",36,0},
		{"����",56,0},

	}
	npc_job(npc,user,"�ּ���",3,magics)
end
function npc_beginner(npc,user)
	local q = "����_�����"
	if user:checkquest(q) then
		npc:dialog(user,"�̹� ����񼭸� �����˳׿�. ����񼭸� ����غ�����~")
		return
	end
	::first::
	local sel = npc:dialog(user,"�ȳ��ϼ���! �ٶ��ǳ��� ���Ű��� ȯ���մϴ�! �������� RQS �׽�Ʈ �����Դϴ�. ����񼭸� �帱�Կ�",true,false)
	if sel == 2 then
		if not user:giveitem2("�����",20) then
			npc:dialog(user,"���â�� Ȯ�����ּ���.",false,false)
			return
		end
		user:setquest(q,true)
	end
end
function npc_jumo(npc,user)
	::first::
	local sel = npc:list(user,"�ȳ��ϼ��� �������� ���͵帱���?",false,false,"���ǻ��","�����ȱ�")
	if sel == 1 then
		npc.pursuit:callbuy(npc,user)
	elseif sel == 2 then
		npc.pursuit:callsell(npc,user)
	end
end
