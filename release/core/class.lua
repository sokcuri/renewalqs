function CreateClass(baseClass)
	local newclass = {}
	newclass.__index = newclass

	function newclass:new(o)
		o = o or {}
		return setmetatable(o,newclass)
	end
	function newclass:class()
		return newclass
	end
	function newclass:isa(theClass)
		local cur_class = newclass
		while ( cur_class ~= nil ) do
			if cur_class == theClass then
				return true
			else
				cur_class = cur_class:superClass()
			end
		end
		return false
	end
	function newclass:superClass()
		return baseClass
	end
	if baseClass then
		setmetatable(newclass , baseClass)
	end
	return newclass
end
