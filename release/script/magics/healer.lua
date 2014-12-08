function unfall_honma(target)
	target:setac(target.ac - 40)
end
function fall_honma(target,obj)
	if target.type == 1 then
		target:message(3,obj.name .. "가(이) 혼마술을 가합니다.")
		target:setac(target.ac + 40)
		target:effect(39)
	end
end
function magic_healer_honma(obj,mg,lv,target)
	if obj.mp < 30 then
		obj:message(3,"마력이 부족합니다.")
		return
	elseif check_state(obj) then
		if obj:can_damage(target) then
			obj:delmp(30)
			obj:action(6,30,79)
			obj:message(3,"혼마술을 외웠습니다.")
			target:magicfall("혼마",1,180,obj)
		else
			obj:message(3,"걸리지 않습니다.")
		end
	end
end
