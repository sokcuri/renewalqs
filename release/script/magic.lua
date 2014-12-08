include ( "script/magics/basic")

biyoung_warp = {
	["�ο���"] = {
		{132,136,88,90},
		{4,8,88,90},
		{75,78,144,148},
		{72,75,13,18}
	},
	["12����������"] = {
		{49,52,31,32},
		{7,10,21,24},
		{28,31,49,52},
		{28,31,10,13},
	},

}
function magic_effect(obj,mg,lv)
	local m,x,y = obj:getpos()
	for i = 1,4 do
		local x1 = x-i
		local x2 = x+i
		local y1 = y-i
		local y2 = y+i
		local k = y
		for j = x1,x do
			if m:check(j,y + j-x1) then
				m:effect(j,y + j -x1,43)
				obj:sound(54)
			end
		end
		for j = x+1,x2 do
			if m:check(j,y2 - j+x) then
				m:effect(j,y2 - j + x,43)
				obj:sound(54)
			end
		end
		for j = x2-1,x,-1 do
			if m:check(j,y + j - x2) then

				m:effect(j,y + j - x2,43)
				obj:sound(54)
			end
		end
		for j = x-1,x1,-1 do
			if m:check(j,y1 - j+x) then
				m:effect(j,y1 - j + x,43)
				obj:sound(54)
			end
		end
	end
end
function magic_effect2(obj,mg,lv)
	local m,x,y = obj:getpos()
	local side = obj:side()
	m.xs = m.xs + 3
end
function _magic_target(obj,mg,lv,target)
	local sub = mg.sub
	if obj.mp < sub[5] then
		obj:message(3,"������ �����մϴ�.")
		return
	elseif obj.hp < sub[4] then
		obj:message(3,"ü���� �����մϴ�.")
		return
	end
	if obj:can_damage(target) then
		local grace = obj.grace or 0
		target:effect(sub[1],sub[2])
		target:damage((sub[3]),2,obj)
		obj:delmp(sub[5])
		obj:delhp(sub[4])
		obj:action(6,20,0)
		obj:message(3,mg.name .. "��(��) �ܿ����ϴ�.")
	else
		obj:message(3,"�ɸ��� �ʽ��ϴ�.")
	end
end
function _magic_curses(obj,mg,lv,target)
	local sub = mg.sub
	if obj.mp < sub[4] then
		obj:message(3,"������ �����մϴ�.")
		return
	elseif check_state(obj) then
		if obj:can_damage(target) then
			local m = target:checkfall("����")
			if m == nil or m < sub[6] then
				target:disappear_magic("����")
				obj:delmp(sub[4])
				obj:action(6,30,0)
				target:effect(sub[1],sub[2])
				obj:message(3, mg.name .. "��(��) �ܿ����ϴ�.")
				target:magicfall2("����",mg.name,sub[6],sub[5],sub[3],obj,mg.name)
			else
				obj:message(3,"�̹� �������� Ȥ�� ���޸����� �ɷ��ֽ��ϴ�.")
			end
		else
			obj:message(3,"�ɸ��� �ʽ��ϴ�.")
		end
	end
end
function _fall_curse(target,ac,obj,name)
	target:setac(target.ac + ac)
	if target.type == 1 then
		target:message(3,string.format("%s��(��) %s��(��) ���մϴ�.",obj.name,name))
	end
end

function _unfall_curse(target,ac,obj,name)
	target:setac(target.ac - ac)
end

function check_state(obj)
	if obj.state == 0 or obj.state == 2 or obj.state == 4	then
		return true
	elseif obj.type == 1 then
		if obj.state == 1 then
			obj:message(3,"���� ���¿����� �� �� �����ϴ�.")
		elseif obj.state == 3 then
			obj:message(3,"���� Ÿ�� �� �� �����ϴ�.")
		end
	end
	return false
end
function magic_sunghwang(obj,mg,lv,inp)
	return
end
function magic_biyoung(obj,mg,lv,inp)
	local direction = 0
	if inp == "��" or inp == "1" then direction = 1
	elseif inp == "��" or inp == "2" then direction = 2
	elseif inp == "��" or inp == "3" then direction = 3
	elseif inp == "��" or inp == "4" then direction = 4
	else return
	end
	if direction ~= nil and direction <= 4 then
		local m,x,y = obj:getpos()
		local pmap = m:parent()
		if pmap ~= nil then
			local b = biyoung_warp[pmap.name]
			if b ~= nil then
				local c = b[direction]
				obj:warp(pmap, math.random(c[1],c[2]),math.random(c[3],c[4]),279)
			end
		end
	end
	obj:action(6,20,0)
	obj:sound(1)
end
function fall_heal(obj)
	obj:healhp(math.random(300000,500000))
	obj:effect(5,3)
	return 1000
end
function fall_protect(obj,user)
	obj:effect(110,8)
	obj:setac(obj.ac - 30)
	if obj.type == 1 and obj ~= user then
		obj:message(3,user.name .. "���� ������ �ܿ��ּ̽��ϴ�.")
	end
end
function fall_unprotect(obj)
	obj:setac(obj.ac + 30)
end
function magic_protect(obj,mg,lv,target)
	if obj.mp < 30  then
		if obj.type == 1 then
			obj:message(3,"������ �����մϴ�.")
		end
		return
	end
	target:magicfall("��ȣ",1,120,obj)
	obj:action(6,30,0)
	obj:message(3,"��ȣ�� �ܿ����ϴ�.")
end
function magic_healing(obj)
	obj:action(6,20,0)
	obj:healhp(math.random(300000,500000))
	obj:effect(5,3)
	if obj.type == 1 then
		obj:message(3,"ȸ���� �ܿ����ϴ�.")
	end
	--obj:magicfall("ȸ��",1,120)
end

function magic_attack(obj)
	local target = obj:frontenemy()
	if target ~= nil then
		target:damage(math.random(1000000,3000000),2,obj)
		target:effect(67,1)
		target:sound(87)
		obj:action(1,20,0)
	end
end
function magic_loudsay(obj)
	if obj.mp < 500 then
		obj:message(3,"������ �����մϴ�.")
		return
	end
	obj:setmp(obj.mp - 500)
	local m = obj:getpos()

end
function magic_mp(obj)
	if obj.mp < 30 then
		obj:message(3,"������ �����մϴ�.")
		return
	end
	obj:setmp(obj.maxmp)
	obj:sethp(math.max(100,obj.hp/2))
	obj:effect(11,21)
	obj:action(6,30,0)
	obj:message(3,"���������� �ܿ����ϴ�.")
end
function magic_hellfire(obj,mg,lv,target)
	if obj.hp < 600 then
		obj:message(3,"������ �����մϴ�.")
		return
	end
	if obj:can_damage(target) then
		obj:action(6,30,0)
		target:effect(8,73)
		target:damage(obj.mp*1.5,2,obj)
		obj:setmp(600)
	else
		obj:message(3,"�ɸ��� �ʽ��ϴ�.")
	end
end


function magic_nuriheal(obj)
	if obj.mp < 30 then
		-- ������ ���� ��ü�� ������ 30 ���϶�� ������ �����ϴٴ� �޼����� ���� ��ũ��Ʈ�� �����Ѵ�
		obj:message(3,"������ �����մϴ�.")
		return
	end
	obj:setmp(obj.mp - 30) -- ���� 30 ����
	obj:effect(5,3)
	obj:healhp(50)
	obj:message(3,"�����Ǳ���� �ܿ����ϴ�.")
end
