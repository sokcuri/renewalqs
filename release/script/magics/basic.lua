
include ( "script/magics/magician")

function fall_collectmp(obj)
	obj:healmp(obj.maxmp * 0.1)
	obj:effect(74,2)
	return 2
end
function magic_collectmp(obj,mg,lv)
	if obj:checkfall("���") ~= nil then
		obj:message(3,"�̹� �ɷ��ֽ��ϴ�.")
		return
	end
	obj:sound(723)
	obj:action(6,30,0)
	obj:message(3,"����� �ܿ����ϴ�.")
	obj:magicfall("���",1,30)
end
