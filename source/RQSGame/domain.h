#pragma once
enum domaintype
{
	user_domain = 0,
	sight_domain = 1,
	map_domain = 2,
	group_domain = 3,
	clan_domain = 4,
	world_domain = 5,
};
struct domain
{
	int type;
};

#define dwset(dm,p) (encrypt(0,p,cryptkey),domain_write(dm,p));

int domain_write(struct domain* dm,char* packet);