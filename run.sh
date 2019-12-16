#!/bin/bash
git pull
ninja query_worker
./query_worker /home/me/index/index/ Index $(ls /home/me/index/index/Index * | wc -l) 23.101.137.53 5790
