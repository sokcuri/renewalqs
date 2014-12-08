function npc_job(npc,user,name,job,magics)

	if user.db.job == 0 then
		local sel = npc:menu(user,"험난한 길을 걷는 수행자여,\n무슨 일로 저를 찾으셨소?" ,"직업가지기","다른 길드로 이동하기")
		if sel == 1 then
			if user.db.level < 5 then
				npc:dialog(user,"자네는 수련이 아직 부족하군. 좀 더 수련을 쌓고 오도록.",true,false)
				return
			end
			local sel = npc:menu(user,name .. "의 길을 가려면 몇 가지\n맹세를 해야하느니","예","아니오")
			if sel == 1 then
				local sel = npc:menu(user,"첫째로, 하늘에서 굽어보고 계신\n천제(天帝) 앞에 복종을 맹세하겠느냐?","예","아니오")
				if sel == 1 then
					local sel = npc:menu(user,"둘째로, 험난한 "..name.." 수련의 길에\n너의 평생을 바칠 것을\n맹세하겠느냐?","예","아니오")
					if sel == 1 then
						local sel = npc:menu(user,"셋째로, 불의를 보고 그냥\n지나치지 않을 것을 맹세하겠느냐?", "예", "아니오")
						if sel == 1 then
							user:setjob(job)
							npc:dialog(user,"자네는 이제" .. name .. "로써의 끊없는 길을 걷게 되었네!",true,false)
							npc:dialog(user,"그 길은 멀고도 험난할 테니,\n 마음을 굳건히 하거라.",true,false)
							return
						end
					end
				end
			end
			npc:dialog(user,"그렇다면 " .. name .. "가 될 수 없다네.")
			return
		elseif sel == 2 then
			local sel = npc:menu(user,"어느 길드로 가겠는가?","전사","도적","주술사","도사")
			if sel == 1 then
				user:warp("검성 예백",6,9)
			elseif sel == 2 then
				user:warp("태성 마서",6,9)
			elseif sel == 3 then
				user:warp("현자 처연",6,9)
			elseif sel == 4 then
				user:warp("신선 선후",6,9)
			end
			return
		else return
		end
	elseif user.db.job == job then
		local sel = npc:list(user,"험난한 길을 걷는 수행자여,\n무슨 일로 저를 찾으셨소?",false,true,"마법 배우기","마법 지우기")
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
			local sel = npc:list_t(user,"자, 자네는 이정도 마법을 배울 수 있겠군. 어떤 마법이 궁금한가?",false,true,l)
			if sel > 0 and sel <= #l then
				local v = t[sel]
				local msg = {}
				if #v>= 4 then
					table.insert(msg,v[1] .. " 마법을 배우기 위해서는 ")
					if v[3] > 0 then
						table.insert(msg,"금전 " .. v[3])
						if #v > 3 then
							table.insert(msg,"전과 ")
						else
							table.insert(msg,"전 ")
						end
					end
					for i = 4,#v do
						table.insert(msg,v[i][1])
						table.insert(msg," ")
						table.insert(msg,v[i][2])
						table.insert(msg,"개")
						if i < #v then
							table.insert(msg,", ")
						end
					end
					table.insert(msg,"가 필요하다네. 배워보겠는가? ")
					sel = npc:list(user,table.concat(msg),false,true,"예","아니오")
				else
					sel = npc:list(user,v[1] .. " 마법을 배워보겠는가? 이 마법은 재물이 필요없다네.","예","아니오")
				end
				if sel == 1 then
					if user.db.money < v[3] then
						npc:dialog(user,"돈이 부족하군.",true,false)
						return
					end
					local items = {}
					for i = 4,#v do
						local r = user:checkitem(v[i][1],v[i][2])
						if r == nil then
							npc:dialog(user,"재료가 다 있는지 제대로 확인해주게.",true,false)
							return
						end
						table.insert(items,r)
					end
					if user:give_magic(v[1]) >= 0 then
						user:setmoney(user.db.money - v[3])
						for i = 1,#items do
							items[i]:setcount(items[i]:count() - v[3+i][2])
						end
						npc:dialog(user,"마법을 가르쳐 주었다네.",true,false)
					else
						npc:dialog(user,"마법이 꽉 차서 더 이상 가르쳐 줄 수가 없군.",true,false)
					end
				elseif sel == 2 then
					npc:dialog(user,"준비가 아직 덜 되었나보지? 배우고 싶으면 다시 나에게 찾아오거라.",true,false)
				end
			end
		end
	else
		npc:dialog(user,"무슨 일로 오셨는가?")
	end
	return
end
function npc_warrior(npc,user)
	local magics = {
		{"비영사천문",5,0,{"도토리",10}},
		{"누리의기원",5,0,{"도토리",10}},
		{"성황령",5,0,{"도토리",10}}
	}
	npc_job(npc,user,"전사",1,magics)

end
function npc_thief(npc,user)
	local magics = {
		{"비영사천문",5,0,{"도토리",10}},
		{"누리의기원",5,0,{"도토리",10}},
		{"성황령",5,0,{"도토리",10}}
	}
	npc_job(npc,user,"도적",2,magics)
end
function npc_healer(npc,user)
	local magics = {
		{"비영사천문",5,0,{"도토리",10}},
		{"누리의기원",5,0,{"도토리",10}},
		{"성황령",5,0,{"도토리",10}}
	}
	npc_job(npc,user,"도사",4,magics)
end
function npc_magician(npc,user)
	local magics = {
		{"비영사천문",5,0,{"도토리",10}},
		{"누리의기원",5,0,{"도토리",10}},
		{"성황령",5,0,{"도토리",10}},
		{"불피우기",5,0,{"도토리",10}},
		{"얼리기",12,0,{"도토리",10}},
		{"뢰격",25,0},
		{"풍격",36,0},
		{"폭격",56,0},

	}
	npc_job(npc,user,"주술사",3,magics)
end
function npc_beginner(npc,user)
	local q = "낙랑_노란비서"
	if user:checkquest(q) then
		npc:dialog(user,"이미 노란비서를 받으셧네요. 노란비서를 사용해보세요~")
		return
	end
	::first::
	local sel = npc:dialog(user,"안녕하세요! 바람의나라에 오신것을 환영합니다! 본서버는 RQS 테스트 서버입니다. 노란비서를 드릴게여",true,false)
	if sel == 2 then
		if not user:giveitem2("노란비서",20) then
			npc:dialog(user,"장비창을 확인해주세요.",false,false)
			return
		end
		user:setquest(q,true)
	end
end
function npc_jumo(npc,user)
	::first::
	local sel = npc:list(user,"안녕하세요 무슨일을 도와드릴까요?",false,false,"물건사기","물건팔기")
	if sel == 1 then
		npc.pursuit:callbuy(npc,user)
	elseif sel == 2 then
		npc.pursuit:callsell(npc,user)
	end
end
