###################################################
# Author: Wang shaohua
# Mail: 2726313053
# Created Time: 2025/4/18
###################################################
#!/bin/bash

set -x

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make -j10
