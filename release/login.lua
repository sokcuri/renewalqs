require "conf"
multiserver_add("쿠우;쿠우","127.0.0.1",8998)
function filecheck(f)
	local file = io.open(f)
	if file == nil then
		return false
	else
		file:close()
		return true
	end
end

function vartofile(file,name,t)
	if type(t) ~= "table" then
		return false
	end
	--print(file..' open')
	local f = io.open(file,"w")
	if f ~= nil then
		vartofile2(f,name,t)
		io.close(f)
	else return false end
end
function vartofile2(fo,n,t)
	--print(fo,n,t)
	fo:write(n..'={}\n')
	for k,v in pairs(t) do
		local ktype = type(k)
		local vtype = type(v)
		if ktype == "string" then
			k=string.gsub(k,"'","\\'")
			if vtype == "string" then
				v=string.gsub(v,"'","\\'")
				fo:write(n.."['"..k.."']='"..v.."'\n")
			elseif vtype == "number" then
				fo:write(n.."['"..k.."']="..v.."\n")
			elseif vtype == "table" then
				vartofile2(fo,n.."['"..k.."']",v)
			elseif vtype == "boolean" then
				fo:write(n.."['"..k.."']="..tostring(v).."\n")
			end
		elseif ktype == "number" then
			if vtype == "string" then
				v=string.gsub(v,"'","\\'")
				fo:write(n.."["..k.."]='"..v.."'\n")
			elseif vtype == "number" then
				fo:write(n.."["..k.."]="..v.."\n")
			elseif vtype == "table" then
				vartofile2(fo,strings,n.."["..k.."]",v)
			elseif vtype == "boolean" then
				fo:write(n.."["..k.."]="..tostring(v).."\n")
			end
		end
	end
	fo:flush()
end
createud = {}
function onUserCheck(sd,id,pw)
	if not filecheck("user/" .. id .. ".lua") then
		local m = {}
		m.name = id
		m.pw = pw
		send_message(sd,0,"")
		createud[sd] = m
	else
		send_message(sd,14,"이미 존재하는 아이디입니다")
	end
end

function onUserCreate(sd,face,hair,gender,nation,totem)

	local m = createud[sd]
	if m == nil then
		return
	end
	m.face = face
	m.hair = hair
	m.gender = gender
	m.nation = nation
	m.totem = totem
	m.state = 0
	m.haircol = 0
	m.hp = 53
	m.maxhp = 53
	m.mp = 53
	m.maxmp = 53
	m.exp = 0
	m.might = 3
	m.grace = 3
	m.will = 3
	m.hit = 3
	m.dam = 3
	m.money = 0
	m.personality = 0
	m.vote = 0
	m.job = 0
	m.level = 1
	m.joblv = 0
	m.pureac = 100
	m.ac = 100
	m.option = 0
	m.title = ""
	vartofile("user/" .. m.name .. ".lua","user_datas[\"" .. m.name .."\"]",m)
	send_message(sd,0,"이어하기를 눌러주세요")
end
user_datas = {}
function onUserLogin(sd,id,pw)
	if not filecheck("user/" .. id .. ".lua") then
		send_message(sd,14,"아이디가 존재하지 않습니다.")
	elseif chklogin(sd,id) then
		send_message(sd,14,"이미 접속중입니다.")
	else
		dofile("user/" .. id .. ".lua")
		if user_datas[id].pw == pw then
			login(sd,id)
			createud[sd] = nil
		else
			send_message(sd,15,"패스워드가 틀렸습니다.")
		end
	end
end

function onUserPasswordChange(sd,id,pw)

end
