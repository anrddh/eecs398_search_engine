#!/bin/bash
git pull
cd build
ninja query_worker
cd ..
./build/query_worker /home/me/index/index/ Index $(ls /home/me/index/index/Index* | wc -l) 23.101.137.53 5790
