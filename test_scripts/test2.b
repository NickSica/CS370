implement Test2;

include "sys.m";
FD: import Sys;
sys: Sys;
dfd: ref FD;
include "draw.m";

Test2: module
{
	init: fn(nil: ref Draw->Context, nil: list of string);
};

init(nil: ref Draw->Context, nil: list of string)
{
	sys = load Sys Sys->PATH;

	spawn test1();
	sys->sleep(10000);

	dfd = sys->open("/prog/5/ctl", sys->OWRITE);
	sys->write(dfd, array of byte "kill", 4);
}

test1()
{
	sys = load Sys Sys->PATH;

	i := 0;
	while(1) {
		for(j := 0; j < 100000000; ++j) ;
		++i;
		sys->print("%d\n", i);
	}
}
