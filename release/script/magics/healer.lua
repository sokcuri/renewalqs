function unfall_honma(target)
	target:setac(target.ac - 40)
end
function fall_honma(target,obj)
	if target.type == 1 then
		target:message(3,obj.name .. "��(��) ȥ������ ���մϴ�.")
		target:setac(target.ac + 40)
		target:effect(39)
	end
end
function magic_healer_honma(obj,mg,lv,target)
	if obj.mp < 30 then
		obj:message(3,"������ �����մϴ�.")
		return
	elseif check_state(obj) then
		if obj:can_damage(target) then
			obj:delmp(30)
			obj:action(6,30,79)
			obj:message(3,"ȥ������ �ܿ����ϴ�.")
			target:magicfall("ȥ��",1,180,obj)
		else
			obj:message(3,"�ɸ��� �ʽ��ϴ�.")
		end
	end
end
