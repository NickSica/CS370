dd -if dd_test -of '#S'/shoutdata -obs 1
dd -if dd_test -of '#S'/shoutdata -obs 1 -oseek 21
cat '#S'/shoutdata 
