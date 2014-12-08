
trademessage = {
	"»ó´ë¹æÀÌ ÀÌ¹Ì ±³È¯ÁßÀÔ´Ï´Ù.",
	"»ó´ë¹æÀÌ ±³È¯°ÅºÎ ÁßÀÔ´Ï´Ù.",
	"±³È¯°ÅºÎ ÁßÀÔ´Ï´Ù.",
}
function onRequestTrade(user,id)
	local u = objects[user]
	local u2 = users[id]
	print(u,u2)
	if u2 == nil then
		u:message(3,id .. "´ÔÀº ¹Ù¶÷ÀÇ ³ª¶ó¿¡ ¾ø½À´Ï´Ù.")
		return
	end

	local r = _trade(user,u2.id)
	if r ~= 0 then
		u:message(3,trademessage[r])
	end
end
function onTradeOnOff(oid)
	local user = objects[oid]
	if user:optCheck(1) then
		user:message(3,"±³È¯ °¡´É OFF")
		user:optUnset(1)
	else
		user:message(3,"±³È¯ °¡´É ON")
		user:optSet(1)
	end
end
function onPKProtect(oid)
	local user = objects[oid]
	if user:optCheck(4) then
		user:message(3,"PKº¸È£ OFF")
		user:optUnset(4)
	else
		user:message(3,"PKº¸È£ ON")
		user:optSet(4)
	end
end
function onGroupOnOff(oid)
	local user = objects[oid]
	if user:optCheck(2) then
		user:message(3,"±×·ìÇã¿ë OFF")
		user:optUnset(2)
	else
		user:message(3,"±×·ìÇã¿ë ON")
		user:optSet(2)
	end
end

function onHelmetVisibleOnOff(oid)
	local user = objects[oid]
	if user:optCheck(64) then
		user:message(3,"Åõ±¸º¸ÀÌ±âë OFF")
		user:optUnset(64)
	else
		user:message(3,"Åõ±¸º¸ÀÌ±â ON")
		user:optSet(64)
	end
end
GroupClass = CreateClass()
function GroupClass:join(user)
	if user.group ~= nil then
		return
	end
	self.members[user.name] = user
	self:message(3,user.name .."´Ô ±×·ìÂü¿©")
	self.count = self.count + 1
end
function GroupClass:delete()
	for k,v in self.members do
		v:message(3,"±×·ì ÇØÃ¼")
		v.group = nil
	end
	_group_delete(self.master.id)
end
function GroupClass:leave(user)
	if user.group == nil or user.group ~= self then return end
	self:message(3,user.name .."´Ô ±×·ì Å»Åğ")
	self.count = self.count - 1
	user.group = nil
	if user == self.master then
		self.master = users[_group_master(user.id)]
		self.members[user.name] = nil
		_group_leave(self.master,user.id)
		if self.count > 1 then
			self:message(3,self.master.name .. "´ÔÀÌ ±×·ìÀåÀÌ µÇ¾ú½À´Ï´Ù.")
		end
	else
		_group_leave(self.master.id,user.id)
	end
	if self.count == 1 then
		self:delete()
	end
end
function GroupClass:message(tp,msg)
	_group_message(self.master.id,tp,msg)
end
function newGroup(user,user2)
	local o = GroupClass.new()
	o.master = user
	o.members = {}
	o.count = 2
	user.group = o
	user2.group = o
	o.members[user.name] = user
	o.members[user2.name] = user2
	_group_make(user.id,user2.id)
	o:message(3,user.name .. "´ÔÀÌ ±×·ìÀåÀÔ´Ï´Ù.")
	o:message(3,user2.name .. "´Ô ±×·ìÂü¿©")
	return o
end
function onRequestGroup(user,id)
	local u = objects[user]

	local u2 = users[id]
	if u2 == nil then
		u:message(3,id .. "´ÔÀº ¹Ù¶÷ÀÇ ³ª¶ó¿¡ ¾ø½À´Ï´Ù.")
		return
	end
	if not u:optCheck(2) then
		u:message(3,"±×·ì °ÅºÎÁßÀÔ´Ï´Ù.")
		return
	elseif not u2:optCheck(2) then
		u:message(3,"»ó´ë¹æÀÌ ±×·ì °ÅºÎÁßÀÔ´Ï´Ù.")
		return
	end
	if u.group == nil and u2.group == nil then
		newGroup(u,u2)
	elseif u.group == u2.group then
		if u.group.master == u then
			u.group:leave(u2)
		else
			u:message(3,"´ç½ÅÀº ±×·ìÀåÀÌ ¾Æ´Õ´Ï´Ù.")
		end
	elseif u.group == nil then
		u:message(3,"»ó´ë¹æÀÌ ÀÌ¹Ì ±×·ì Âü¿©ÁßÀÔ´Ï´Ù.")
	elseif u2.group == nil then
		if u.group.master ~= u then
			u:message(3,"´ç½ÅÀº ±×·ìÀåÀÌ ¾Æ´Õ´Ï´Ù.")
		else
			u.group:join(u2)
		end
	else
		u:message(3,"»ó´ë¹æÀÌ ÀÌ¹Ì ±×·ì Âü¿©ÁßÀÔ´Ï´Ù.")
	end
end
