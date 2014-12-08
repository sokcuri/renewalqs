include ( "script/items/basic")
function item_weapon1(ev,user,item,sub,sub2)

	if ev == 1 then
		print("A",math.random(1,100))
		if math.random(1,100) <= 50 then
			sub:effect(188,104)
			return math.random(1000,2000)
		end
		return sub2
	end
	return true
end

function item_nobi(user,item)
	user:warp("주막연실이네",5,5)
end
function item_teleport(user,item)
	user:teleport(8)
end

function item_teleport2(user,item)
	local m,x,y = user:getpos()
	local side = user:side()
	local sx = x
	local sy = y
	_effect_xy(m.id,x,y,279,1)
	for i = 1,6 do
		if side == 0 then
			y = y -1
		elseif side == 1 then
			x = x + 1
		elseif side == 2 then
			y = y + 1
		elseif side == 3 then
			x = x - 1
		end
		if user:canmove(m,x,y) then
			sx = x
			sy = y
		else break
		end
	end
	user:warp(m,sx,sy)
	user:sound(370)
end
