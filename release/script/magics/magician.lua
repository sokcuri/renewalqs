
-- magician
function fall_poison(target,obj)
	if target.hp > target.maxhp * 0.05 then
		target:delhp(target.maxhp*0.01)
	end
	target:effect(14)
	return 1
end
function magic_magician_poison(obj,mg,lv,target)
	if obj.mp < 30 then
		obj:message(3,"������ �����մϴ�.")
		return
	elseif check_state(obj) then
		if obj:can_damage(target) then
			if target:checkfall("�ߵ�") then
				obj:message(3,"�̹� �ɷ��ֽ��ϴ�.")
			else
				obj:delmp(30)
				obj:action(6,30,68)
				obj:message(3,"�ߵ��� �ܿ����ϴ�.")
				target:magicfall("�ߵ�",1,180,obj)
			end
		else
			obj:message(3,"�ɸ��� �ʽ��ϴ�.")
		end
	end
end
function unfall_curse(target)
	target:setac(target.ac - 30)
end
function fall_curse(target,obj)
	if target.type == 1 then
		target:message(3,obj.name .. "��(��) ���ָ� ���մϴ�.")
		target:setac(target.ac + 30)
		target:effect(13)
	end
end
function magic_magician_curse(obj,mg,lv,target)
	if obj.mp < 30 then
		obj:message(3,"������ �����մϴ�.")
		return
	elseif check_state(obj) then
		if obj:can_damage(target) then
			obj:delmp(30)
			obj:action(6,30,69)
			obj:message(3,"���ָ� �ܿ����ϴ�.")
			target:magicfall("����",1,180,obj)
		else
			obj:message(3,"�ɸ��� �ʽ��ϴ�.")
		end
	end
end
function magic_magician_fire(obj,mg,lv,target)
	-- ���ǿ��
	if obj.mp < 120 then
		obj:message(3,"������ �����մϴ�.")
		return
	end
	if obj:can_damage(target) then
		local grace = obj.grace or 0
		target:effect(172,40)
		target:damage((420 + grace * 3),2,obj)
		obj:delmp(120)
		obj:action(6,20,0)
		obj:message(3,"���ǿ�⸦ �ܿ����ϴ�.")
	else
		obj:message(3,"�ɸ��� �ʽ��ϴ�.")
	end
end

function magic_magician_ice(obj,mg,lv,target)
	--�󸮱�
	if obj.mp < 120 then
		obj:message(3,"������ �����մϴ�.")
		return
	end
	if obj:can_damage(target) then
		local grace = obj.grace or 0
		target:effect(52,47)
		target:damage((800 + grace * 3),2,obj)
		obj:delmp(120)
		obj:action(6,20,0)
		obj:message(3,"�󸮱⸦ �ܿ����ϴ�.")
	else
		obj:message(3,"�ɸ��� �ʽ��ϴ�.")
	end
end
function magic_magician_bomb(obj,mg,lv,target)
	if obj.mp < 360 then
		obj:message(3,"������ �����մϴ�.")
		return
	end
	if obj:can_damage(target) then
		local grace = obj.grace or 0
		target:effect(188,73)
		target:damage(3600 + grace * 8,2,obj)
		obj:delmp(360)
		obj:action(6,20,0)
		obj:message(3,"���� �ܿ����ϴ�.")
	else
		obj:message(3,"�ɸ��� �ʽ��ϴ�.")
	end
end

function magic_magician_thunder(obj,mg,lv,target)
	if obj.mp < 240 then
		obj:message(3,"������ �����մϴ�.")
		return
	end
	if obj:can_damage(target) then
		local grace = obj.grace or 0
		target:effect(143,58)
		target:damage((1200 + grace * 4),2,obj)
		obj:delmp(120)
		obj:action(6,20,0)
		obj:message(3,"�ڰ� �ܿ����ϴ�.")
	else
		obj:message(3,"�ɸ��� �ʽ��ϴ�.")
	end
end
function magic_magician_wind(obj,mg,lv,target)
	if obj.mp < 240 then
		obj:message(3,"������ �����մϴ�.")
		return
	end

	if obj:can_damage(target) then
		local grace = obj.grace or 0
		target:effect(255,116)
		target:damage(2000 + grace * 5,2,obj)
		obj:delmp(120)
		obj:action(6,20,0)
		obj:message(3,"ǳ�� �ܿ����ϴ�.")
	else
		obj:message(3,"�ɸ��� �ʽ��ϴ�.")
	end
end
