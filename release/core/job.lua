jobs = jobs or {}
function job_init(job,joblv)
	_jobmax_set(job,joblv)
	for i = 0,job-1 do
		jobs[i] = {}
		jobs[i].needexp = {}
		jobs[i].uphp = {}
		jobs[i].upmp = {}
		jobs[i].upmight = {}
		jobs[i].upgrace = {}
		jobs[i].upwill = {}
		jobs[i].upac = {}

	end
	local file = io.open("db/class.txt")
	if file ~= nil then
		for v in file:lines() do
			v = string.gsub(v,";.*","")
			local t = string.split(v,"\t")
			if t[1] ~= nil and #t >= 2 then
				--t[1] = tonumber(t[1])

				if t[1] == "class" then
					local j = jobs[tonumber(t[2])]
					for i = 3,#t do
						j[i-3] = t[i]
						_job_set(tonumber(t[2]),i-3,t[i])
					end
				else
					local j = tonumber(t[1])
					local lv = tonumber(t[2])
					local exp = tonumber(t[3])
					local hp = tonumber(t[4]) or 0
					local mp = tonumber(t[5]) or 0
					local might = tonumber(t[6]) or 0
					local grace = tonumber(t[7]) or 0
					local will = tonumber(t[8]) or 0
					local ac = tonumber(t[9]) or 0
					jobs[j].needexp[lv] = exp
					jobs[j].uphp[lv] = hp
					jobs[j].upmp[lv] = mp
					jobs[j].upmight[lv] = might
					jobs[j].upgrace[lv] = grace
					jobs[j].upwill[lv] = will
					jobs[j].upac[lv] = ac
				end
			end
		end
	end
end
function get_jobup(job,lv)
	return jobs[job].uphp[lv],jobs[job].upmp[lv],jobs[job].upmight[lv],jobs[job].upgrace[lv],jobs[job].upwill[lv],jobs[job].upac[lv]
end
function get_needexp(job,lv)
	return jobs[job].needexp[lv+1]
end
