
include ( "script/magics/magician")

function fall_collectmp(obj)
	obj:healmp(obj.maxmp * 0.1)
	obj:effect(74,2)
	return 2
end
function magic_collectmp(obj,mg,lv)
	if obj:checkfall("명상") ~= nil then
		obj:message(3,"이미 걸려있습니다.")
		return
	end
	obj:sound(723)
	obj:action(6,30,0)
	obj:message(3,"명상을 외웠습니다.")
	obj:magicfall("명상",1,30)
end
